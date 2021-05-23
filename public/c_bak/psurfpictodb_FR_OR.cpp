#include"_freecplus.h"
#include"_ooci.h"
#include"_mylib.h"


struct st_starg
{
  char filename[256];
  char datetime[21];
  int  filesize;
}starg;


CLogFile logfile;
CDir Dir;
connection conn;
connection fconn;


int main(int argc,char* argv[])
{
  if(argc != 3)
  {
    printf("这个程序的作用是把目标文件夹的文件放入数据库中\n");
    printf("./psurfpictodb_FR_OR /data/shqx/picdata /log/shqx/psurfpictodb_FR_OR.log\n");
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

  if(fconn.connecttodb("shqx/pwdidc@snorcl11g_119","Simplified Chinese_China.ZHS16GBK") != 0)
  {
    logfile.Write_P("fconn.connecttodb failed!\n");
    return -1;
  }

  while(true) 
  {
    if(Dir.ReadDir() == false)
      break;
    
    memset(&starg,0,sizeof(struct st_starg));
    strcpy(starg.filename,Dir.m_FullFileName);  
    strcpy(starg.datetime,Dir.m_ModifyTime);
    starg.filesize = Dir.m_FileSize;
    sqlstatement stmtins(&conn);
    
//  判断文件记录在表中是否已存在
    int icount=0;
    stmtins.prepare("select count(*) from T_SURFPIC where filename=:1");
    stmtins.bindin(1,starg.filename,255);
    stmtins.bindout(1,&icount);
    if (stmtins.execute() != 0)
    {
      logfile.Write_P("FileToTable() Failed.%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message); 
      continue;
    }

    stmtins.next();

    if (icount>0) 
      continue;
      
    stmtins.prepare("insert into T_SURFPIC (filename,datetime,filesize,filecon,keyid) values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,empty_blob(),SEQ_SURFPIC.nextval)"); 
  //必须在filecon字段插入empty_blob()函数，否则会报错
  //stmtins.prepare("insert into T_SURFPIC (filename,datetime,filesize,keyid) values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,SEQ_SURFPIC.nextval)"); 
    stmtins.bindin(1,starg.filename,255);
    stmtins.bindin(2,starg.datetime,20);
    stmtins.bindin(3,&starg.filesize);
     
    if(stmtins.execute() != 0)
    {
      logfile.Write_P("stmt.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      continue;
    }      
    
//  char constr[1024];
//  memset(constr,0,sizeof(constr));
//  sprintf(constr,"filename='%s'",starg.filename);
//  
//  if(FileToTable(&conn,"T_SURFPIC","filecon",constr,starg.filename,&logfile) != 0)
//  {
//    logfile.Write_P("FileToTable Failed!\n");
//    continue;
//  }

    stmtins.prepare("select filecon from T_SURFPIC where filename=:1 for update");
    stmtins.bindin(1,starg.filename,255);
    stmtins.bindblob();

    if(stmtins.execute() != 0)
    {
      logfile.Write_P("stmt.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      continue;
    } 

    if (stmtins.next() != 0)
    {
      logfile.Write_P("\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      continue;
    }
    
    if (stmtins.filetolob((char *)starg.filename) != 0)
    {
      logfile.Write_P("stmt.filetolob() failed.\n%s\n",stmtins.m_cda.message); 
      return -1;
    } 

    logfile.Write_P("Insert %s Success!\n",starg.filename);
  }

  conn.commit();  

  return 0;
}
