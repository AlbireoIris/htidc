#include "_freecplus.h"
#define TCPBUFLEN 8182
struct st_arg
{
  char ip[32];             //��������ip
  int port;                //�������Ķ˿�
  int ptype;               //�ļ����ͺ��ļ��Ĵ���ʽ 1-���� 2-ɾ�� 3-�ƶ�������Ŀ¼
  char clientpath[256];    //�����ļ���ŵĸ�Ŀ¼
  char clientpathbak[256]; //�����ļ����ݵĸ�Ŀ¼������ѡ��Ϊ3ʱ��Ч
  char srvpath[256];       //������ļ���ŵĸ�Ŀ¼
  bool andchild;           //�Ƿ���clientpathĿ¼�¸�������Ŀ¼���ļ���true-�� false-��
  char matchname[256];     //�������ļ�����ƥ�䷽ʽ ��"*.txt"
  char okfilename[256];    //�ѳɹ������ļ������嵥
  int timetvl;             //ɨ�豾��Ŀ¼�ļ���ʱ���� ��λ��
} starg;

char strRecvBuffer[TCPBUFLEN + 10];
char strSendBuffer[TCPBUFLEN + 10];

vector<struct st_fileinfo> vlistfile, vlistfile1;
vector<struct st_fileinfo> vokfilename, vokfilename1;

CTcpClient TcpClient;
CLogFile logfile;

void _help(char *argv[]);

void EXIT(int sig);

bool _xmltoarg(char *strxmlbuffer);

//���������������¼�������ģ����δ���ӵ���������һֱ�ڴ˺�����ѭ��
bool ClientLogin(const char *argv);

//��������ǳ���������
bool _tcpputfiles();

//��������ǽ�clientpathĿ¼�µ��ļ����ص�vlistfile������
bool LoadListFile();

//��okfilename�ļ����ݼ��ص�vokfilename������
bool LoadOKFileName();

//ͨ�����������Ƚϣ������ɼ����ļ�
bool CompVector();

//��okfilenameд������
bool WriteToOKFileName();

//׷���ļ���okfilename��
bool AppendToOKFileName(struct st_fileinfo *stfileinfo);

//�������ĺ���
bool ActiveTest();

int main(int argc, char *argv[])
{
  //��ʾ��Ϣ
  if (argc != 3)
  {
    _help(argv);
    return -1;
  }

  //�ر������źź����
  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  //�ȴ���־�ļ��ڹر�IO
  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("����־�ļ� %s ʧ�ܣ�\n", argv[1]);
    return -1;
  }

  if (_xmltoarg(argv[2]) == false)
  {
    logfile.Write("_xmltoarg() Failed!\n");
    return -1;
  }

  while (true)
  {
    ClientLogin(argv[2]);

    _tcpputfiles();

    if (vlistfile.size() == 0)
    {
      if (ActiveTest() == false)
        logfile.Write("ActiveTest() Failed\n");
      else
        logfile.Write("Beating...\n");

      sleep(starg.timetvl);
    }
  }
  return 0;
}

void _help(char *argv[])
{
  printf("\n");
  printf("Using:/htidc/public/bin/tcpputfiles logfilename xmlbuffer\n\n");

  printf("Sample:/htidc/public/bin/tcpputfiles_FR /log/shqx/tcpputfiles_surfdata.log \"<ip>119.29.18.109</ip><port>5051</port><ptype>1</ptype><clientpath>/data/shqx/tcpclient/tcpput</clientpath><clientpathbak>/data/shqx/tcpclient/tcpputbak</clientpathbak><srvpath>/data/shqx/tcpserver/surfdata</srvpath><andchild>true</andchild><matchname>SURF_*.TXT,*.DAT</matchname><okfilename>/data/shqx/tcpclient/tcplist/tcpputfiles_surfdata.xml</okfilename><timetvl>10</timetvl>\"\n\n\n");

  printf("���������������ĵĹ�������ģ�飬����TCPЭ����ļ����͸�����ˡ�\n");
  printf("logfilename   ���������е���־�ļ���\n");
  printf("xmlbuffer     ���������еĲ��������£�\n");
  printf("ip            �������˵�IP��ַ��\n");
  printf("port          �������˵Ķ˿ڡ�\n");
  printf("ptype         �ļ����ͳɹ���Ĵ���ʽ��1-�����ļ���2-ɾ���ļ���3-�ƶ�������Ŀ¼��\n");
  printf("clientpath    �����ļ���ŵĸ�Ŀ¼��\n");
  printf("clientpathbak �ļ��ɹ����ͺ󣬱����ļ����ݵĸ�Ŀ¼����ptype==3ʱ��Ч��ȱʡΪ�ա�\n");
  printf("srvpath       ������ļ���ŵĸ�Ŀ¼��\n");
  printf("andchild      �Ƿ���clientpathĿ¼�¸�����Ŀ¼���ļ���true-�ǣ�false-��ȱʡΪfalse��\n");
  printf("matchname     �������ļ�����ƥ�䷽ʽ����\"*.TXT,*.XML\"��ע���ô�д��\n");
  printf("okfilename    �ѷ��ͳɹ��ļ����嵥��ȱʡΪ�ա�\n");
  printf("timetvl       ɨ�豾��Ŀ¼�ļ���ʱ��������λ���룬ȡֵ��1-50֮�䡣\n\n\n");
}

