#include"_freecplus.h"
#include"_ooci.h"
#include"_mylib.h"

struct st_starg
{
  char filename[256];
  char localname[256];
  int  filesize;
}starg;

CLogFile logfile;
CDir Dir;
connection conn;

int main(int argc,char* argv[])
{
  if(argc != 3)
  {
    printf("这个程序的功能是将文件从数据库中取出并放入指定的目录\n");
    printf("./psurfdbtopic_FR_OR /data/shqx/picdata /log/shqx/psurfpictodb_FR_OR.log\n");
    return -1;
  }

  if(logfile.Open(argv[2]) == false)
  {
    printf("open the logfile failed!\n");
    return -1;
  }

  if(Dir.OpenDir(argv[1],"*JPG") == false)
  {
    printf("open the dir falied!\n");
    return -1;
  }
  
  if(conn.connecttodb("shqx/pwdidc@snorcl11g_119","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    logfile.Write_P("conn.connecttodb failed!\n");
    return -1;
  }

  while(true) 
  {
    if(Dir.ReadDir() == false)
      break;
  
    memset(&starg,0,sizeof(struct st_starg));
    strcpy(starg.filename,Dir.m_FullFileName);  
    strcpy(starg.localname,Dir.m_FileName);
    starg.filesize = Dir.m_FileSize;
//    sqlstatement stmtins(&conn);
    char constr[1024];
    memset(constr,0,sizeof(constr));
    sprintf(constr,"filename='%s'",starg.filename);

    if(TableToFile(&conn,"T_SURFPIC","filecon",constr,starg.localname,&logfile) != 0)
    {
      printf("TableToFile Failed!\n"); 
      break;
    }
//    stmtins.prepare("select filecon from T_SURFPIC where filename=:1");
//    stmtins.bindin(1,starg.filename,255);
//    stmtins.bindblob();
//
//    if(stmtins.execute() != 0)
//    {
//      logfile.Write_P("stmt.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
//      continue;
//    } 
//
//    if (stmtins.next() != 0)
//    {
//      logfile.Write_P("stmtins.bindblob() stmtins.next() failed!\n");
//      logfile.Write_P("\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
//      continue;
//    }
//    
//    if (stmtins.lobtofile((char *)starg.localname) != 0)
//    {
//      logfile.Write_P("stmt.filetolob() failed.\n%s\n",stmtins.m_cda.message); 
//      return -1;
//    } 
   
    logfile.Write_P("putout %s success!\n",starg.filename);
    break;
  }
  
  conn.commit();  

  return 0;
}
