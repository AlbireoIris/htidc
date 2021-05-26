#include "_freecplus.h"
#include "_ftp.h"

using namespace std;

//类的声明
CLogFile logfile;
Cftp ftp;

//容器的声明
vector<struct st_stfileinfo> vlistfile,vlistfile1;
vector<struct st_stfileinfo> vokfilename,vokfilename1;

//函数的声明
bool LoadListFile();     //文件list加载到容器内
bool _ftpgetfile();      //程序主流程
bool LoadOKFileName();   //把okfilename的文件加载到容器中
bool CompVector();       //比较两个容器，确定增量采集对象
bool WriteToOKFileName();  //将vokfilename容器内容写入okfilename的文件中
bool AppendToOKFileName(struct st_stfileinfo *stfileinfo);  //把采集成功的文件记录追加到okfilename文件中


struct st_arg
{
   char host[64];
   char mode[32];
   char username[32];
   char password[32];
   char localpath[256];
   char remotepath[256];
   char matchname[256];
   char remotepathbak[256];
   char listfilename[256];
   char okfilename[256];
   int ptype;
   int timetvl;
}starg;

struct st_stfileinfo
{
  char filename[256];
  char mtime[32];
};

void EXIT(int sig);
void _help(char* argv[]);

int main(int argc,char* argv[])
{
  if(argc != 3)
  { 
    _help(argv);
    return -1;
  }

  CloseIOAndSignal();  // 屏蔽所有输入输出
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);

  if(logfile.Open(argv[1],"a+") == false)
  {
    printf("open the logfile: %s failed\n",argv[3]);
    return -1;
  }

  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(argv[2],"host",starg.host);
  GetXMLBuffer(argv[2],"mode",starg.mode);
  GetXMLBuffer(argv[2],"username",starg.username);
  GetXMLBuffer(argv[2],"password",starg.password);
  GetXMLBuffer(argv[2],"localpath",starg.localpath);
  GetXMLBuffer(argv[2],"remotepath",starg.remotepath);
  GetXMLBuffer(argv[2],"matchname",starg.matchname);
  GetXMLBuffer(argv[2],"ptype",&starg.ptype);
  GetXMLBuffer(argv[2],"remotepathbak",starg.remotepathbak);
  GetXMLBuffer(argv[2],"listfilename",starg.listfilename);
  GetXMLBuffer(argv[2],"okfilename",starg.okfilename);
  GetXMLBuffer(argv[2],"timetvl",&starg.timetvl);

  int mode=FTPLIB_PASSIVE;
  if(strcmp(starg.mode,"port")==0)
    mode=FTPLIB_PASSIVE;
 while(true)
 {
  if(ftp.login(starg.host,starg.username,starg.password,mode) == false)
  {
    logfile.Write("ftp.login(%s,%s,%s) failed!\n",starg.host,starg.username,starg.password);
    continue;
  } 
  logfile.Write("ftp.login success!\n");
  
  _ftpgetfile();

  ftp.logout();
  
  sleep(10);
 } 
  return 0;
}


void EXIT(int sig)
{
  logfile.Write("程序退出 sig=%d\n",sig);
  exit(0);
}


