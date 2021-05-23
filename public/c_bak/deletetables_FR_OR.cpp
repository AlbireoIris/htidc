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
  logfile.Write_P("��������\n");

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
  printf("�յ��ź� %d �����˳�!\n");
  exit(0);
}

void _help(char* argv[])
{
  printf("\n");
  printf("Using: /htidc/public/bin/deletetables_FR_OR \"<logfilename>/log/shqx/deletetables_FR_OR_SURFDATA.log</logfilename><connstr>shqx/pwdidc@snorcl11g_119</connstr><tname>t_surfdata</tname><where>2021-4-9 16:40:00</where><hourstr>11,12,13</hourstr>\"\n");
  printf("����һ�����߳�����������������ݣ�\n");
  printf("logfilename  ������������־��\n");
  printf("connstr      Ŀ�����ݿ�����Ӳ���\n");
  printf("tname        ������ı���\n");
  printf("where        ���������������\n");
  printf("hourstr      ������������ʱ�Σ�Сʱ\n");
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
  
