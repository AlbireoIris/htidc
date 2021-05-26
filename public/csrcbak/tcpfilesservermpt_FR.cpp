#include "_freecplus.h"

#define TCPBUFLEN 8182

struct st_arg
{
  int clienttype;
  char ip[32];
  int port;
  int ptype;
  char clientpath[256];
  char clientpathbak[256];
  char srvpath[256];
  char srvpathbak[256];
  bool andchild;
  char matchname[256];
  char okfilename[256];
  int timetvl;
};

CLogFile logfile;

bool _xmltoarg(char *strxmlbuffer, struct st_arg *starg);

bool ClientLogin(int clientfd, struct st_arg *starg);

bool ListFile(int clientfd, struct st_arg *starg);

void Exit(int sig);

void *pth_main(void *arg);

bool RecvFilesMain(int clientfd, struct st_arg *starg);

bool SendFilesMain(int clientfd, struct st_arg *starg);

vector<int> vclientfd;

void AddClient(int clientfd);

void RemoveClient(int clientfd);

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("\n");
    printf("Using:/htidc/public/bin/tcpfileserver1 logfilename port\n");
    printf("Example:/htidc/public/bin/tcpfilesservermpt_FR /log/shqx/tcpfilesservermpt_FR.log 5051\n\n");
    printf("本程序是一个公共功能模块，采用TCP/IP传输文件的服务端。\n");
    printf("本程序采用的是多线程的服务端\n");
    printf("logfilename 日志文件名。\n");
    printf("port 用于传输文件的TCP端口。\n");

    return -1;
  }

  CloseIOAndSignal();
  signal(SIGINT, Exit);
  signal(SIGTERM, Exit);

  if (logfile.Open(argv[1], "a+", false) == false)
  {
    printf("logfile.Open(%s) failed.\n", argv[1]);
    return -1;
  }

  logfile.Write("FileServer Start %s\n", argv[2]);

  CTcpServer TcpServer;

  if (TcpServer.InitServer(atoi(argv[2])) == false)
  {
    logfile.Write("TcpServer InitServer %s Failed!\n", argv[2]);
    return -1;
  }

  AddClient(TcpServer.m_listenfd);

  while (true)
  {
    if (TcpServer.Accept() == false)
    {
      logfile.Write("TcpServer.Accept() Failed!\n");
      continue;
    }

    pthread_t pthid;

    if (pthread_create(&pthid, NULL, pth_main, (void *)(long)TcpServer.m_connfd) != 0)
    {
      logfile.Write("创建线程失败！\n");
      close(TcpServer.m_connfd);
      Exit(-1);
    }

    logfile.Write("%s is Connected!\n", TcpServer.GetIP());

    AddClient(TcpServer.m_connfd);
  }
  return 0;
}

void Exit(int sig)
{
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  if (sig > 0)
    signal(sig, SIG_IGN);

  logfile.Write("TcpServer_mpth Exit(%d)...\n", sig);

  for (int acc = 0; acc < vclientfd.size(); acc++)
    close(vclientfd[acc]);

  exit(0);
}

bool ClientLogin(int clientfd, struct st_arg *starg)
{
  int ibuflen = 0;
  char strRecvBuffer[TCPBUFLEN + 10];
  char strSendBuffer[TCPBUFLEN + 10];

  memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
  memset(strSendBuffer, 0, sizeof(strSendBuffer));

  if (TcpRead(clientfd, strRecvBuffer, &ibuflen, 20) == false)
  {
    logfile.Write("ClientLogin()/TcpRead() Failed!\n");
    return false;
  }

  GetXMLBuffer(strRecvBuffer, "clienttype", &(starg->clienttype));

  if ((starg->clienttype == 1) || (starg->clienttype == 2))
    strcpy(strSendBuffer, "ok");
  else
    strcpy(strSendBuffer, "failed");

  if (TcpWrite(clientfd, strSendBuffer) == false)
  {
    logfile.Write("ClientLogin()/TcpWrite() Failed!\n");
    return false;
  }

  logfile.Write("Login %s (clienttype = %d)\n", strSendBuffer, starg->clienttype);

  if (strcmp(strSendBuffer, "failed") == 0)
    return false;

  _xmltoarg(strRecvBuffer, starg);

  return true;
}

