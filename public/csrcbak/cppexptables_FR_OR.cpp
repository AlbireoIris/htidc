#include "_freecplus.h"
#include "_ooci.h"

using namespace std;

//�������
CLogFile logfile;
connection conn;

//��������ṹ�������
struct st_arg
{
   char connstr[64];
   char charset[64];
   char tname[64];
   char cols[64];
   char fieldname[256];
   char filedlen[128];
   char andstr[256];
   char bname[256];
   char ename[256];
   char taskname[256];
   char exppath[256];
   int  exptype;
   int  timetvl;
}starg;

//����������
void EXIT(int sig);          //�˳��źŵĴ���
void _help(char* argv[]);    //��ʾ��Ϣ
bool _exptables();           //����������
void getargs(char* argv);  //�������������ṹ��



int main(int argc,char* argv[])
{

  if(argc != 3)
  { 
    _help(argv);
    return -1;
  }

//  CloseIOAndSignal(); 
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);

  if(logfile.Open(argv[1],"a+") == false)
  {
    printf("open the logfile: %s failed\n",argv[3]);
    return -1;
  }

  getargs(argv[2]);

  if(conn.connecttodb(starg.connstr,starg.charset) != 0)
  {
    logfile.Write_P("conn.connecttodb %s failed!\n",starg.connstr);
    return false;
  }

  while(true)
  {
    _exptables();
  
    sleep(starg.timetvl);
  } 
  return 0;
}


void EXIT(int sig)
{
  logfile.Write("�����˳� sig=%d\n",sig);
  exit(0);
}


bool _exptables()
{
  return true;
}

void getargs(char* argv)  //�������������ṹ��
{
  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(argv,"connstr",starg.connstr);
  GetXMLBuffer(argv,"charset",starg.charset);
  GetXMLBuffer(argv,"tname",starg.tname);
  GetXMLBuffer(argv,"cols",starg.cols);
  GetXMLBuffer(argv,"fieldname",starg.fieldname);
  GetXMLBuffer(argv,"filedlen",starg.filedlen);
  GetXMLBuffer(argv,"andstr",starg.andstr);
  GetXMLBuffer(argv,"bname",starg.bname);
  GetXMLBuffer(argv,"ename",starg.ename);
  GetXMLBuffer(argv,"taskname",starg.taskname);
  GetXMLBuffer(argv,"exppath",starg.exppath);
  GetXMLBuffer(argv,"exptype",&starg.exptype);
  GetXMLBuffer(argv,"timetvl",&starg.timetvl);
}

void _help(char* argv[])
{
  printf("\n");
  printf("����һ��ftp�������ͷ���ĳ����밴���и�ʽ�������ݣ�\n");
  printf("���ܸ����ֶι��ܣ�\n");
  printf("logfilename  ��־�ļ����ļ�·����������þ���·��\n");
  printf("connstr      ���ݿ����Ӳ���\n");
  printf("charset      ���ݿ���ַ�����Ҫ�����ݿ���ַ�������һ�£����������ַ����������\n");
  printf("tname        �ȴ�����������ݿ�ı���\n");
  printf("cols         ��Ҫ�����ֶε��б����Բ��ú���\n");
  printf("fieldname    ��Ҫ�����ֶεı����б�\n");
  printf("filedlen     ��Ҫ�����ֶεĳ��ȣ�������closһһ��Ӧ\n");
  printf("exptype      ����ģʽ��1-����������2-ȫ������\n");
  printf("andstr       �������ݵĸ�������\n");
  printf("bname        �����ļ�������ǰ�벿��\n");
  printf("ename        �����ļ������ĺ�벿��\n");
  printf("taskname     �������������\n");
  printf("exppath      �����ļ���ŵ�·��\n");
  printf("timetvl      �����ļ���ʱ��������λ�룬�������10��\n");
  printf("\n");
  printf("Example:/htidc/public/bin/exptables_FR_OR  /log/shqx/exptables_surfdata.log  \"<connstr>shqx/pwdidc@snorcl11g_119</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_SURFDATA</tname><cols>obtid,to_date(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis</cols><fieldname>obtid,ddatetime,t,p,u,wd,wf,r,vis</fieldname><filedlen>5,14,8,8,8,8,8,8,8</filedlen><exptype>1</exptype><andstr>and obtid in('59293','50745')</andstr><bname>SURFDATA_</bname><ename>_FOR_HB</ename><taskname>SURFDATA_FOR_HB</taskname><exppath>data/shqx/exp/tohb</exppath><timetvl>10</timetvl>\"\n\n");
}
