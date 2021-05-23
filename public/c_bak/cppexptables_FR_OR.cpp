#include "_freecplus.h"
#include "_ooci.h"

using namespace std;

//类的声明
CLogFile logfile;
connection conn;

//参数定义结构体的声明
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

//函数的声明
void EXIT(int sig);          //退出信号的处理
void _help(char* argv[]);    //提示信息
bool _exptables();           //函数主流程
void getargs(char* argv);  //将输入参数放入结构体



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
  logfile.Write("程序退出 sig=%d\n",sig);
  exit(0);
}


bool _exptables()
{
  return true;
}

void getargs(char* argv)  //将输入参数放入结构体
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
  printf("这是一个ftp数据推送服务的程序，请按下列格式输入数据！\n");
  printf("介绍各个字段功能：\n");
  printf("logfilename  日志文件的文件路径，建议采用绝对路径\n");
  printf("connstr      数据库连接参数\n");
  printf("charset      数据库的字符集，要与数据库的字符集保持一致，否则会产生字符乱码的现象\n");
  printf("tname        等待导出打的数据库的表名\n");
  printf("cols         需要导出字段的列表，可以采用函数\n");
  printf("fieldname    需要导出字段的别名列表\n");
  printf("filedlen     需要导出字段的长度，必须与clos一一对应\n");
  printf("exptype      导出模式，1-增量导出，2-全量导出\n");
  printf("andstr       导出数据的附加条件\n");
  printf("bname        导出文件命名的前半部分\n");
  printf("ename        导出文件命名的后半部分\n");
  printf("taskname     导出任务的命名\n");
  printf("exppath      导出文件存放的路径\n");
  printf("timetvl      导出文件的时间间隔，单位秒，建议大于10秒\n");
  printf("\n");
  printf("Example:/htidc/public/bin/exptables_FR_OR  /log/shqx/exptables_surfdata.log  \"<connstr>shqx/pwdidc@snorcl11g_119</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_SURFDATA</tname><cols>obtid,to_date(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis</cols><fieldname>obtid,ddatetime,t,p,u,wd,wf,r,vis</fieldname><filedlen>5,14,8,8,8,8,8,8,8</filedlen><exptype>1</exptype><andstr>and obtid in('59293','50745')</andstr><bname>SURFDATA_</bname><ename>_FOR_HB</ename><taskname>SURFDATA_FOR_HB</taskname><exppath>data/shqx/exp/tohb</exppath><timetvl>10</timetvl>\"\n\n");
}
