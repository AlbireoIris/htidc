#include"_freecplus.h"
#include"_ooci.h"

void EXIT(int sig);
void _help(char* argv[]);
bool _deletetables();

CLogFile logfile;
connection conn;

struct st_args
{
  char logfilename[256];
  char where[128];
  char connstr[64];
  char tname[32];
  char hourstr[32];
}starg;



int main(int argc,char* argv[])
{
  if(argc != 2)
  {
    _help(argv);
    return -1;
  }
 
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);

  memset(&starg,0,sizeof(struct st_args));
  GetXMLBuffer(argv[1],"logfilename",starg.logfilename);
  GetXMLBuffer(argv[1],"where",starg.where);
  GetXMLBuffer(argv[1],"connstr",starg.connstr);
  GetXMLBuffer(argv[1],"tname",starg.tname);
  GetXMLBuffer(argv[1],"hourstr",starg.hourstr);
 
  if(logfile.Open(starg.logfilename,"a+") == false)
  {
    logfile.Write_P("open the logfile %s failed!\n",starg.logfilename);
    return -1;
  }
  logfile.Write_P("程序启动\n");

  if(conn.m_state == 0)
    if(conn.connecttodb(starg.connstr,"Simplified Chinese_China.ZHS16GBK") != 0)
      logfile.Write_P("conn.connecttodb %s failed! %s\n",starg.connstr,conn.m_cda.message);
  logfile.Write_P("conn.connecttodb %s success!\n",starg.connstr);

  if(_deletetables() == false)
    logfile.Write_P("_deletetables() failed!\n"); 
  else
    logfile.Write_P("_deletetables() success!\n");
 
  return 0;
}

void EXIT(int sig)
{
  printf("收到信号 %d 程序退出!\n");
  exit(0);
}

void _help(char* argv[])
{
  printf("\n");
  printf("Using: /htidc/public/bin/deletetables_FR_OR \"<logfilename>/log/shqx/deletetables_FR_OR_SURFDATA.log</logfilename><connstr>shqx/pwdidc@snorcl11g_119</connstr><tname>t_surfdata</tname><where>2021-4-9 16:40:00</where><hourstr>11,12,13</hourstr>\"\n");
  printf("这是一个工具程序，用来清理表中数据！\n");
  printf("logfilename  本程序运行日志名\n");
  printf("connstr      目的数据库的连接参数\n");
  printf("tname        待清理的表名\n");
  printf("where        待清理的数据条件\n");
  printf("hourstr      本程序启动的时次，小时\n");
  printf("\n");
}

bool _deletetables()
{
  sqlstatement stmtsel(&conn);
  
  stmtsel.prepare("delete from %s where ddatetime < to_date('%s','YYYY-MM-DD HH24:mi:ss')",starg.tname,starg.where);

  if(stmtsel.execute() != 0)
  {
    logfile.Write_P("stmtsel.execute() failed!\nstmtsel.m_sql:%s\nstmtsel.m_cda.message:%s\n",stmtsel.m_sql,stmtsel.m_cda.message);
    return false;
  }

  conn.commit();
   
  return true;
} 
  
