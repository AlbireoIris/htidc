//这是一个多进程的tcp文件服务端
#include "_freecplus.h"

#define TCPBUFLEN 8182

struct st_arg
{
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
} starg;

CTcpServer TcpServer;
CLogFile logfile;

char strRecvBuffer[TCPBUFLEN + 10];
char strSendBuffer[TCPBUFLEN + 10];

int clienttype = 0;

bool _xmltoarg(char *strxmlbuffer);

bool ClientLogin();

bool ListFile();

void FathExit(int sig);

void ChildExit(int sig);

void RecvFilesMain();

void SendFilesMain();

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("\n");
    printf("Using:/htidc/public/bin/tcpfilesservermp_FR logfilename port\n");

    printf("Example:/htidc/public/bin/tcpfileserrvermp_FR /log/shqx/tcpfileserver.log 5051\n\n");
    printf("本程序是一个公共功能模块，采用TCP/IP传输文件的服务端。\n");
    printf("logfilename 日志文件名。\n");
    printf("port 用于传输文件的TCP端口。\n");

    return -1;
  }

  CloseIOAndSignal();
  signal(SIGINT, FathExit);
  signal(SIGTERM, FathExit);

  //日志不能自动切换，多线性，会出现日志不一致的问题
  if (logfile.Open(argv[1], "a+", false) == false)
  {
    printf("logfile.Open %s failed!\n", argv[1]);
    return -1;
  }

  logfile.Write("Fileserver Start %s \n", argv[2]);

  if (TcpServer.InitServer(atoi(argv[2])) == false)
  {
    logfile.Write("TcpServer.InitServer %s failed!\n", argv[2]);
    exit(-1);
  }
  logfile.Write("TcpServer.InitServer %s Success!\n", argv[2]);

  while (true)
  {
    //等待客户端连接
    if (TcpServer.Accept() == false)
    {
      logfile.Write("TcpServer.Accept() failed!\n");
      continue;
    }
    logfile.Write("TcpServer.Accept(%s) Success!\n", TcpServer.GetIP());

    if (fork() > 0)
    {
      //父进程关闭socket连接，等待Accept()
      TcpServer.CloseClient();
      continue;
    }

    signal(SIGINT, ChildExit);
    signal(SIGTERM, ChildExit);

    TcpServer.CloseListen();

    if (ClientLogin() == false)
      ChildExit(0);

    if (clienttype == 1)
      RecvFilesMain();

    if (clienttype == 2)
      SendFilesMain();

    ChildExit(0);
  }

  return 0;
}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg, 0, sizeof(struct st_arg));
  GetXMLBuffer(strxmlbuffer, "ip", starg.ip);
  GetXMLBuffer(strxmlbuffer, "port", &starg.port);
  GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
  GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
  GetXMLBuffer(strxmlbuffer, "clientpathbak", starg.clientpathbak);
  GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
  GetXMLBuffer(strxmlbuffer, "srvpathbak", starg.srvpathbak);
  GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);
  GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
  GetXMLBuffer(strxmlbuffer, "okfilename", starg.okfilename);
  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);

  return true;
}

bool ClientLogin()
{
  memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
  memset(strSendBuffer, 0, sizeof(strSendBuffer));

  if (TcpServer.Read(strRecvBuffer, 20) == false)
  {
    logfile.Write("ClientLogin()/TcpServer.Read() failed!\n");
    return false;
  }
  //logfile.Write("ClientLogin()/TcpServer.Read() Success!\n");

  GetXMLBuffer(strRecvBuffer, "clienttype", &clienttype);

  if ((clienttype == 1) || (clienttype == 2))
    strcpy(strSendBuffer, "TcpServer.Read clienttype Success");
  else
    strcpy(strSendBuffer, "TcpServer.Read clienttype Failed");

  if (TcpServer.Write(strSendBuffer) == false)
  {
    logfile.Write("ClientLogin()/TcpServer.Write() failed!\n");
    return false;
  }
  //logfile.Write("ClientLogin()/TcpServer.Write() Success!\n");

  if (strcmp(strSendBuffer, "TcpServer.Read clienttype Failed") == 0)
    return false;

  _xmltoarg(strRecvBuffer);

  logfile.Write("%s login Success!\n", TcpServer.GetIP());

  return true;
}

void FathExit(int sig)
{
  if (sig > 0)
  {
    signal(sig, SIG_IGN);
    logfile.Write("Catching the signal %d\n", sig);
  }

  TcpServer.CloseListen();

  kill(0, 15);

  logfile.Write("FileServer Exit\n");

  exit(0);
}

void ChildExit(int sig)
{
  if (sig > 0)
    signal(sig, SIG_IGN);

  TcpServer.CloseClient();

  exit(0);
}

