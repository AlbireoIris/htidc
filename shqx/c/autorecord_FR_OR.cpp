#include "_freecplus.h"
#include "_mylib.h"
#include "_ooci.h"

struct st_autodata
{
  char stationid[7];    
  char observetime[33];
  char observealt[7]; 
  int  stationpress;            
  int  tempre;           
  int  lowesttempre24;         
  int  rainperhour;         
  int  min2wd;          
  int  min2wf;
  int  earthtempre;
  int  min1vis;
  int  vis;
  char snowdepth[7];
  char huorperminrain[121];
  char manobserve[121];        
};

CLogFile logfile;

CDir Dir;

bool _psurfdata();

void EXIT(int sig);

char argv1[32];
char argv2[32];
char argv3[32];

int main(int argc,char *argv[])
{
//  if (argc!=5)
//  {
//    printf("\n本程序用于处理全国气象站点观测的分钟数据，并保存到数据库的T_SURFDATA表中。\n");
//    printf("/htidc/shqx/bin/psurfdata 数据文件存放的目录 日志文件名 数据库连接参数 程序运行时间间隔\n");
//    printf("例如：/htidc/shqx/bin/psurfdata /data/shqx/sdata/surfdata /log/shqx/psurfdata.log shqx/pwdidc@snorcl11g_119 10\n");
//    return -1;
//  }
//
//  // 关闭全部的信号和输入输出
//  CloseIOAndSignal();

  memset(argv1,0,sizeof(argv1));
  memset(argv1,0,sizeof(argv2));
  memset(argv1,0,sizeof(argv2));
  
  strcpy(argv1,"/data/shqx/autorecord");
  strcpy(argv2,"/log/shqx/pautodata.log");
  strcpy(argv3,"shqx/pwdidc@snorcl11g_119");
  
  // 处理程序退出的信号
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv2,"a+")==false)
  {
    printf("打开日志文件失败（%s）。\n",argv2); return -1;
  }

  logfile.Write_P("程序启动。\n");

//  while (true)
//  {
    if (Dir.OpenDir(argv1,"*",1000,true,true)==false)
    {
      logfile.Write_P("Dir.OpenDir(%s) failed.\n",argv1); 
      //sleep(atoi(argv[4])); 
      //continue;
    }

    while (true)
    {
      if (Dir.ReadDir()==false) break;
  
      logfile.Write_P("开始处理文件%s...",Dir.m_FileName);
  
      if (_psurfdata()==false) 
      {
        logfile.Write_P("失败。\n"); 
        break;
      }
      logfile.Write_P("成功。\n");
      //break;
    }

    // 断开与数据库的连接
//    if (conn.m_state==1) 
//      conn.disconnect(); 

//    sleep(atoi(argv[4]));
//  }

  return 0;
}

void EXIT(int sig)
{
  logfile.Write_P("程序退出，sig=%d\n\n",sig);

  exit(0);
}
     
bool _psurfdata()
{
  CFile File;
  CCmdStr CmdStr;
  CQuickBind quickbind("T_AUTODATA",argv3);
  struct st_autodata stautodata;
  memset(&stautodata,0,sizeof(st_autodata));
  strcpy(stautodata.stationid,"world");
  strcpy(stautodata.observetime,"20181001001000");

  quickbind.Bind(stautodata.stationid,"stationid",6,true);   
  quickbind.Bind(stautodata.observetime,"observetime","yyyymmddhh24miss",true);   
  quickbind.Bind(stautodata.observealt,"observeralt",6);   
  quickbind.Bind(stautodata.stationpress,"stationpress");   
  quickbind.Bind(stautodata.tempre,"tempre");   
  quickbind.Bind(stautodata.lowesttempre24,"lowesttempre24");   
  quickbind.Bind(stautodata.rainperhour,"rainperhour");   
  quickbind.Bind(stautodata.min2wd,"min2wd");   
  quickbind.Bind(stautodata.min2wf,"min2wf");   
  quickbind.Bind(stautodata.earthtempre,"earthtempre");   
  quickbind.Bind(stautodata.min1vis,"min1vis");   
  quickbind.Bind(stautodata.vis,"vis");   
  quickbind.Bind(stautodata.snowdepth,"snowdepth",6);   
  quickbind.Bind(stautodata.huorperminrain,"huorperminrain",120);   
  quickbind.Bind(stautodata.manobserve,"manobserve",120);   

  if(quickbind.Prepare() == false)
  {
    logfile.Write_P("quickbind.Prepare failed!");
    return false;
  }

  if(quickbind.Bindin() == false)
  {
    logfile.Write_P("quickbind.Bindin failed!");
    return false;
  }

  if (File.Open(Dir.m_FullFileName,"r")==false)
  {
    logfile.Write_P("(File.Open(%s) failed.\n",Dir.m_FullFileName); 
    return false;
  }

  char strbuffer[256];

  while(true)
  {
    memset(&stautodata,0,sizeof(struct st_autodata));
    memset(strbuffer,0,sizeof(strbuffer));
    
    File.Fgets(strbuffer,255);
    if(strcmp(strbuffer,"NNNN") == 0)
      break;
   
    UpdateStr(strbuffer,"  "," "); 
    CmdStr.SplitToCmd(strbuffer," ");
//    if(CmdStr.CmdCount() != 9)
//    {
//      logfile.Write_P("\nHandline Error %s\n",strbuffer);
//      return false;  
//    }
    CmdStr.GetValue(0,stautodata.stationid);
    CmdStr.GetValue(1,stautodata.observetime);
    CmdStr.GetValue(4,stautodata.observealt);

    for(int acc=0;acc<14;acc++)
    {
      CCmdStr Cmdstr;
      memset(strbuffer,0,sizeof(strbuffer));
      File.Fgets(strbuffer,255);
      
      UpdateStr(strbuffer,"  "," ");
      Cmdstr.SplitToCmd(strbuffer," ");
                              
      switch(acc+1)
      {
        case  1: { Cmdstr.GetValue(1,&stautodata.stationpress);  break; }
        case  2: { Cmdstr.GetValue(1,&stautodata.tempre);  Cmdstr.GetValue(8,&stautodata.lowesttempre24);  break; }
        case  3: { Cmdstr.GetValue(1,&stautodata.rainperhour);   break; }
        case  4: { Cmdstr.GetValue(1,&stautodata.min2wd);  Cmdstr.GetValue(2,&stautodata.min2wf);  break; } 
        case  5: { Cmdstr.GetValue(1,&stautodata.earthtempre);   break; } 
        case  6: { Cmdstr.GetValue(1,&stautodata.min1vis);   break; }
        case  7: { Cmdstr.GetValue(1,&stautodata.vis);   break; }
        case  8: { Cmdstr.GetValue(1,stautodata.snowdepth,6);   break; }
        case  9: { Cmdstr.GetValue(1,stautodata.huorperminrain,120);   break; }
        case 10: { Cmdstr.GetValue(1,stautodata.manobserve,120);   break; }
        case 11: {}
        case 12: {}
        case 13: {}
        default: { break; }
      }    
    }
    
    if(quickbind.Execute() == false)
    {
      quickbind.Error(); 
      return false;
    } 
  }

  quickbind.Report();

// 关闭文件指针，并删除文件
// File.CloseAndRemove();

  return true;
}

