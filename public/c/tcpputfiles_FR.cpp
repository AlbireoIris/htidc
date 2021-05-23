#include "_freecplus.h"
#define TCPBUFLEN 8182
struct st_arg
{
  char ip[32];             //服务器的ip
  int port;                //服务器的端口
  int ptype;               //文件发送后文件的处理方式 1-保留 2-删除 3-移动到备份目录
  char clientpath[256];    //本地文件存放的根目录
  char clientpathbak[256]; //本地文件备份的根目录，仅在选项为3时有效
  char srvpath[256];       //服务端文件存放的根目录
  bool andchild;           //是否发送clientpath目录下个各级子目录的文件，true-是 false-否
  char matchname[256];     //待发送文件名的匹配方式 如"*.txt"
  char okfilename[256];    //已成功发送文件名的清单
  int timetvl;             //扫描本地目录文件的时间间隔 单位秒
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

//这个函数是用来登录服务器的，如果未连接到服务器则将一直在此函数内循环
bool ClientLogin(const char *argv);

//这个函数是程序主流程
bool _tcpputfiles();

//这个函数是将clientpath目录下的文件加载到vlistfile容器中
bool LoadListFile();

//把okfilename文件内容加载到vokfilename容器中
bool LoadOKFileName();

//通过两个容器比较，决定采集的文件
bool CompVector();

//把okfilename写入容器
bool WriteToOKFileName();

//追加文件到okfilename中
bool AppendToOKFileName(struct st_fileinfo *stfileinfo);

//心跳报文函数
bool ActiveTest();

int main(int argc, char *argv[])
{
  //提示信息
  if (argc != 3)
  {
    _help(argv);
    return -1;
  }

  //关闭所有信号和输出
  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  //先打开日志文件在关闭IO
  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("打开日志文件 %s 失败！\n", argv[1]);
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

  printf("本程序是数据中心的公共功能模块，采用TCP协议把文件发送给服务端。\n");
  printf("logfilename   本程序运行的日志文件。\n");
  printf("xmlbuffer     本程序运行的参数，如下：\n");
  printf("ip            服务器端的IP地址。\n");
  printf("port          服务器端的端口。\n");
  printf("ptype         文件发送成功后的处理方式：1-保留文件；2-删除文件；3-移动到备份目录。\n");
  printf("clientpath    本地文件存放的根目录。\n");
  printf("clientpathbak 文件成功发送后，本地文件备份的根目录，当ptype==3时有效，缺省为空。\n");
  printf("srvpath       服务端文件存放的根目录。\n");
  printf("andchild      是否发送clientpath目录下各级子目录的文件，true-是；false-否，缺省为false。\n");
  printf("matchname     待发送文件名的匹配方式，如\"*.TXT,*.XML\"，注意用大写。\n");
  printf("okfilename    已发送成功文件名清单，缺省为空。\n");
  printf("timetvl       扫描本地目录文件的时间间隔，单位：秒，取值在1-50之间。\n\n\n");
}

void EXIT(int sig)
{
  logfile.Write_P("程序退出，sig=%d", sig);

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
  //由于是一个死循环，所以如果m_sockfd大于0则表示已有连接
  if (TcpClient.m_sockfd > 0)
    return true;

  int acc = 0;

  while (true)
  {
    //第一次进入循环时不休眠
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
    //表示进入Recv模式
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