bool RecvFilesMain(int clientfd, struct st_arg *starg)
{
  int ibuflen = 0;
  char strRecvBuffer[TCPBUFLEN + 10]; // 接收报文的缓冲区
  char strSendBuffer[TCPBUFLEN + 10]; // 发送报文的缓冲区

  while (true)
  {
    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
    memset(strSendBuffer, 0, sizeof(strSendBuffer));

    if (TcpRead(clientfd, strRecvBuffer, &ibuflen, 80) == false)
    {
      logfile.Write("RecvFilesMain()/TcpRead() Failed.\n");
      return false;
    }

    // 处理心跳报文
    if (strstr(strRecvBuffer, "activetest") != 0)
    {
      strcpy(strSendBuffer, "ok");
      if (TcpWrite(clientfd, strSendBuffer) == false)
      {
        logfile.Write("RecvFilesMain()/TcpWrite() Failed.\n");
        return false;
      }
      continue;
    }

    struct st_fileinfo stfileinfo;
    memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

    // 获取待接收的文件的时间和大小
    GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
    GetXMLBuffer(strRecvBuffer, "filesize", &stfileinfo.filesize);
    GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

    // 把文件名中的clientpath替换成srvpath，要小心第三个参数
    UpdateStr(stfileinfo.filename, starg->clientpath, starg->srvpath, false);

    // 接收文件的内容
    if (RecvFile(clientfd, &stfileinfo, &logfile) == false)
    {
      logfile.Write("RecvFilesMain()/RecvFile() Failed.\n");
      return false;
    }

    logfile.Write("Recv %s Success!\n", stfileinfo.filename);
  }
  return true;
}

// 发送文件主函数
bool SendFilesMain(int clientfd, struct st_arg *starg)
{
  int ibuflen = 0;
  char strRecvBuffer[TCPBUFLEN + 10]; // 接收报文的缓冲区
  char strSendBuffer[TCPBUFLEN + 10]; // 发送报文的缓冲区

  while (true)
  {
    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
    if (TcpRead(clientfd, strRecvBuffer, &ibuflen, 30) == false)
    {
      logfile.Write("SendFilesMain()/TcpRead() Failed.\n");
      return false;
    }

    // 处理心跳报文
    if (strstr(strRecvBuffer, "activetest") != 0)
    {
      memset(strSendBuffer, 0, sizeof(strSendBuffer));
      strcpy(strSendBuffer, "ok");

      if (TcpWrite(clientfd, strSendBuffer) == false)
      {
        logfile.Write("SendFilesMain()/TcpWrite() Failed.\n");
        return false;
      }

      //logfile.Write("%s Beating...\n",TcpServer.GetIP());

      continue;
    }

    // 处理获取文件列表报文
    if (strcmp(strRecvBuffer, "<list>") == 0)
    {
      if (ListFile(clientfd, starg) == false)
      {
        logfile.Write("SendFilesMain()/ListFile() Failed.\n");
        return false;
      }
      continue;
    }

    // 取文件报文
    if (strncmp(strRecvBuffer, "<filename>", 10) == 0)
    {
      // 获取待接收的文件的时间和大小
      struct st_fileinfo stfileinfo;
      memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
      GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
      GetXMLBuffer(strRecvBuffer, "filesize", &stfileinfo.filesize);
      GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

      logfile.Write("Putting %s ... ", stfileinfo.filename);

      // 把文件发送给客户端
      if (SendFile(clientfd, &stfileinfo, &logfile) == false)
      {
        logfile.WriteEx("Failed!\n");
        return false;
      }

      logfile.WriteEx("Success!\n");

      //啥也不干
      if (starg->ptype == 1)
        ;

      // 删除服务端的文件
      if (starg->ptype == 2)
        REMOVE(stfileinfo.filename);

      // 备份服务端的文件
      if (starg->ptype == 3)
      {
        char strfilenamebak[256];
        memset(strfilenamebak, 0, sizeof(strfilenamebak));
        strcpy(strfilenamebak, stfileinfo.filename);

        UpdateStr(strfilenamebak, starg->srvpath, starg->srvpathbak, false); // 要小心第三个参数

        if (RENAME(stfileinfo.filename, strfilenamebak) == false)
        {
          logfile.Write("RENAME %s to %s Failed.\n", stfileinfo.filename, strfilenamebak);
          return false;
        }
      }
    }
  }
  return true;
}

