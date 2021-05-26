#include"_freecplus.h"

CFile File;
CDir  Dir;
CLogFile logfile;

vector<struct st_cleanrule> cleanarg;

struct st_cleanrule
{
  char TargetDir[256];
  char MatchRule[256];
  char CleanTime[64];
  int  MinNum;
}stcleanrule;
     
int main()
{
  if(logfile.Open("/log/publiclog/batchclear_FR.log") == false)
  {
    printf("打开日志文件失败！\n");
    return -1;
  }

  if(File.Open("/htidc/public/c/cleararg.xml","r") == false)
  {
    logfile.Write_P("打开参数文件 /htidc/public/c/cleararg.ini 失败！\n");
    return false;
  }

  while(true)
  {
    char strbuffer[1024];
    memset(strbuffer,0,sizeof(strbuffer));

    if(File.Fgets(strbuffer,1023) == false)
      break;
   
    if(strncmp(strbuffer,"//",2) == 0)
      continue;
    
    char numtemp[64];
    memset(numtemp,0,sizeof(numtemp));
    memset(&stcleanrule,0,sizeof(struct st_cleanrule));
    GetXMLBuffer(strbuffer,"TargetDir",stcleanrule.TargetDir);
    GetXMLBuffer(strbuffer,"MatchRule",stcleanrule.MatchRule);
    GetXMLBuffer(strbuffer,"CleanTime",stcleanrule.CleanTime);
    GetXMLBuffer(strbuffer,"MinNum",&stcleanrule.MinNum);
//    StrToNum(numtemp);
  }
//  printf("a = %d\n",atoi("5*6"));
//  string TargetDir; //这个参数用来存放清理文件的目标目录
//  string MatchRule; //这个参数用来存放匹配的规则
//  string CleanTime; //这个参数用来存放匹配的时间,单位：分钟
//  int    MinNum;    //这个参数用来存放保留文件的数量
//  
//  for(int acc = 0;acc<cleanarg.count();acc++)
//  {
//    BatchClean()  
  return 0;
}
