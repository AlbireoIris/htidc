#include "_freecplus.h"
#include "_ooci.h"

//类的声明
CLogFile logfile;
connection conn;

vector<int>    vcmdstrnum; //用来存放字段的
vector<string> vcmdstrstr; //用来存放长度的

//参数结构体的声明
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
void  EXIT(int sig);          //退出信号的处理
void  _help(char* argv[]);    //提示信息
bool  _exptables();           //函数主流程
bool  getargs(char* argv);    //将输入参数放入结构体
bool  GetHisKey();            //获取历史增量参数
bool  UpdateKey();            //更新历史增量参数

//一些全局变量
int HisKey;                   //这个变量里存放了历史获取到的keyid，仅在模式为增量传输时发挥作用。
int maxfieldlen=0;            //这个变量存放了fieldlen的最大值


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

  if(getargs(argv[2]) == false)  
    return -1;

  if(conn.connecttodb(starg.connstr,starg.charset) != 0)
  {
    logfile.Write_P("conn.connecttodb %s failed!\n",starg.connstr);
    return -1;
  }

  while(true)
  {
    _exptables();
 
//    sleep(starg.timetvl);
    break;
  } 

  conn.commit();

  return 0;
}


//以下是函数的定义部分
//
//
//
//用来处理退出信号的函数部分
void EXIT(int sig)
{
  logfile.Write("程序退出 sig=%d\n",sig);
  exit(0);
}

//程序的主流程函数
bool _exptables()
{
  //获取增量历史 历史最大的key放入HisKey中
  if(starg.exptype == 1)
  {
    if(GetHisKey() == false)
    {
      logfile.Write_P("_exptables()/GetHisKey() failed!\n");
      return false; 
    }
  }

  
  //准备prepare语句
  char pretemp[2048];
  memset(pretemp,0,sizeof(pretemp));
  char fieldvalue[vcmdstrstr.size()][maxfieldlen+1]; //二维数组用来存放get数据

  sprintf(pretemp,"select %s ,keyid from %s where 1=1",starg.cols,starg.tname);

  if(starg.exptype == 1)
  {
    char addtemp[1024];
    memset(addtemp,0,sizeof(addtemp));
    sprintf(addtemp," and keyid > %d order by keyid",HisKey);
    strcat(pretemp,addtemp);
  }
  
  sqlstatement stmtget(&conn);
  if(stmtget.prepare(pretemp) != 0)
  {
    logfile.Write_P("exptables()/stmtget.prepare failed!\n%s\n",stmtget.m_cda.message);
    return false;
  }
   
//printf("pretemp = %s\n",pretemp);

  //将输出绑定到二维数组里
  for (int acc=0;acc<vcmdstrstr.size();acc++)
    stmtget.bindout(acc+1,fieldvalue[acc],vcmdstrnum[acc]);
  
  //如果为增量传输就要报HisKey字段和keyid绑定
  if (starg.exptype==1) 
    stmtget.bindout(vcmdstrstr.size()+1,&HisKey);

  if(stmtget.execute() != 0)
  {
    logfile.Write_P("exptables()/stmtget.execute() failed!\n%s\n",stmtget.m_cda.message);
    return false;
  } 

  int   filesql=1;      //生成的文件序号
  char  filename[256]; //拼接成的文件名
  char  datetime[32];  //当前时间
  CFile File;

  while(true)
  {
    //先清空在赋值
    memset(fieldvalue,0,sizeof(fieldvalue));

    if(stmtget.next() != 0)
      break;
     
    //文件没有打开的情况
    if (File.IsOpened()==false)
    {
      //获取当前时间
      memset(datetime,0,sizeof(datetime));
      LocalTime(datetime,"yyyymmddhh24miss");

      //预期的文件名
      memset(filename,0,sizeof(filename));
      sprintf(filename,"%s/%s%s%s_%d.xml",starg.exppath,starg.bname,datetime,starg.ename,filesql++);

      //打开文件并重命名为filename.tmp
      if (File.OpenForRename(filename,"w")==false)
      {
        logfile.Write_P("File.OpenForRename(%s) failed.\n",filename); 
        return false;
      }

      File.Fprintf("<data>\n");
    }
   
    //开始写入数据
    for(int acc=0;acc<vcmdstrstr.size();acc++)
      File.Fprintf("<%s>%s</%s>",vcmdstrstr[acc].c_str(),fieldvalue[acc],vcmdstrstr[acc].c_str());

    //一行写完在行末加endl
    File.Fprintf("<endl/>\n");

    // 1000条记录写入一个文件完成
    // stmt.m_cda.rpc的意思是影响1000条数据
    // 用%号是考虑到一张表里数据条数超过1000条的情况
    if (stmtget.m_cda.rpc%1000 ==0)
    {
      //文件末尾加上</data>
      File.Fprintf("</data>\n");
      
      //关闭文件并将filename.tmp的tmp去除
      if (File.CloseAndRename()==false)
      {
        logfile.Write_P("exptables/File.CloseAndRename(%s) failed.\n",filename); 
        return false;
      }

      // 更新系统参数T_SYSARG表中已导出数据的最大的keyid
      if (UpdateKey()==false) 
      { 
        logfile.Write_P("exptables/updatekey() failed.\n"); 
        return false; 
      }
      logfile.Write_P("create file %s ok.\n",filename);
    }
  }
  
  //如果跳出循环后文件没有关闭说明没到1000
  if (File.IsOpened()==true)
  {
    //同循环内一样的处理流程
    File.Fprintf("</data>\n");
    if (File.CloseAndRename()==false)
    {
      logfile.Write("File.CloseAndRename(%s) failed.\n",filename); 
      return false;
    }

    // 更新系统参数T_SYSARG表中已导出数据的最大的keyid
    if (UpdateKey()==false) 
    { 
      logfile.Write("UptMaxKeyid() failed.\n"); 
      return false; 
    }

    logfile.Write_P("create file %s ok.\n",filename);
  }

//  if (stmtget.m_cda.rpc>0) 
    logfile.Write_P("本次导出了%d条记录。\n",stmtget.m_cda.rpc);

  return true;
}