bool _ftpgetfile()
{
  //改变ftp的目录
  if(ftp.chdir(starg.remotepath) == false)
  {
    logfile.Write("ftp.chdir %s failed!\n",starg.remotepath);
    return false; 
  }
  logfile.Write("ftp.chdir %s success!\n",starg.remotepath);
 
  
  //列出ftp的目录
  if(ftp.nlist(".",starg.listfilename) == false)
  {
    logfile.Write("ftp.nlist %s failed!\n",starg.listfilename);
    return false;
  }
  logfile.Write("ftp.nlist %s success!\n",starg.listfilename);



  //将list文件加入vlist容器中
  if(LoadListFile() == false)
  {
    logfile.Write("LoadListFile %s failed!\n",starg.listfilename);
    return false;
  }
   logfile.Write("LoadListFile %s success!\n",starg.listfilename);



  //切换本地工作目录
  char strremotefilename[256];
  char strlocalfilename[256];


  
  if(starg.ptype == 1)
  {
    //把okfilename文件里的内容加到vokfilename中
    if(LoadOKFileName() == false)
    {
      logfile.Write("LoadOKFileName() failed!\n");
      return false;
    }
      logfile.Write("LoadOKFileName() success!\n");

      
    //把把vlistfile容器里的文件和vokfilename进行对比，得到两个容器
    //1.在vlistfile中存在，并且采集成功的文件vokfilename1
    //2.在vlistfile中存在，新文件需要重新采集的文件vlistfile1
    if(CompVector() == false)
    { 
      logfile.Write("CompVector() failed!\n");
      return false;
    }
      logfile.Write("CompVector() success!\n");
    //把vokfilename容器中的内容先写到okfilename中，覆盖okfilename
    if(WriteToOKFileName() == false)
    {
      logfile.Write("WriteToOKFileName() failed!\n");
      return false;
    }
      logfile.Write("WriteToOKFileName() success!\n");
    //把vlistfile1容器里的内容复制到vlistfile容器中
    vlistfile.clear();
    vlistfile.swap(vlistfile1);
  }


  int tempcount=0;
  for(int acc=0;acc<vlistfile.size();acc++)
  {
    //获取文件
    SNPRINTF(strlocalfilename,255,255,"%s/%s",starg.localpath,vlistfile[acc].filename);
    SNPRINTF(strremotefilename,255,255,"%s/%s",starg.remotepath,vlistfile[acc].filename);

    printf("get %s ...\n",vlistfile[acc].filename);

    if(ftp.get(strremotefilename,strlocalfilename,true) == false)
    {
      logfile.Write("ftp.get %s failed!\n",vlistfile[acc].filename);
      break;
    }
      logfile.Write("ftp.get %s success!\n",vlistfile[acc].filename);
    //删除文件
    if(starg.ptype == 2)
      ftp.ftpdelete(strremotefilename);

    //转存到备份目录
    if(starg.ptype == 3)
    {
      char strremotefilenamebak[256];
      SNPRINTF(strremotefilenamebak,255,255,"%s/%s",starg.remotepathbak,vlistfile[acc].filename);
      ftp.ftprename(strremotefilename,strremotefilenamebak);
    }
    tempcount++;

    //追加到vlistfile容器内
    if(starg.ptype == 1)
      AppendToOKFileName(&vlistfile[acc]);
  } 
  logfile.Write("ftp.get %d file success!\n",tempcount); 
  return true;
}



bool LoadListFile()
{
  vlistfile.clear();
  
  CFile File;
  if(File.Open(starg.listfilename,"r") == false)
  {
    logfile.Write("File.Open %s failed!\n",starg.listfilename);
    return false;
  }
  logfile.Write("File.Open %s success!\n",starg.listfilename);
  
  struct st_stfileinfo stfileinfo;
  
  while(1)
  {
    memset(&stfileinfo,0,sizeof(struct st_stfileinfo));
    if(File.Fgets(stfileinfo.filename,255,true) == false)   
      break;                                              
     
   if(MatchFileName(stfileinfo.filename,starg.matchname) == false)
      continue;

    if(starg.ptype == 1)
    {
      if(ftp.mtime(stfileinfo.filename) == false)
      {
        logfile.Write("ftp.mtime %s failed!\n",stfileinfo.filename);
        return false;
      }
      
      strcpy(stfileinfo.mtime,ftp.m_mtime);
    }
    vlistfile.push_back(stfileinfo);
    logfile.Write("vlistfile filename=%s,mtime=%s\n",stfileinfo.filename,stfileinfo.mtime);
  }
  return true;
}  


//把okfilename文件内容加载到vokfilename容器中
bool LoadOKFileName()
{
  vokfilename.clear();
  CFile File;
  if(File.Open(starg.okfilename,"r") == false)
    return true; //初次打开文件可能不存在

  struct st_stfileinfo stfileinfo;
  char strbuffer[256];

  logfile.Write("Loading %s ...\n",starg.okfilename);

  while(1)
  {
    memset(&stfileinfo,0,sizeof(struct st_stfileinfo));
    if(File.Fgets(strbuffer,255,true) == false)  break; 

    GetXMLBuffer(strbuffer,"filename",stfileinfo.filename,255);
    GetXMLBuffer(strbuffer,"mtime",stfileinfo.mtime,31);                                    
    vokfilename.push_back(stfileinfo);
  }
  logfile.Write("Loading %s success! tatalcount=%d\n",starg.okfilename,vokfilename.size());
  return true;
}




