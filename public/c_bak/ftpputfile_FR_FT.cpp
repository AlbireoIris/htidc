#include "_freecplus.h"
#include "_ftp.h"

//类的声明
CLogFile logfile;
Cftp ftp;

//容器的声明
vector<struct st_stfileinfo> vlistfile;    //这个容器中存放了远程目标目录里文件的文件名和修改时间
vector<struct st_stfileinfo> vlistfile1;   //这个容器存放了需要再次推送的文件名
vector<struct st_stfileinfo> vokfilename;  //这个容器存放了上一次完成推送的名单
vector<struct st_stfileinfo> vokfilename1; //这个容器存放了这一次不需要重新推送的名单

//函数的声明
bool _ftpgetfile(); //程序主流程
bool getargs(char *argv);
bool LoadListFile();                                       //文件list加载到容器内
bool LoadOKFileName();                                     //把okfilename的文件加载到容器中
bool CompVector();                                         //比较两个容器，确定增量采集对象
bool Write_PToOKFileName();                                //将vokfilename容器内容写入okfilename的文件中
bool AppendToOKFileName(struct st_stfileinfo *stfileinfo); //把推送成功的文件记录追加到okfilename文件中

void EXIT(int sig);
void _help(char *argv[]);

//全局变量的声明
struct st_arg
{
  char host[64];
  char mode[32];
  char username[32];
  char password[32];
  char localpath[256];
  char remotepath[256];
  char matchname[256];
  char localpathbak[256];
  char listfilename[256];
  char okfilename[256];
  int ptype;
  int timetvl;
} starg;

struct st_stfileinfo
{
  char filename[256];
  char mtime[32];
};

int mode = FTPLIB_PASSIVE; //这参数决定了传输的模式

//main函数的开始
int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    _help(argv);
    return -1;
  }

  //CloseIOAndSignal();  // 屏蔽所有输入输出
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("open the logfile: %s failed\n", argv[3]);
    return -1;
  }

  getargs(argv[2]);

  while (true)
  {
    if (ftp.login(starg.host, starg.username, starg.password, mode) == false)
    {
      logfile.Write_P("ftp.login(%s,%s,%s) failed!\n", starg.host, starg.username, starg.password);
      continue;
    }
    logfile.Write_P("ftp.login success!\n");

    _ftpgetfile();

    ftp.logout();

    break;
    //    sleep(10);
  }
  return 0;
}

void EXIT(int sig)
{
  logfile.Write_P("程序退出 sig=%d\n", sig);
  exit(0);
}

