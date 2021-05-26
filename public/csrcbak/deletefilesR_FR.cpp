#include "_freecplus.h"

void _help(char *argv[]);
void EXIT(int sig);

int main(int argc, char *argv[])
{
  if ((argc != 3) && (argc != 4))
  {
    _help(argv);
    return -1;
  }

  //CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  char strPathName[201];
  double dDayOut = 0;

  memset(strPathName, 0, sizeof(strPathName));

  strcpy(strPathName, argv[1]);
  dDayOut = atof(argv[2]);

  char strTimeOut[21];

  LocalTime(strTimeOut, "yyyy-mm-dd hh:mi:ss", 0 - (int)(dDayOut * 24 * 60 * 60));

  CDir Dir;

  char strMatch[50];
  memset(strMatch, 0, sizeof(strMatch));
  if (argc == 3)
    strcpy(strMatch, "*");
  else
    strcpy(strMatch, argv[3]);

  if (Dir.OpenDir(strPathName, strMatch, 10000, true, false) == false)
  {
    printf("Dir.OpenDir %s failed!\n", strPathName);
    return -1;
  }

  char strlocalTime[21];

  while (Dir.ReadDir() == true)
  {
    if (strcmp(Dir.m_ModifyTime, strTimeOut) > 0)
      continue;
    if (REMOVE(Dir.m_FullFileName) == false)
      printf("删除 %s 失败！", Dir.m_FullFileName);

    printf("删除 %s 成功！", Dir.m_FullFileName);
  }

  return 0;
}

void EXIT(int sig)
{
  printf("收到信号 %d 程序退出！\n");
  exit(0);
}

void _help(char *argv[])
{
  printf("\n");
  printf("本程序用来清理历史文件\n");
  printf("使用格式：/htidc/shqx/bin/deletefiles  目标文件夹  目标清理时间  匹配规则【选填】\n");
  printf("example : /htidc/shqx/bin/deletefiles /data/shqx/ftp/surfdata 20210409000000\n");
  printf("Using:/htidc/shqx/bin/deletefiles pathname dayout [matchstr]\n\n");
}
