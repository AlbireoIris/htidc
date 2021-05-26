#include "_freecplus.h"

CDir Dir;

int main(int argc, char *argv[])
{
   if ((argc != 3) && (argc != 4))
  {
    printf("\n");
    printf("本程序用来清理历史文件\n");
    printf("使用格式：/htidc/shqx/bin/deletefiles  目标文件夹  目标清理时间  匹配规则【选填】\n");
    printf("example : /htidc/shqx/bin/deletefiles /data/shqx/ftp/surfdata 20210409000000\n");
    printf("Using:/htidc/shqx/bin/deletefiles pathname dayout [matchstr]\n\n");
    return -1;
  }

  //存放输入时间
  char localtime[32];
  char finaltime[32];
  memset(localtime, 0, sizeof(localtime));
  LocalTime(localtime, "yyyymmddhh24miss");
  if (strlen(localtime) == 0)
  {
    printf("时间格式不正确！请使用例如：yyyymmddhh24miss \n");
    return -1;
  }
  STRNCPY(finaltime, sizeof(finaltime), localtime, strlen(localtime) - strlen(argv[2]) - 2);
  strcat(finaltime, argv[2]);
  strcat(finaltime, "00");

  time_t filetime = strtotime(finaltime);

  //打开目标文件夹
  int total = 0;
  
  if (argc == 3)
  {
    if ((Dir.OpenDir(argv[1], "*", 1000, true, false) == false))
    {
      printf("打开文件目录 %s 失败!\n", argv[1]);
      return -1;
    }

    while (true)
    {
      if (Dir.ReadDir() == false)
        break;

      if (strtotime(Dir.m_CreateTime) <= filetime)
      {
        if (REMOVE(Dir.m_FullFileName) == false)
        {
          printf("删除文件 %s 失败！\n", Dir.m_FileName);
          continue;
        }
        printf("删除文件 %s 成功！\n", Dir.m_FileName);
        total++;
      }
    }
  }
  else if (argc == 4)
  {
    if ((Dir.OpenDir(argv[1], argv[3], 1000, true, false) == false))
    {
      printf("打开文件目录 %s 失败!\n", argv[1]);
      return -1;
    }

    while (true)
    {
      if (Dir.ReadDir() == false)
        break;

      if (atoi(Dir.m_CreateTime) <= filetime)
      {
        if (REMOVE(Dir.m_FullFileName) == false)
        {
          printf("删除文件 %s 失败！\n", Dir.m_FileName);
          continue;
        }
        printf("删除文件 %s 成功！\n", Dir.m_FileName);
        total++;
      }
    }
  }

  printf("%d个文件删除完成!\n", total);
  return 0;
}