bool _ftpgetfile()
{
  //  //改变ftp的目录到远程推送服务器目录，目的是为了获取文件名和改变时间
  //  if(ftp.chdir(starg.remotepath) == false)
  //  {
  //    logfile.Write_P("ftp.chdir %s failed!\n",starg.remotepath);
  //    return false;
  //  }
  //  logfile.Write_P("ftp.chdir %s success!\n",starg.remotepath);
  //
  //
  //  //列出ftp的目录,到starg.listfilename这个文件中
  //  if(ftp.nlist(".",starg.listfilename) == false)
  //  {
  //    logfile.Write_P("ftp.nlist %s failed!\n",starg.listfilename);
  //    return false;
  //  }
  //  logfile.Write_P("ftp.nlist %s success!\n",starg.listfilename);

  //将目标文件所在文件夹的信息加入vlistfile容器中
  if (LoadListFile() == false)
  {
    logfile.Write_P("LoadListFile %s failed!\n", starg.listfilename);
    return false;
  }
  logfile.Write_P("LoadListFile %s success!\n", starg.listfilename);

  //把上一次推送完毕的文件的文件名和修改信息添加到okfilename容器中
  if (LoadOKFileName() == false)
  {
    logfile.Write_P("LoadOKFileName() failed!\n");
    return false;
  }
  logfile.Write_P("LoadOKFileName() success!\n");

  //切换本地工作目录
  char strremotefilename[256];
  char strlocalfilename[256];

  if (starg.ptype == 1)
  {
    //把上一次推送完毕的文件的文件名和修改信息添加到okfilename容器中
    if (LoadOKFileName() == false)
    {
      logfile.Write_P("LoadOKFileName() failed!\n");
      return false;
    }
    logfile.Write_P("LoadOKFileName() success!\n");

    //把vlistfile容器里的文件和vokfilename进行对比，得到两个容器
    //1.在vlistfile中存在，并且推送成功的文件vokfilename1
    //2.在vlistfile中存在，新文件需要重新推送的文件vlistfile1
    if (CompVector() == false)
    {
      logfile.Write_P("CompVector() failed!\n");
      return false;
    }
    logfile.Write_P("CompVector() success!\n");

    //把vokfilename1容器中的内容先写到okfilename中，覆盖okfilename
    if (Write_PToOKFileName() == false)
    {
      logfile.Write_P("Write_PToOKFileName() failed!\n");
      return false;
    }
    logfile.Write_P("Write_PToOKFileName() success!\n");

    //把vlistfile1容器里的内容复制到vlistfile容器中
    //现在vlistfile容器中存放的就是需要推送的文件名了
    vlistfile.clear();
    vlistfile.swap(vlistfile1);
  }

  //开始推送
  int tempcount = 0;
  for (int acc = 0; acc < vlistfile.size(); acc++)
  {
    //获取文件
    SNPRINTF(strlocalfilename, 255, 255, "%s/%s", starg.localpath, vlistfile[acc].filename);
    SNPRINTF(strremotefilename, 255, 255, "%s/%s", starg.remotepath, vlistfile[acc].filename);

    printf("putting %s ...\n", vlistfile[acc].filename);

    if (ftp.put(strlocalfilename, strremotefilename, true) == false)
    {
      logfile.Write_P("ftp.put %s failed!\n", vlistfile[acc].filename);
      break;
    }
    logfile.Write_P("ftp.put %s success!\n", vlistfile[acc].filename);

    //删除文件
    if (starg.ptype == 2)
      REMOVE(strlocalfilename, 2);

    //转存到备份目录
    if (starg.ptype == 3)
    {
      char strlocalfilenamebak[256];
      SNPRINTF(strlocalfilenamebak, 255, 255, "%s/%s", starg.localpathbak, vlistfile[acc].filename);
      COPY(strlocalfilename, strlocalfilenamebak);
      REMOVE(strlocalfilename, 2);
    }
    tempcount++;

    //追加到vlistfile容器内
    if (starg.ptype == 1)
      AppendToOKFileName(&vlistfile[acc]);
  }
  logfile.Write_P("ftp.put %d file success!\n", tempcount);
  return true;
}

//这个函数的目的是把参数传入全局的结构体变量中
bool getargs(char *argv)
{
  memset(&starg, 0, sizeof(struct st_arg));
  GetXMLBuffer(argv, "host", starg.host);
  GetXMLBuffer(argv, "mode", starg.mode);
  GetXMLBuffer(argv, "username", starg.username);
  GetXMLBuffer(argv, "password", starg.password);
  GetXMLBuffer(argv, "localpath", starg.localpath);
  GetXMLBuffer(argv, "remotepath", starg.remotepath);
  GetXMLBuffer(argv, "matchname", starg.matchname);
  GetXMLBuffer(argv, "ptype", &starg.ptype);
  GetXMLBuffer(argv, "localpathbak", starg.localpathbak);
  GetXMLBuffer(argv, "listfilename", starg.listfilename);
  GetXMLBuffer(argv, "okfilename", starg.okfilename);
  GetXMLBuffer(argv, "timetvl", &starg.timetvl);

  if (strcmp(starg.mode, "port") == 0)
    mode = FTPLIB_PORT;

  return true;
}

//加载远程服务器信息到容器vlistfile中
bool LoadListFile()
{
  vlistfile.clear();

  CDir Dir;

  Dir.OpenDir(starg.localpath, starg.matchname, 5000, false, false);

  struct st_stfileinfo stfileinfo;

  while (1)
  {
    memset(&stfileinfo, 0, sizeof(struct st_stfileinfo));

    if (Dir.ReadDir() == false)
      break;

    strcpy(stfileinfo.filename, Dir.m_FileName);
    strcpy(stfileinfo.mtime, Dir.m_ModifyTime);

    vlistfile.push_back(stfileinfo);
    logfile.Write_P("vlistfile filename=%s,mtime=%s\n", stfileinfo.filename, stfileinfo.mtime);
  }
  return true;
}

//这个函数的目的是将上一次推送完成的文件名和修改时间加载到vokfilename容器中
bool LoadOKFileName()
{
  vokfilename.clear();
  CFile File;

  //初次打开文件可能不存在
  if (File.Open(starg.okfilename, "r") == false)
    return true;

  struct st_stfileinfo stfileinfo;
  char strbuffer[256];

  logfile.Write_P("Loading %s ...\n", starg.okfilename);

  while (1)
  {
    memset(&stfileinfo, 0, sizeof(struct st_stfileinfo));
    if (File.Fgets(strbuffer, 255, true) == false)
      break;

    GetXMLBuffer(strbuffer, "filename", stfileinfo.filename, 255);
    GetXMLBuffer(strbuffer, "mtime", stfileinfo.mtime, 31);
    vokfilename.push_back(stfileinfo);
  }
  logfile.Write_P("Loading %s success! tatalcount=%d\n", starg.okfilename, vokfilename.size());
  return true;
}