//将输入的参数获取，拆分，存放的函数
bool getargs(char* argv) 
{
  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(argv,"connstr",  starg.connstr);
  GetXMLBuffer(argv,"charset",  starg.charset);
  GetXMLBuffer(argv,"tname",    starg.tname);
  GetXMLBuffer(argv,"cols",     starg.cols);
  GetXMLBuffer(argv,"fieldname",starg.fieldname);
  GetXMLBuffer(argv,"filedlen", starg.filedlen);
  GetXMLBuffer(argv,"andstr",   starg.andstr);
  GetXMLBuffer(argv,"bname",    starg.bname);
  GetXMLBuffer(argv,"ename",    starg.ename);
  GetXMLBuffer(argv,"taskname", starg.taskname);
  GetXMLBuffer(argv,"exppath",  starg.exppath);
  GetXMLBuffer(argv,"exptype", &starg.exptype);
  GetXMLBuffer(argv,"timetvl", &starg.timetvl);

  CCmdStr CmdStrstr; //用来拆分字段的
  CCmdStr CmdStrnum; //用来拆分长度的

  CmdStrstr.SplitToCmd(starg.fieldname,",");
  CmdStrnum.SplitToCmd(starg.filedlen,",");

  if(CmdStrstr.CmdCount() != CmdStrnum.CmdCount())
  {
    logfile.Write_P("getargs failed! cols字段与filedlen字段不匹配\n");
    return false;
  }

  for(int acc=0;acc<CmdStrstr.CmdCount();acc++)
  {
    int inttemp=atoi(CmdStrnum.m_vCmdStr[acc].c_str());
    if(maxfieldlen < inttemp)
      maxfieldlen=inttemp; 
    string stringtemp=CmdStrstr.m_vCmdStr[acc];
    vcmdstrstr.push_back(stringtemp);
    vcmdstrnum.push_back(inttemp);
  }
  return true;
}

//提示的函数
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
  printf("Example:/htidc/public/bin/exptables_FR_OR  /log/shqx/exptables_surfdata.log  \"<connstr>shqx/pwdidc@snorcl11g_119</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_SURFDATA</tname><cols>obtid,to_char(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis</cols><fieldname>obtid,ddatetime,t,p,u,wd,wf,r,vis</fieldname><filedlen>5,33,8,8,8,8,8,8,8</filedlen><exptype>1</exptype><andstr>and obtid in('59293','50745')</andstr><bname>SURFDATA_</bname><ename>_FOR_HB</ename><taskname>SURFDATA_FOR_HB</taskname><exppath>/data/shqx/exp/tohb</exppath><timetvl>10</timetvl>\"\n\n");
}


//这个函数的作用是获取历史最大的key值
bool GetHisKey()
{
  sqlstatement stmtkey(&conn);
  stmtkey.prepare("select keyid from T_SYSDATA where taskname=:1");
  stmtkey.bindin(1,starg.taskname,20);
  stmtkey.bindout(1,&HisKey);

  if(stmtkey.execute() != 0)
  {
    logfile.Write_P("stmtkey.execute(select keyid from T_SYSDATA) failed!\n%s\n",stmtkey.m_cda.message);
    return false;
  }

  //stmtkey.next() !=0 表示获取字符集失败既字符集为空，插入一条数据
  if(stmtkey.next() != 0)
  {
    HisKey=0;
    stmtkey.prepare("insert into T_SYSDATA (taskname,keyid) values(:1,:2)");
    stmtkey.bindin(1,starg.taskname,20);
    stmtkey.bindin(2,&HisKey);
    if(stmtkey.execute() != 0)
    {
      logfile.Write_P("stmtkey.execute(insert into T_SYSDATA (taskname,keyid)) failed!\n%s\n",stmtkey.m_cda.message);
      return false;
    }
    //一定要在函数内部提交，不然会因为局部变量销毁而不报错又无预期结果
    conn.commit();
  }
  return true;
}

bool UpdateKey()
{
  sqlstatement stmtupdate(&conn);
  stmtupdate.prepare("update T_SYSDATA set keyid=:1 where taskname=:2");
  stmtupdate.bindin(1,&HisKey);
  stmtupdate.bindin(2,starg.taskname,20);

  if(stmtupdate.execute() != 0)
  {
    logfile.Write_P("stmtupdate.execute(select keyid from T_SYSDATA) failed!\n%s\n",stmtupdate.m_cda.message);
    return false;
  }

  conn.commit();

  return true;
}