bool CompVector()
{
  vokfilename1.clear();
  vlistfile1.clear();

  for(int acc=0;acc<vlistfile.size();acc++)
  {
    int jj = 0;
    for(jj=0;jj<vokfilename.size();jj++)
    {
      if( strcmp(vlistfile[acc].filename,vokfilename[jj].filename) == 0 && strcmp(vlistfile[acc].mtime,vokfilename[jj].mtime) == 0)
      {
        vokfilename1.push_back(vlistfile[acc]);
        break;
      }
    }
    
    if(jj == vokfilename.size())
      vlistfile1.push_back(vlistfile[acc]);

   }
  
   for(int acc=0;acc<vokfilename1.size();acc++)
   {
     logfile.Write("vokfilename filename=%smtime=%s\n",vokfilename1[acc].filename,vokfilename1[acc].mtime);
   }


   for(int acc=0;acc<vlistfile1.size();acc++)
   {
     logfile.Write("vlistfile1 filename=%s mtime=%s\n",vlistfile1[acc].filename,vlistfile1[acc].mtime);
   }
  return true;
}



bool WriteToOKFileName()
{
  CFile File;
  
  if(File.Open(starg.okfilename,"w") == false)
  {
    logfile.Write("WriteToOKFileName(File.Open (%s)) failed!\n",starg.okfilename);
    return false;
  }
  logfile.Write("WriteToOKFileName(File.Open (%s)) success!\n",starg.okfilename); 


  for(int acc=0;acc<vokfilename1.size();acc++)
  {
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",vokfilename1[acc].filename,vokfilename1[acc].mtime);
  }

  return true;  
}




bool AppendToOKFileName(struct st_stfileinfo* stfileinfo)
{
  CFile File;

  if(File.Open(starg.okfilename,"a") == false)
  {
    logfile.Write("WriteToOKFileName(File.Open (%s)) failed!\n",starg.okfilename);
    return false;
  }
 
  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",stfileinfo->filename,stfileinfo->mtime);


  return true;
}



void _help(char* argv[])
{
  printf("\n");
  printf("这是一个ftp数据采集的程序，请按下列格式输入数据！\n");
  printf("介绍各个字段功能：\n");
  printf("host  远程服务器的IP和端口\n");
  printf("mode  传输模式有pasv和port 默认为pasv\n");
  printf("username  远程服务器登录用户名\n");
  printf("password  远程服务器的登录密码\n");
  printf("localpath  本地文件的存放目录\n");
  printf("remotepath  远程服务器的存放目标文件的目录\n");
  printf("matchname  待采集文件匹配的用户名，用大写匹配，不支持*匹配全部文件\n");
  printf("ptype  文件采集成功后远程服务器文件的处理方式 “1” 什不也做 ”2“后删除 ”3“采集完在远程服务器备份\n");
  printf("remotepathbak 当ptype=3时用来指定备份目录\n");
  printf("listfilename  列出采集前服务器文件名的文件\n");
  printf("timetvl  程序执行间隔\n");
  printf("okfilename  列出成功采集的文件的清单文件\n\n");
  printf("Example:/htidc/public/bin/ftpgetfile /log/shqx/ftpgetfile_surfdata.log \"<host>119.29.18.109:21</host><mode>pasv</mode><username>oracle</username><password>zhululin123</password><localpath>/data/shqx/sdata/surfdata</localpath><remotepath>/data/shqx/ftp/surfdata</remotepath><matchname>SURF_*.txt</matchname><ptype>1</ptype><remotepathbak>/data/shqx/ftp/surfdatabak</remotepathbak><listfilename>/data/shqx/ftplist/ftpgetfile_surfdata.list</listfilename><okfilename>/data/shqx/ftplist/ftpgetfile_surfdata.xml</okfilename><timetvl>30</timetvl>\"\n\n");
}