bool CompVector()
{
  vokfilename1.clear();
  vlistfile1.clear();

  for (int acc = 0; acc < vlistfile.size(); acc++)
  {
    //将文件名和修改时间进行比较，如果vlistfile修改时间和文件名与vokfilename中保持一致则表示无需推送，放入vokfilename1容器中
    int jj = 0;
    for (jj = 0; jj < vokfilename.size(); jj++)
    {
      if (strcmp(vlistfile[acc].filename, vokfilename[jj].filename) == 0 && strcmp(vlistfile[acc].mtime, vokfilename[jj].mtime) == 0)
      {
        vokfilename1.push_back(vlistfile[acc]);
        break;
      }
    }

    //如果找到最后一个也没发现有一致的则表示需要重新推送，放入vlistfile1中
    if (jj == vokfilename.size())
      vlistfile1.push_back(vlistfile[acc]);
  }

  for (int acc = 0; acc < vokfilename1.size(); acc++)
    logfile.Write_P("vokfilename filename=%smtime=%s\n", vokfilename1[acc].filename, vokfilename1[acc].mtime);

  for (int acc = 0; acc < vlistfile1.size(); acc++)
    logfile.Write_P("vlistfile1 filename=%s mtime=%s\n", vlistfile1[acc].filename, vlistfile1[acc].mtime);

  return true;
}

bool Write_PToOKFileName()
{
  CFile File;

  //打开文件复写
  if (File.Open(starg.okfilename, "w") == false)
  {
    logfile.Write_P("Write_PToOKFileName(File.Open (%s)) failed!\n", starg.okfilename);
    return false;
  }
  logfile.Write_P("Write_PToOKFileName(File.Open (%s)) success!\n", starg.okfilename);

  //用xml格式将完成推送的文件名写入okfilename文件中
  for (int acc = 0; acc < vokfilename1.size(); acc++)
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vokfilename1[acc].filename, vokfilename1[acc].mtime);

  return true;
}

//这个函数的目的是将成功完成推送的文件名写入okfilename.xml文件中
bool AppendToOKFileName(struct st_stfileinfo *stfileinfo)
{
  CFile File;

  if (File.Open(starg.okfilename, "a") == false)
  {
    logfile.Write_P("Write_PToOKFileName(File.Open (%s)) failed!\n", starg.okfilename);
    return false;
  }

  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo->filename, stfileinfo->mtime);

  return true;
}

void _help(char *argv[])
{
  printf("\n");
  printf("这是一个ftp数据推送的程序，请按下列格式输入数据！\n");
  printf("介绍各个字段功能：\n");
  printf("host          远程服务器的IP和端口\n");
  printf("mode          传输模式有pasv和port 默认为pasv\n");
  printf("username      远程服务器登录用户名\n");
  printf("password      远程服务器的登录密码\n");
  printf("localpath     本地目标文件的存放目录\n");
  printf("remotepath    远程服务器的目标目录\n");
  printf("matchname     推送文件匹配的用户名，用大写匹配，不支持*匹配全部文件\n");
  printf("ptype         文件采集成功后远程服务器文件的处理方式 1-什么也不做 2-推送后删除 3-推送后在本地服务器备份\n");
  printf("localpathbak  当ptype=3时用来指定本地备份目录\n");
  printf("listfilename  列出推送前文件名的文件\n");
  printf("timetvl       程序执行间隔\n");
  printf("okfilename    列出成功推送的文件的清单文件\n\n");
  printf("Example:/htidc/public/bin/ftpputfile_FR_FT /log/shqx/ftpputfile_FR_FT_surfdata.log \"<host>119.29.18.109:21</host><mode>pasv</mode><username>oracle</username><password>zhululin123</password><localpath>/data/shqx/exp/tohb</localpath><remotepath>/data/shqx/ftpput</remotepath><matchname>SURFDATA*</matchname><ptype>3</ptype><localpathbak>/data/shqx/exp/tohbbak</localpathbak><listfilename>/data/shqx/ftplist/ftpputfile_surfdata.list</listfilename><okfilename>/data/shqx/ftplist/ftpputfile_surfdata.xml</okfilename><timetvl>30</timetvl>\"\n\n");
}
