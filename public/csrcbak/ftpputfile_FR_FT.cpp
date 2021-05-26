#include "_freecplus.h"
#include "_ftp.h"

//�������
CLogFile logfile;
Cftp ftp;

//����������
vector<struct st_stfileinfo> vlistfile;    //��������д����Զ��Ŀ��Ŀ¼���ļ����ļ������޸�ʱ��
vector<struct st_stfileinfo> vlistfile1;   //��������������Ҫ�ٴ����͵��ļ���
vector<struct st_stfileinfo> vokfilename;  //��������������һ��������͵�����
vector<struct st_stfileinfo> vokfilename1; //��������������һ�β���Ҫ�������͵�����

//����������
bool _ftpgetfile(); //����������
bool getargs(char *argv);
bool LoadListFile();                                       //�ļ�list���ص�������
bool LoadOKFileName();                                     //��okfilename���ļ����ص�������
bool CompVector();                                         //�Ƚ�����������ȷ�������ɼ�����
bool Write_PToOKFileName();                                //��vokfilename��������д��okfilename���ļ���
bool AppendToOKFileName(struct st_stfileinfo *stfileinfo); //�����ͳɹ����ļ���¼׷�ӵ�okfilename�ļ���

void EXIT(int sig);
void _help(char *argv[]);

//ȫ�ֱ���������
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

int mode = FTPLIB_PASSIVE; //����������˴����ģʽ

//main�����Ŀ�ʼ
int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    _help(argv);
    return -1;
  }

  //CloseIOAndSignal();  // ���������������
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
  logfile.Write_P("�����˳� sig=%d\n", sig);
  exit(0);
}

bool _ftpgetfile()
{
  //  //�ı�ftp��Ŀ¼��Զ�����ͷ�����Ŀ¼��Ŀ����Ϊ�˻�ȡ�ļ����͸ı�ʱ��
  //  if(ftp.chdir(starg.remotepath) == false)
  //  {
  //    logfile.Write_P("ftp.chdir %s failed!\n",starg.remotepath);
  //    return false;
  //  }
  //  logfile.Write_P("ftp.chdir %s success!\n",starg.remotepath);
  //
  //
  //  //�г�ftp��Ŀ¼,��starg.listfilename����ļ���
  //  if(ftp.nlist(".",starg.listfilename) == false)
  //  {
  //    logfile.Write_P("ftp.nlist %s failed!\n",starg.listfilename);
  //    return false;
  //  }
  //  logfile.Write_P("ftp.nlist %s success!\n",starg.listfilename);

  //��Ŀ���ļ������ļ��е���Ϣ����vlistfile������
  if (LoadListFile() == false)
  {
    logfile.Write_P("LoadListFile %s failed!\n", starg.listfilename);
    return false;
  }
  logfile.Write_P("LoadListFile %s success!\n", starg.listfilename);

  //����һ��������ϵ��ļ����ļ������޸���Ϣ��ӵ�okfilename������
  if (LoadOKFileName() == false)
  {
    logfile.Write_P("LoadOKFileName() failed!\n");
    return false;
  }
  logfile.Write_P("LoadOKFileName() success!\n");

  //�л����ع���Ŀ¼
  char strremotefilename[256];
  char strlocalfilename[256];

  if (starg.ptype == 1)
  {
    //����һ��������ϵ��ļ����ļ������޸���Ϣ��ӵ�okfilename������
    if (LoadOKFileName() == false)
    {
      logfile.Write_P("LoadOKFileName() failed!\n");
      return false;
    }
    logfile.Write_P("LoadOKFileName() success!\n");

    //��vlistfile��������ļ���vokfilename���жԱȣ��õ���������
    //1.��vlistfile�д��ڣ��������ͳɹ����ļ�vokfilename1
    //2.��vlistfile�д��ڣ����ļ���Ҫ�������͵��ļ�vlistfile1
    if (CompVector() == false)
    {
      logfile.Write_P("CompVector() failed!\n");
      return false;
    }
    logfile.Write_P("CompVector() success!\n");

    //��vokfilename1�����е�������д��okfilename�У�����okfilename
    if (Write_PToOKFileName() == false)
    {
      logfile.Write_P("Write_PToOKFileName() failed!\n");
      return false;
    }
    logfile.Write_P("Write_PToOKFileName() success!\n");

    //��vlistfile1����������ݸ��Ƶ�vlistfile������
    //����vlistfile�����д�ŵľ�����Ҫ���͵��ļ�����
    vlistfile.clear();
    vlistfile.swap(vlistfile1);
  }

  //��ʼ����
  int tempcount = 0;
  for (int acc = 0; acc < vlistfile.size(); acc++)
  {
    //��ȡ�ļ�
    SNPRINTF(strlocalfilename, 255, 255, "%s/%s", starg.localpath, vlistfile[acc].filename);
    SNPRINTF(strremotefilename, 255, 255, "%s/%s", starg.remotepath, vlistfile[acc].filename);

    printf("putting %s ...\n", vlistfile[acc].filename);

    if (ftp.put(strlocalfilename, strremotefilename, true) == false)
    {
      logfile.Write_P("ftp.put %s failed!\n", vlistfile[acc].filename);
      break;
    }
    logfile.Write_P("ftp.put %s success!\n", vlistfile[acc].filename);

    //ɾ���ļ�
    if (starg.ptype == 2)
      REMOVE(strlocalfilename, 2);

    //ת�浽����Ŀ¼
    if (starg.ptype == 3)
    {
      char strlocalfilenamebak[256];
      SNPRINTF(strlocalfilenamebak, 255, 255, "%s/%s", starg.localpathbak, vlistfile[acc].filename);
      COPY(strlocalfilename, strlocalfilenamebak);
      REMOVE(strlocalfilename, 2);
    }
    tempcount++;

    //׷�ӵ�vlistfile������
    if (starg.ptype == 1)
      AppendToOKFileName(&vlistfile[acc]);
  }
  logfile.Write_P("ftp.put %d file success!\n", tempcount);
  return true;
}