void RecvFilesMain()
{
  while (true)
  {
    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
    memset(strSendBuffer, 0, sizeof(strSendBuffer));

    if (TcpServer.Read(strRecvBuffer, 30) == false)
    {
      logfile.Write("RecvFilesMain()/TcpServer.Read() failed!\n");
      ChildExit(-1);
    }

    if (strstr(strRecvBuffer, "activetest") != 0)
    {
      strcpy(strSendBuffer, "ok");

      if (TcpServer.Write(strSendBuffer) == false)
      {
        logfile.Write("RecvFilesMain()/TcpServer.Write(%s) Failed!\n", TcpServer.GetIP());
        ChildExit(-1);
      }

      logfile.Write("%s Beating ...\n", TcpServer.GetIP());

      continue;
    }

    struct st_fileinfo stfileinfo;

    memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

    GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
    GetXMLBuffer(strRecvBuffer, "filesize", &stfileinfo.filesize);
    GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

    UpdateStr(stfileinfo.filename, starg.clientpath, starg.srvpath, false);

    if (RecvFile(TcpServer.m_connfd, &stfileinfo, &logfile) == false)
    {
      logfile.Write("RecvFilesMain()/RecvFile() failed!\n");
      ChildExit(-1);
    }

    logfile.Write("Recv %s Success!\n", stfileinfo.filename);
  }
}

void SendFilesMain()
{
  while (true)
  {
    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));

    //获取Tcp报文
    if (TcpServer.Read(strRecvBuffer, 30) == false)
    {
      logfile.Write("SendFilesMain()/TcpServer.Read() failed!\n");
      ChildExit(-1);
    }
    //logfile.Write("SendFilesMain()/TcpServer.Read(%s) Success!\n",strRecvBuffer);

    //响应心跳报文
    if (strstr(strRecvBuffer, "activetest") != NULL)
    {
      memset(strSendBuffer, 0, sizeof(strSendBuffer));
      strcpy(strSendBuffer, "ok");

      if (TcpServer.Write(strSendBuffer) == false)
      {
        logfile.Write("SendFilesMain()/TcpServer.Write() failed!\n");
        ChildExit(-1);
      }
      logfile.Write("%s Beating...\n", TcpServer.GetIP());
      continue;
    }

    //响应list命令
    if (strcmp(strRecvBuffer, "<list>") == 0)
    {
      if (ListFile() == false)
      {
        logfile.Write("SendFilesMain()/ListFile() failed!\n");
        ChildExit(-1);
      }
      //      logfile.Write("SendFilesMain()/ListFile() Success\n");
      continue;
    }

    //响应获取文件的请求
    if (strncmp(strRecvBuffer, "<filename>", 10) == 0)
    {
      struct st_fileinfo stfileinfo;

      memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
      GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
      GetXMLBuffer(strRecvBuffer, "filesize", &stfileinfo.filesize);
      GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

      if (SendFile(TcpServer.m_connfd, &stfileinfo, &logfile) == false)
      {
        logfile.Write("Put %s ... Failed!\n", stfileinfo.filename);
        ChildExit(-1);
      }

      logfile.Write("Put %s ... Success!\n", stfileinfo.filename);

      //如果是1什么也不做
      if (starg.ptype == 1)
        ;

      //如果是2删除文件
      if (starg.ptype == 2)
        REMOVE(stfileinfo.filename);

      //如果是3移动到备份目录
      if (starg.ptype == 3)
      {
        char strfilenamebak[256];
        memset(strfilenamebak, 0, sizeof(strfilenamebak));
        strcpy(strfilenamebak, stfileinfo.filename);

        UpdateStr(strfilenamebak, starg.srvpath, starg.srvpathbak, false);

        if (RENAME(stfileinfo.filename, strfilenamebak) == false)
        {
          logfile.Write("Rename %s to %s failed!\n", stfileinfo.filename, strfilenamebak);
          ChildExit(-1);
        }
        logfile.Write("Move %s to %s Success!\n", stfileinfo.filename, strfilenamebak);
      }
    }
  }
}

bool ListFile()
{
  CDir Dir;

  if (Dir.OpenDir(starg.srvpath, starg.matchname, 5000, starg.andchild, false) == false)
  {
    logfile.Write("Dir.OpenDir %s failed!\n", starg.srvpath);
    return false;
  }
  //logfile.Write("Dir.OpenDir %s Success! %d \n",starg.srvpath,Dir.m_vFileName.size());

  memset(strSendBuffer, 0, sizeof(strSendBuffer));

  sprintf(strSendBuffer, "<totalfile>%d</totalfile>", Dir.m_vFileName.size());

  //logfile.Write("TcpServer.Write(%s) ... \n",strSendBuffer);

  if (TcpServer.Write(strSendBuffer) == false)
  {
    logfile.Write("ListFile()/TcpServer.Write(totalfile...) failed!\n");
    return false;
  }
  //logfile.Write("ListFile()/TcpServer.Write(totalfile...) Success!\n");

  while (true)
  {
    if (Dir.ReadDir() == false)
      break;

    memset(strSendBuffer, 0, sizeof(strSendBuffer));
    sprintf(strSendBuffer, "<filename>%s</filename><mtime>%s</mtime><filesize>%d</filesize>", Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);

    if (TcpServer.Write(strSendBuffer) == false)
    {
      logfile.Write("ListFile()/TcpServer.Write() failed!\n");
      return false;
    }
    //logfile.Write("ListFile()/TcpServer.Write(%d) Success!\n",ii+1);
  }
  return true;
}
