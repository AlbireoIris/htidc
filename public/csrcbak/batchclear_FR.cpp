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
    printf("����־�ļ�ʧ�ܣ�\n");
    return -1;
  }

  if(File.Open("/htidc/public/c/cleararg.xml","r") == false)
  {
    logfile.Write_P("�򿪲����ļ� /htidc/public/c/cleararg.ini ʧ�ܣ�\n");
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
//  string TargetDir; //�������������������ļ���Ŀ��Ŀ¼
//  string MatchRule; //��������������ƥ��Ĺ���
//  string CleanTime; //��������������ƥ���ʱ��,��λ������
//  int    MinNum;    //�������������ű����ļ�������
//  
//  for(int acc = 0;acc<cleanarg.count();acc++)
//  {
//    BatchClean()  
  return 0;
}