//���������Ŀ���ǰѲ�������ȫ�ֵĽṹ�������
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

//����Զ�̷�������Ϣ������vlistfile��
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

//���������Ŀ���ǽ���һ��������ɵ��ļ������޸�ʱ����ص�vokfilename������
bool LoadOKFileName()
{
  vokfilename.clear();
  CFile File;

  //���δ��ļ����ܲ�����
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
    //���ļ������޸�ʱ����бȽϣ����vlistfile�޸�ʱ����ļ�����vokfilename�б���һ�����ʾ�������ͣ�����vokfilename1������
    int jj = 0;
    for (jj = 0; jj < vokfilename.size(); jj++)
    {
      if (strcmp(vlistfile[acc].filename, vokfilename[jj].filename) == 0 && strcmp(vlistfile[acc].mtime, vokfilename[jj].mtime) == 0)
      {
        vokfilename1.push_back(vlistfile[acc]);
        break;
      }
    }

    //����ҵ����һ��Ҳû������һ�µ����ʾ��Ҫ�������ͣ�����vlistfile1��
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

  //���ļ���д
  if (File.Open(starg.okfilename, "w") == false)
  {
    logfile.Write_P("Write_PToOKFileName(File.Open (%s)) failed!\n", starg.okfilename);
    return false;
  }
  logfile.Write_P("Write_PToOKFileName(File.Open (%s)) success!\n", starg.okfilename);

  //��xml��ʽ��������͵��ļ���д��okfilename�ļ���
  for (int acc = 0; acc < vokfilename1.size(); acc++)
    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vokfilename1[acc].filename, vokfilename1[acc].mtime);

  return true;
}

//���������Ŀ���ǽ��ɹ�������͵��ļ���д��okfilename.xml�ļ���
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
  printf("����һ��ftp�������͵ĳ����밴���и�ʽ�������ݣ�\n");
  printf("���ܸ����ֶι��ܣ�\n");
  printf("host          Զ�̷�������IP�Ͷ˿�\n");
  printf("mode          ����ģʽ��pasv��port Ĭ��Ϊpasv\n");
  printf("username      Զ�̷�������¼�û���\n");
  printf("password      Զ�̷������ĵ�¼����\n");
  printf("localpath     ����Ŀ���ļ��Ĵ��Ŀ¼\n");
  printf("remotepath    Զ�̷�������Ŀ��Ŀ¼\n");
  printf("matchname     �����ļ�ƥ����û������ô�дƥ�䣬��֧��*ƥ��ȫ���ļ�\n");
  printf("ptype         �ļ��ɼ��ɹ���Զ�̷������ļ��Ĵ���ʽ 1-ʲôҲ���� 2-���ͺ�ɾ�� 3-���ͺ��ڱ��ط���������\n");
  printf("localpathbak  ��ptype=3ʱ����ָ�����ر���Ŀ¼\n");
  printf("listfilename  �г�����ǰ�ļ������ļ�\n");
  printf("timetvl       ����ִ�м��\n");
  printf("okfilename    �г��ɹ����͵��ļ����嵥�ļ�\n\n");
  printf("Example:/htidc/public/bin/ftpputfile_FR_FT /log/shqx/ftpputfile_FR_FT_surfdata.log \"<host>119.29.18.109:21</host><mode>pasv</mode><username>oracle</username><password>zhululin123</password><localpath>/data/shqx/exp/tohb</localpath><remotepath>/data/shqx/ftpput</remotepath><matchname>SURFDATA*</matchname><ptype>3</ptype><localpathbak>/data/shqx/exp/tohbbak</localpathbak><listfilename>/data/shqx/ftplist/ftpputfile_surfdata.list</listfilename><okfilename>/data/shqx/ftplist/ftpputfile_surfdata.xml</okfilename><timetvl>30</timetvl>\"\n\n");
}
