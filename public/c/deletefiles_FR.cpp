#include "_freecplus.h"

CDir Dir;

int main(int argc, char *argv[])
{
   if ((argc != 3) && (argc != 4))
  {
    printf("\n");
    printf("����������������ʷ�ļ�\n");
    printf("ʹ�ø�ʽ��/htidc/shqx/bin/deletefiles  Ŀ���ļ���  Ŀ������ʱ��  ƥ�����ѡ�\n");
    printf("example : /htidc/shqx/bin/deletefiles /data/shqx/ftp/surfdata 20210409000000\n");
    printf("Using:/htidc/shqx/bin/deletefiles pathname dayout [matchstr]\n\n");
    return -1;
  }

  //�������ʱ��
  char localtime[32];
  char finaltime[32];
  memset(localtime, 0, sizeof(localtime));
  LocalTime(localtime, "yyyymmddhh24miss");
  if (strlen(localtime) == 0)
  {
    printf("ʱ���ʽ����ȷ����ʹ�����磺yyyymmddhh24miss \n");
    return -1;
  }
  STRNCPY(finaltime, sizeof(finaltime), localtime, strlen(localtime) - strlen(argv[2]) - 2);
  strcat(finaltime, argv[2]);
  strcat(finaltime, "00");

  time_t filetime = strtotime(finaltime);

  //��Ŀ���ļ���
  int total = 0;
  
  if (argc == 3)
  {
    if ((Dir.OpenDir(argv[1], "*", 1000, true, false) == false))
    {
      printf("���ļ�Ŀ¼ %s ʧ��!\n", argv[1]);
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
          printf("ɾ���ļ� %s ʧ�ܣ�\n", Dir.m_FileName);
          continue;
        }
        printf("ɾ���ļ� %s �ɹ���\n", Dir.m_FileName);
        total++;
      }
    }
  }
  else if (argc == 4)
  {
    if ((Dir.OpenDir(argv[1], argv[3], 1000, true, false) == false))
    {
      printf("���ļ�Ŀ¼ %s ʧ��!\n", argv[1]);
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
          printf("ɾ���ļ� %s ʧ�ܣ�\n", Dir.m_FileName);
          continue;
        }
        printf("ɾ���ļ� %s �ɹ���\n", Dir.m_FileName);
        total++;
      }
    }
  }

  printf("%d���ļ�ɾ�����!\n", total);
  return 0;
}
