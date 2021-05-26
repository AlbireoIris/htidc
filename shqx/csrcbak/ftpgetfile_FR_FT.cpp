#include "_freecplus.h"
#include "_ftp.h"

using namespace std;

//�������
CLogFile logfile;
Cftp ftp;

//����������
vector<struct st_stfileinfo> vlistfile,vlistfile1;
vector<struct st_stfileinfo> vokfilename,vokfilename1;

//����������
bool LoadListFile();     //�ļ�list���ص�������
bool _ftpgetfile();      //����������
bool LoadOKFileName();   //��okfilename���ļ����ص�������
bool CompVector();       //�Ƚ�����������ȷ�������ɼ�����
bool WriteToOKFileName();  //��vokfilename��������д��okfilename���ļ���
bool AppendToOKFileName(struct st_stfileinfo *stfileinfo);  //�Ѳɼ��ɹ����ļ���¼׷�ӵ�okfilename�ļ���


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

  CloseIOAndSignal();  // ���������������
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
  logfile.Write("�����˳� sig=%d\n",sig);
  exit(0);
}


bool _ftpgetfile()
{
  //�ı�ftp��Ŀ¼
  if(ftp.chdir(starg.remotepath) == false)
  {
    logfile.Write("ftp.chdir %s failed!\n",starg.remotepath);
    return false; 
  }
  logfile.Write("ftp.chdir %s success!\n",starg.remotepath);
 
  
  //�г�ftp��Ŀ¼
  if(ftp.nlist(".",starg.listfilename) == false)
  {
    logfile.Write("ftp.nlist %s failed!\n",starg.listfilename);
    return false;
  }
  logfile.Write("ftp.nlist %s success!\n",starg.listfilename);



  //��list�ļ�����vlist������
  if(LoadListFile() == false)
  {
    logfile.Write("LoadListFile %s failed!\n",starg.listfilename);
    return false;
  }
   logfile.Write("LoadListFile %s success!\n",starg.listfilename);



  //�л����ع���Ŀ¼
  char strremotefilename[256];
  char strlocalfilename[256];


  
  if(starg.ptype == 1)
  {
    //��okfilename�ļ�������ݼӵ�vokfilename��
    if(LoadOKFileName() == false)
    {
      logfile.Write("LoadOKFileName() failed!\n");
      return false;
    }
      logfile.Write("LoadOKFileName() success!\n");

      
    //�Ѱ�vlistfile��������ļ���vokfilename���жԱȣ��õ���������
    //1.��vlistfile�д��ڣ����Ҳɼ��ɹ����ļ�vokfilename1
    //2.��vlistfile�д��ڣ����ļ���Ҫ���²ɼ����ļ�vlistfile1
    if(CompVector() == false)
    { 
      logfile.Write("CompVector() failed!\n");
      return false;
    }
      logfile.Write("CompVector() success!\n");
    //��vokfilename�����е�������д��okfilename�У�����okfilename
    if(WriteToOKFileName() == false)
    {
      logfile.Write("WriteToOKFileName() failed!\n");
      return false;
    }
      logfile.Write("WriteToOKFileName() success!\n");
    //��vlistfile1����������ݸ��Ƶ�vlistfile������
    vlistfile.clear();
    vlistfile.swap(vlistfile1);
  }


  int tempcount=0;
  for(int acc=0;acc<vlistfile.size();acc++)
  {
    //��ȡ�ļ�
    SNPRINTF(strlocalfilename,255,255,"%s/%s",starg.localpath,vlistfile[acc].filename);
    SNPRINTF(strremotefilename,255,255,"%s/%s",starg.remotepath,vlistfile[acc].filename);

    printf("get %s ...\n",vlistfile[acc].filename);

    if(ftp.get(strremotefilename,strlocalfilename,true) == false)
    {
      logfile.Write("ftp.get %s failed!\n",vlistfile[acc].filename);
      break;
    }
      logfile.Write("ftp.get %s success!\n",vlistfile[acc].filename);
    //ɾ���ļ�
    if(starg.ptype == 2)
      ftp.ftpdelete(strremotefilename);

    //ת�浽����Ŀ¼
    if(starg.ptype == 3)
    {
      char strremotefilenamebak[256];
      SNPRINTF(strremotefilenamebak,255,255,"%s/%s",starg.remotepathbak,vlistfile[acc].filename);
      ftp.ftprename(strremotefilename,strremotefilenamebak);
    }
    tempcount++;

    //׷�ӵ�vlistfile������
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


//��okfilename�ļ����ݼ��ص�vokfilename������
bool LoadOKFileName()
{
  vokfilename.clear();
  CFile File;
  if(File.Open(starg.okfilename,"r") == false)
    return true; //���δ��ļ����ܲ�����

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
  printf("����һ��ftp���ݲɼ��ĳ����밴���и�ʽ�������ݣ�\n");
  printf("���ܸ����ֶι��ܣ�\n");
  printf("host  Զ�̷�������IP�Ͷ˿�\n");
  printf("mode  ����ģʽ��pasv��port Ĭ��Ϊpasv\n");
  printf("username  Զ�̷�������¼�û���\n");
  printf("password  Զ�̷������ĵ�¼����\n");
  printf("localpath  �����ļ��Ĵ��Ŀ¼\n");
  printf("remotepath  Զ�̷������Ĵ��Ŀ���ļ���Ŀ¼\n");
  printf("matchname  ���ɼ��ļ�ƥ����û������ô�дƥ�䣬��֧��*ƥ��ȫ���ļ�\n");
  printf("ptype  �ļ��ɼ��ɹ���Զ�̷������ļ��Ĵ���ʽ ��1�� ʲ��Ҳ�� ��2����ɾ�� ��3���ɼ�����Զ�̷���������\n");
  printf("remotepathbak ��ptype=3ʱ����ָ������Ŀ¼\n");
  printf("listfilename  �г��ɼ�ǰ�������ļ������ļ�\n");
  printf("timetvl  ����ִ�м��\n");
  printf("okfilename  �г��ɹ��ɼ����ļ����嵥�ļ�\n\n");
  printf("Example:/htidc/public/bin/ftpgetfile /log/shqx/ftpgetfile_surfdata.log \"<host>119.29.18.109:21</host><mode>pasv</mode><username>oracle</username><password>zhululin123</password><localpath>/data/shqx/sdata/surfdata</localpath><remotepath>/data/shqx/ftp/surfdata</remotepath><matchname>SURF_*.txt</matchname><ptype>1</ptype><remotepathbak>/data/shqx/ftp/surfdatabak</remotepathbak><listfilename>/data/shqx/ftplist/ftpgetfile_surfdata.list</listfilename><okfilename>/data/shqx/ftplist/ftpgetfile_surfdata.xml</okfilename><timetvl>30</timetvl>\"\n\n");
}
