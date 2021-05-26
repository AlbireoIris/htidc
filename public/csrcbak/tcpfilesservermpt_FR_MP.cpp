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
    printf("��������һ����������ģ�飬����TCP/IP�����ļ��ķ���ˡ�\n");
    printf("��������õ��Ƕ��̵߳ķ����\n");
    printf("logfilename ��־�ļ�����\n");
    printf("port ���ڴ����ļ���TCP�˿ڡ�\n");

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
      logfile.Write("�����߳�ʧ�ܣ�\n");
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
  char strRecvBuffer[TCPBUFLEN + 10]; // ���ձ��ĵĻ�����
  char strSendBuffer[TCPBUFLEN + 10]; // ���ͱ��ĵĻ�����

  while (true)
  {
    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
    memset(strSendBuffer, 0, sizeof(strSendBuffer));

    if (TcpRead(clientfd, strRecvBuffer, &ibuflen, 80) == false)
    {
      logfile.Write("RecvFilesMain()/TcpRead() Failed.\n");
      return false;
    }

    // ������������
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

    // ��ȡ�����յ��ļ���ʱ��ʹ�С
    GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
    GetXMLBuffer(strRecvBuffer, "filesize", &stfileinfo.filesize);
    GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

    // ���ļ����е�clientpath�滻��srvpath��ҪС�ĵ���������
    UpdateStr(stfileinfo.filename, starg->clientpath, starg->srvpath, false);

    // �����ļ�������
    if (RecvFile(clientfd, &stfileinfo, &logfile) == false)
    {
      logfile.Write("RecvFilesMain()/RecvFile() Failed.\n");
      return false;
    }

    logfile.Write("Recv %s Success!\n", stfileinfo.filename);
  }
  return true;
}

// �����ļ�������
bool SendFilesMain(int clientfd, struct st_arg *starg)
{
  int ibuflen = 0;
  char strRecvBuffer[TCPBUFLEN + 10]; // ���ձ��ĵĻ�����
  char strSendBuffer[TCPBUFLEN + 10]; // ���ͱ��ĵĻ�����

  while (true)
  {
    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
    if (TcpRead(clientfd, strRecvBuffer, &ibuflen, 30) == false)
    {
      logfile.Write("SendFilesMain()/TcpRead() Failed.\n");
      return false;
    }

    // ������������
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

    // �����ȡ�ļ��б���
    if (strcmp(strRecvBuffer, "<list>") == 0)
    {
      if (ListFile(clientfd, starg) == false)
      {
        logfile.Write("SendFilesMain()/ListFile() Failed.\n");
        return false;
      }
      continue;
    }

    // ȡ�ļ�����
    if (strncmp(strRecvBuffer, "<filename>", 10) == 0)
    {
      // ��ȡ�����յ��ļ���ʱ��ʹ�С
      struct st_fileinfo stfileinfo;
      memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
      GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
      GetXMLBuffer(strRecvBuffer, "filesize", &stfileinfo.filesize);
      GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

      logfile.Write("Putting %s ... ", stfileinfo.filename);

      // ���ļ����͸��ͻ���
      if (SendFile(clientfd, &stfileinfo, &logfile) == false)
      {
        logfile.WriteEx("Failed!\n");
        return false;
      }

      logfile.WriteEx("Success!\n");

      //ɶҲ����
      if (starg->ptype == 1)
        ;

      // ɾ������˵��ļ�
      if (starg->ptype == 2)
        REMOVE(stfileinfo.filename);

      // ���ݷ���˵��ļ�
      if (starg->ptype == 3)
      {
        char strfilenamebak[256];
        memset(strfilenamebak, 0, sizeof(strfilenamebak));
        strcpy(strfilenamebak, stfileinfo.filename);

        UpdateStr(strfilenamebak, starg->srvpath, starg->srvpathbak, false); // ҪС�ĵ���������

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

// �г�srvpathĿ¼���ļ����嵥�����ظ��ͻ��ˡ�
bool ListFile(int clientfd, struct st_arg *starg)
{
  int ibuflen = 0;
  char strRecvBuffer[TCPBUFLEN + 10]; // ���ձ��ĵĻ�����
  char strSendBuffer[TCPBUFLEN + 10]; // ���ͱ��ĵĻ�����

  CDir Dir;

  // ע�⣬���Ŀ¼�µ����ļ�������50000�����������ļ����ܽ�������
  if (Dir.OpenDir(starg->srvpath, starg->matchname, 5000, starg->andchild, false) == false)
  {
    logfile.Write("Dir.OpenDir(%s) ʧ�ܡ�\n", starg->srvpath);
    return false;
  }

  // �Ȱ��ļ��������ظ��ͻ���
  memset(strSendBuffer, 0, sizeof(strSendBuffer));
  sprintf(strSendBuffer, "<totalfile>%d</totalfile>", Dir.m_vFileName.size());

  if (TcpWrite(clientfd, strSendBuffer) == false)
  {
    logfile.Write("ListFile()/TcpWrite(TotalFile) Failed.\n");
    return false;
  }

  // ���ļ���Ϣһ�����ķ��ظ��ͻ���
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

//��ͻ���ͨ�Ž��̵�������
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