void EXIT(int sig)
{
  logfile.Write_P("�����˳���sig=%d", sig);

  TcpClient.Close();

  exit(0);
}

bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg, 0, sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer, "ip", starg.ip);
  if (strlen(starg.ip) == 0)
  {
    logfile.Write("ip is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "port", &starg.port);
  if (starg.port == 0)
  {
    logfile.Write("port is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
  if ((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3))
  {
    logfile.Write("ptype not in (1,2,3).\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
  if (strlen(starg.clientpath) == 0)
  {
    logfile.Write("clientpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "clientpathbak", starg.clientpathbak);
  if ((starg.ptype == 3) && (strlen(starg.clientpathbak) == 0))
  {
    logfile.Write("clientpathbak is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
  if (strlen(starg.srvpath) == 0)
  {
    logfile.Write("srvpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);

  GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
  if (strlen(starg.matchname) == 0)
  {
    logfile.Write("matchname is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "okfilename", starg.okfilename);
  if ((starg.ptype == 1) && (strlen(starg.okfilename) == 0))
  {
    logfile.Write("okfilename is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
  if (starg.timetvl == 0)
  {
    logfile.Write("timetvl is null.\n");
    return false;
  }

  if (starg.timetvl > 50)
    starg.timetvl = 50;

  return true;
}

bool ClientLogin(const char *argv)
{
  //������һ����ѭ�����������m_sockfd����0���ʾ��������
  if (TcpClient.m_sockfd > 0)
    return true;

  int acc = 0;

  while (true)
  {
    //��һ�ν���ѭ��ʱ������
    if (acc++ > 0)
      sleep(20);

    if (TcpClient.ConnectToServer(starg.ip, starg.port) == false)
    {
      logfile.Write("ClientLogin()/TcpClient.ConnectToServer %s %d failed!\n", starg.ip, starg.port);
      continue;
    }

    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
    memset(strSendBuffer, 0, sizeof(strSendBuffer));

    strcpy(strSendBuffer, argv);
    //��ʾ����Recvģʽ
    strcat(strSendBuffer, "<clienttype>1</clienttype>");

    if (TcpClient.Write(strSendBuffer) == false)
    {
      logfile.Write("ClientLogin()/TcpClient.Write() failed\n");
      continue;
    }

    if (TcpClient.Read(strRecvBuffer, 30) == false)
    {
      logfile.Write("ClientLogin()/TcpClient.Read() failed!\n");
      continue;
    }
    break;
  }

  logfile.Write("Login(%s %d) Success!\n", starg.ip, starg.port);

  return true;
}

bool _tcpputfiles()
{
  if (LoadListFile() == false)
  {
    logfile.Write("_tcpputfiles()/LoadListFile() failed!\n");
    return false;
  }

  if (starg.ptype == 1)
  {
    LoadOKFileName();

    CompVector();

    WriteToOKFileName();

    vlistfile.clear();
    vlistfile.swap(vlistfile1);
  }

  for (int acc = 0; acc < vlistfile.size(); acc++)
  {
    logfile.Write("Putting %s ...\n", vlistfile[acc].filename);

    if (SendFile(TcpClient.m_sockfd, &vlistfile[acc], &logfile) == false)
    {
      logfile.Write("_tcpputfiles()/SendFile() failed!\n");
      TcpClient.Close();
      return false;
    }

    logfile.WriteEx("ok\n");

    if (starg.ptype == 1)
      AppendToOKFileName(&vlistfile[acc]);

    if (starg.ptype == 2)
      REMOVE(vlistfile[acc].filename);

    if (starg.ptype == 3)
    {
      char strfilenamebak[256];
      memset(strfilenamebak, 0, sizeof(strfilenamebak));
      strcpy(strfilenamebak, vlistfile[acc].filename);
      UpdateStr(strfilenamebak, starg.clientpath, starg.clientpathbak, false);
      if (RENAME(vlistfile[acc].filename, strfilenamebak) == false)
      {
        logfile.Write("_tcpputfiles().if(starg.ptype == 3)/RENAME %s to %s failed!\n", vlistfile[acc].filename, strfilenamebak);
        return false;
      }
    }
  }
  return true;
}

bool LoadListFile()
{
  vlistfile.clear();

  CDir Dir;

  if (Dir.OpenDir(starg.clientpath, starg.matchname, 5000, starg.andchild, false) == false)
  {
    logfile.Write("_tcpputfiles()/LoadListFile()/OpenDir() %s Failed!\n", starg.clientpath);
    return false;
  }

  struct st_fileinfo stfileinfo;

  while (true)
  {
    memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
    if (Dir.ReadDir() == false)
      break;

    strcpy(stfileinfo.filename, Dir.m_FullFileName);
    strcpy(stfileinfo.mtime, Dir.m_ModifyTime);
    stfileinfo.filesize = Dir.m_FileSize;

    vlistfile.push_back(stfileinfo);
  }

  return true;
}

bool LoadOKFileName()
{
  vokfilename.clear();

  CFile File;

  if (File.Open(starg.okfilename, "r") == false)
    return true;

  struct st_fileinfo stfileinfo;

  char strbuffer[256];

  while (true)
  {
    memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

    if (File.Fgets(strbuffer, 255, true) == false)
      break;

    GetXMLBuffer(strbuffer, "filename", stfileinfo.filename, 300);
    GetXMLBuffer(strbuffer, "mtime", stfileinfo.mtime, 20);

    vokfilename.push_back(stfileinfo);
  }
  return true;
}

bool CompVector()
{
  vokfilename1.clear();
  vlistfile1.clear();

  for (int acc = 0; acc < vlistfile.size(); acc++)
  {
    int jj = 0;
    for (jj = 0; jj < vokfilename.size(); jj++)
    {
      if ((strcmp(vlistfile[acc].filename, vokfilename[jj].filename) == 0) && (strcmp(vlistfile[acc].mtime, vokfilename[jj].mtime) == 0))
      {
        vokfilename1.push_back(vlistfile[acc]);
        break;
      }
    }

    if (jj == vokfilename.size())
      vlistfile1.push_back(vlistfile[acc]);
  }
  return true;
}

bool WriteToOKFileName()
{
  CFile File;

  if (File.Open(starg.okfilename, "w") == false)
  {
    logfile.Write("_tcpputfiles()/WriteToOKFileName()/File.Open(%s) failed!\n", starg.okfilename);
    return false;
  }

  for (int acc = 0; acc < vokfilename1.size(); acc++)
  {
    File.Fprintf("<filename>%s</filename><mtime>%s<mtime>\n", vokfilename1[acc].filename, vokfilename1[acc].mtime);
  }

  return true;
}

bool ActiveTest()
{
  memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
  memset(strSendBuffer, 0, sizeof(strSendBuffer));

  strcpy(strSendBuffer, "<activetest>ok</activetest>");

  if (TcpClient.Write(strSendBuffer) == false)
  {
    logfile.Write("ActiveTest()/TcpClient.Write() failed!\n");
    TcpClient.Close();
    return false;
  }

  if (TcpClient.Read(strRecvBuffer, 30) == false)
  {
    logfile.Write("ActiveTest()/TcpClient.Read() failed!\n");
    TcpClient.Close();
    return false;
  }

  if (strcmp(strRecvBuffer, "ok") != 0)
  {
    TcpClient.Close();
    return false;
  }

  return true;
}

bool AppendToOKFileName(struct st_fileinfo *stfileinfo)
{
  CFile File;

  if (File.Open(starg.okfilename, "a") == false)
  {
    logfile.Write("File.Open(%s) failed!\n", starg.okfilename);
    return false;
  }

  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo->filename, stfileinfo->mtime);

  return true;
}
