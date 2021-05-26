#include "_freecplus.h"
#define TCPBUFLEN 8182

struct st_arg
{
  char ip[32];
  int port;
  int ptype;
  char clientpath[256];
  char srvpath[256];
  char srvpathbak[256];
  bool andchild;
  char matchname[256];
  char okfilename[256];
  int timetvl;
} starg;

char strRecvBuffer[TCPBUFLEN + 10];
char strSendBuffer[TCPBUFLEN + 10];

CTcpClient TcpClient;
CLogFile logfile;

vector<struct st_fileinfo> vlistfile, vlistfile1;
vector<struct st_fileinfo> vokfilename, vokfilename1;

void _help(char *argv[]);

bool _xmltoarg(char *strxmlbuffer);

bool ClientLogin(const char *argv);

bool _tcpgetfiles();

bool LoadListFile();

bool LoadOKFileName();

bool CompVector();

bool WriteToOKFileName();

bool AppendToOKFileName(struct st_fileinfo *stfileinfo);

bool ActiveTest();

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    _help(argv);
    return -1;
  }

  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("打开日志 %s 文件失败\n", argv[1]);
    return -1;
  }

  if (_xmltoarg(argv[2]) == false)
  {
    logfile.Write("main()/_xmltoarg(argv[2] failed!\n");
    return -1;
  }

  while (true)
  {
    if (ClientLogin(argv[2]) == false)
      logfile.Write("main()/ClientLogin() failed!");
    //logfile.Write("main()/ClientLogin() Success!\n");

    _tcpgetfiles();

    if (vlistfile.size() == 0)
    {
      if (ActiveTest() == false)
        logfile.Write("ActiveTest() Failed!\n");
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
  printf("Using:/htidc/public/bin/tcpgetfiles_FR logfilename xmlbuffer\n\n");

  printf("Sample:/htidc/public/bin/tcpgetfiles_FR /log/shqx/tcpgetfiles_surfdata.log \"<ip>119.29.18.109</ip><port>5051</port><ptype>1</ptype><clientpath>/data/shqx/tcpclient/tcpget</clientpath><srvpath>/data/shqx/tcpserver/surfdata</srvpath><srvpathbak>/data/shqx/tcpserver/surfdatabak</srvpathbak><andchild>true</andchild><matchname>SURF_*.TXT,*.DAT</matchname><okfilename>/data/shqx/tcpclient/tcplist/tcpgetfiles_surfdata.xml</okfilename><timetvl>10</timetvl>\"\n\n\n");

  printf("这是一个通用的功能模块，采用TCP协议获取文件的客户端。\n");
  printf("logfilename   本程序运行的日志文件。\n");
  printf("xmlbuffer     本程序运行的参数，如下：\n");
  printf("ip            服务器端的IP地址。\n");
  printf("port          服务器端的端口。\n");
  printf("clientpath    客户端文件存放的根目录。\n");
  printf("srvpath       服务端文件存放的根目录。\n");
  printf("ptype         文件获取成功后服务端文件的处理方式：1-保留文件；2-删除文件；3-移动到备份目录。\n");
  printf("srvpathbak    文件成功获取后，服务端文件备份的根目录，当ptype==3时有效，缺省为空。\n");
  printf("andchild      是否获取srvpath目录下各级子目录的文件，true-是；false-否，缺省为false。\n");
  printf("matchname     待获取文件名的匹配方式，如\"*.TXT,*.XML\"，注意用大写。\n");
  printf("okfilename    已获取成功文件名清单，缺省为空。\n");
  printf("timetvl       扫描本地目录文件的时间间隔，单位：秒，取值在1-50之间。\n\n\n");
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

  GetXMLBuffer(strxmlbuffer, "srvpathbak", starg.srvpathbak);
  if ((starg.ptype == 3) && (strlen(starg.srvpathbak) == 0))
  {
    logfile.Write("srvpathbak is null.\n");
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
  if (TcpClient.m_sockfd > 0)
    return true;

  int ii = 0;

  while (true)
  {
    if (ii++ > 0)
      sleep(20);

    if (TcpClient.ConnectToServer(starg.ip, starg.port) == false)
    {
      logfile.Write("ClientLogin()/TcpClient.ConnectToServer() failed!\n");
      continue;
    }
    //logfile.Write("ClientLogin()/TcpClient.ConnectToServer() Success!\n");

    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
    memset(strSendBuffer, 0, sizeof(strSendBuffer));

    strcpy(strSendBuffer, argv);
    strcat(strSendBuffer, "<clienttype>2</clienttype>");

    if (TcpClient.Write(strSendBuffer) == false)
    {
      logfile.Write("ClientLogin()/TcpClient.Write() failed!\n");
      continue;
    }
    //logfile.Write("ClientLogin()/TcpClient.Write() Success!\n");

    if (TcpClient.Read(strRecvBuffer, 30) == false)
    {
      logfile.Write("ClientLogin()/TcpClient.Read() failed!\n");
      continue;
    }
    //logfile.Write("ClientLogin()/TcpClient.Read(%s) Success!\n",strRecvBuffer);

    break;
  }

  logfile.Write("Login(%s,%d) success!\n", starg.ip, starg.port);

  return true;
}

bool _tcpgetfiles()
{
  if (LoadListFile() == false)
  {
    logfile.Write("_tcpgetfiles()/LoadListFile() failed!\n");
    TcpClient.Close();
    return false;
  }
  //logfile.Write("_tcpgetfiles()/LoadListFile() Success!\n");

  //logfile.Write("_tcpgetfiles()/starg.ptype=%d Success!\n",starg.ptype);
  if (starg.ptype == 1)
  {
    if (LoadOKFileName() == false)
      logfile.Write("_tcpgetfiles()/LoadOKFileName() failed!\n");
    //logfile.Write("_tcpgetfiles()/LoadOKFileName() Success!\n");

    if (CompVector() == false)
      logfile.Write("_tcpgetfiles()/CompVector() failed!\n");
    //logfile.Write("_tcpgetfiles()/CompVector() Success!\n");

    if (WriteToOKFileName() == false)
      logfile.Write("_tcpgetfiles()/WriteToOKFileName() failed!\n");
    //logfile.Write("_tcpgetfiles()/WriteToOKFileName() Success!\n");

    vlistfile.clear();
    vlistfile.swap(vlistfile1);
  }

  //logfile.Write("_tcpgetfiles()/vlistfile.size()=%d\n",vlistfile.size());

  for (int ii = 0; ii < vlistfile.size(); ii++)
  {
    memset(strSendBuffer, 0, sizeof(strSendBuffer));

    sprintf(strSendBuffer, "<filename>%s</filename><filesize>%d</filesize><mtime>%s</mtime>", vlistfile[ii].filename, vlistfile[ii].filesize, vlistfile[ii].mtime);

    if (TcpClient.Write(strSendBuffer) == false)
    {
      logfile.Write("_tcpgetfiles()/TcpClient.Write() failed!\n");
      TcpClient.Close();
      return false;
    }
    //logfile.Write("_tcpgetfiles()/TcpClient.Write() Success!\n");

    if (TcpClient.Read(strRecvBuffer, 30) == false)
    {
      logfile.Write("_tcpgetfiles()/TcpClient.Read() failed!\n");
      TcpClient.Close();
      return false;
    }

    struct st_fileinfo stfileinfo;
    memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
    strcpy(stfileinfo.filename, vlistfile[ii].filename);
    strcpy(stfileinfo.mtime, vlistfile[ii].mtime);
    stfileinfo.filesize = vlistfile[ii].filesize;

    UpdateStr(stfileinfo.filename, starg.srvpath, starg.clientpath);

    logfile.Write("Getting %s ...", stfileinfo.filename);

    if (RecvFile(TcpClient.m_sockfd, &stfileinfo, &logfile) == false)
    {
      logfile.Write("Failed!\n");
      logfile.Write("_tcpgetfiles()/RecvFile() failed!\n");
      TcpClient.Close();
      return false;
    }

    logfile.Write("Success!\n");

    if (starg.ptype == 1)
      AppendToOKFileName(&vlistfile[ii]);
  }
  return true;
}

bool LoadListFile()
{
  vlistfile.clear();

  memset(strSendBuffer, 0, sizeof(strSendBuffer));
  strcpy(strSendBuffer, "<list>");

  if (TcpClient.Write(strSendBuffer) == false)
  {
    logfile.Write("_tcpgetfiles()/LoadListFile()/TcpClient.Write() failed!\n");
    return false;
  }
  //logfile.Write("_tcpgetfiles()/LoadListFile()/TcpClient.Write() Success!\n");

  memset(strRecvBuffer, 0, sizeof(strRecvBuffer));

  if (TcpClient.Read(strRecvBuffer, 30) == false)
  {
    logfile.Write("_tcpgetfiles()/LoadListFile()/TcpClient.Read() failed! %d\n");
    return false;
  }
  //logfile.Write("_tcpgetfiles()/LoadListFile()/TcpClient.Read(%s) Success!\n",strRecvBuffer);

  int totalfile = 0;

  GetXMLBuffer(strRecvBuffer, "totalfile", &totalfile);

  struct st_fileinfo stfileinfo;

  //logfile.Write("totalfile = %d\n",totalfile);

  for (int ii = 0; ii < totalfile; ii++)
  {
    memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
    memset(strRecvBuffer, 0, sizeof(strRecvBuffer));

    if (TcpClient.Read(strRecvBuffer) == false)
    {
      logfile.Write("Num %d _tcpgetfiles()/LoadListFile()/TcpClient.Read() failed!\n", ii + 1);
      return false;
    }
    //logfile.Write("Num %d _tcpgetfiles()/LoadListFile()/TcpClient.Read Success!\n",ii+1);

    GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
    GetXMLBuffer(strRecvBuffer, "filesize", &stfileinfo.filesize);
    GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

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

    GetXMLBuffer(strbuffer, "filename", stfileinfo.filename, 255);
    GetXMLBuffer(strbuffer, "mtime", stfileinfo.mtime, 20);

    vokfilename.push_back(stfileinfo);
  }

  return true;
}

bool CompVector()
{
  vokfilename1.clear();
  vlistfile1.clear();

  for (int ii = 0; ii < vlistfile.size(); ii++)
  {
    int jj = 0;
    for (jj = 0; jj < vokfilename.size(); jj++)
    {
      if ((strcmp(vlistfile[ii].filename, vokfilename[jj].filename) == 0) && (strcmp(vlistfile[ii].mtime, vokfilename[jj].mtime) == 0))
      {
        vokfilename1.push_back(vlistfile[ii]);
        break;
      }
    }

    if (jj == vokfilename.size())
      vlistfile1.push_back(vlistfile[ii]);
  }
  return true;
}

bool WriteToOKFileName()
{
  CFile File;

  if (File.Open(starg.okfilename, "w", false) == false)
  {
    logfile.Write("_tcpgetfiles()/WriteToOKFileName()/File.Open() failed!\n");
    return false;
  }

  for (int ii = 0; ii < vokfilename1.size(); ii++)
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vokfilename1[ii].filename, vokfilename1[ii].mtime);

  return true;
}

bool AppendToOKFileName(struct st_fileinfo *stfileinfo)
{
  CFile File;

  if (File.Open(starg.okfilename, "a", false) == false)
  {
    logfile.Write("_tcpgetfiles()/AppendToOKFileName/File.Open() failed!\n");
    return false;
  }

  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo->filename, stfileinfo->mtime);

  return true;
}

bool ActiveTest()
{
  memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
  memset(strSendBuffer, 0, sizeof(strRecvBuffer));

  strcpy(strSendBuffer, "<activetest>ok</activetest>");

  if (TcpClient.Write(strSendBuffer) == false)
  {
    logfile.Write("main()/ActiveTest()/TcpClient.Write() failed!\n");
    TcpClient.Close();
    return false;
  }

  if (TcpClient.Read(strRecvBuffer, 20) == false)
  {
    logfile.Write("main()/ActiveTest()/TcpClient.Read() failed!\n");
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