bool _xmltoarg(char *strxmlbuffer, struct st_arg *starg)
{
  GetXMLBuffer(strxmlbuffer, "ip", starg->ip);
  GetXMLBuffer(strxmlbuffer, "port", &starg->port);
  GetXMLBuffer(strxmlbuffer, "ptype", &starg->ptype);
  GetXMLBuffer(strxmlbuffer, "clientpath", starg->clientpath);
  GetXMLBuffer(strxmlbuffer, "clientpathbak", starg->clientpathbak);
  GetXMLBuffer(strxmlbuffer, "srvpath", starg->srvpath);
  GetXMLBuffer(strxmlbuffer, "srvpathbak", starg->srvpathbak);
  GetXMLBuffer(strxmlbuffer, "andchild", &starg->andchild);
  GetXMLBuffer(strxmlbuffer, "matchname", starg->matchname);
  GetXMLBuffer(strxmlbuffer, "okfilename", starg->okfilename);
  GetXMLBuffer(strxmlbuffer, "timetvl", &starg->timetvl);

  return true;
}

// 列出srvpath目录下文件的清单，返回给客户端。
bool ListFile(int clientfd, struct st_arg *starg)
{
  int ibuflen = 0;
  char strRecvBuffer[TCPBUFLEN + 10]; // 接收报文的缓冲区
  char strSendBuffer[TCPBUFLEN + 10]; // 发送报文的缓冲区

  CDir Dir;

  // 注意，如果目录下的总文件数超过50000，增量发送文件功能将有问题
  if (Dir.OpenDir(starg->srvpath, starg->matchname, 5000, starg->andchild, false) == false)
  {
    logfile.Write("Dir.OpenDir(%s) 失败。\n", starg->srvpath);
    return false;
  }

  // 先把文件总数返回给客户端
  memset(strSendBuffer, 0, sizeof(strSendBuffer));
  sprintf(strSendBuffer, "<totalfile>%d</totalfile>", Dir.m_vFileName.size());

  if (TcpWrite(clientfd, strSendBuffer) == false)
  {
    logfile.Write("ListFile()/TcpWrite(TotalFile) Failed.\n");
    return false;
  }

  // 把文件信息一条条的返回给客户端
  while (true)
  {
    if (Dir.ReadDir() == false)
      break;

    memset(strSendBuffer, 0, sizeof(strSendBuffer));
    sprintf(strSendBuffer, "<filename>%s</filename><mtime>%s</mtime><filesize>%d</filesize>", Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);

    if (TcpWrite(clientfd, strSendBuffer) == false)
    {
      logfile.Write("ListFile()/TcpWrite(FileInfoXML) Failed.\n");
      return false;
    }
  }
  return true;
}

//与客户端通信进程的主函数
void *pth_main(void *arg)
{
  int clientfd = (long)arg;

  struct st_arg starg;

  memset(&starg, 0, sizeof(struct st_arg));

  if (ClientLogin(clientfd, &starg) == false)
  {
    RemoveClient(clientfd);
    pthread_exit(0);
  }

  if (starg.clienttype == 1)
  {
    if (RecvFilesMain(clientfd, &starg) == false)
    {
      RemoveClient(clientfd);
      pthread_exit(0);
    }
  }

  if (starg.clienttype == 2)
  {
    if (SendFilesMain(clientfd, &starg) == false)
    {
      RemoveClient(clientfd);
      pthread_exit(0);
    }
  }

  RemoveClient(clientfd);

  pthread_exit(0);
}

void AddClient(int clientfd)
{
  vclientfd.push_back(clientfd);
}

void RemoveClient(int clientfd)
{
  for (int acc = 0; acc < vclientfd.size(); acc++)
  {
    if (vclientfd[acc] == clientfd)
    {
      close(clientfd);
      vclientfd.erase(vclientfd.begin() + acc);
      return;
    }
  }
}
