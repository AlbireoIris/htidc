#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

int main(int argc, char *argv[])
{
  char strfile[300][32];
  char strpwd[301];

  char filetemp[4096];

  memset(filetemp, 0, sizeof(filetemp));
  memset(strpwd, 0, sizeof(strpwd));
  memset(strfile, 0, sizeof(strfile));

  getcwd(strpwd, 300);

  char pwdfilenamestore[256];
  memset(pwdfilenamestore, 0, sizeof(pwdfilenamestore));
  sprintf(pwdfilenamestore, "%s/filenamestore.txt", strpwd);

  FILE *temp = 0;
  temp = fopen(pwdfilenamestore, "a");
  fclose(temp);

  FILE *fps = 0;
  if ((fps = fopen(pwdfilenamestore, "r")) == 0)
  {
    printf("open the filenamestore failed!\n");
    return -1;
  }

  char buffer[50];
  while (1)
  {
    memset(buffer, 0, sizeof(buffer));
    if (fgets(buffer, 50, fps) == 0)
      break;
    strcat(buffer, "$%$");
    strcat(filetemp, buffer);
  }

  fclose(fps);

  FILE *fpsr = 0;
  if ((fpsr = fopen(pwdfilenamestore, "w")) == 0)
  {
    printf("open the filenamestore.txt failed!\n");
    return -1;
  }

  DIR *dir;

  if ((dir = opendir(strpwd)) == 0)
  {
    printf("open the dir failed!\n");
    return -1;
  }

  struct dirent *stdinfo;
  int itotalcount = 0;

  while (1)
  {
    if ((stdinfo = readdir(dir)) == 0)
      break;

    if ((stdinfo->d_type == 8) && (strcmp(stdinfo->d_name + strlen(stdinfo->d_name) - 4, ".cpp") == 0) && (strncmp(stdinfo->d_name, "_", 1) != 0))
    {
      strncpy(strfile[itotalcount], stdinfo->d_name, strlen(stdinfo->d_name) - 4);
      if (strstr(filetemp, strfile[itotalcount]) == NULL)
        printf("%s 追加完成！ \n", strfile[itotalcount]);
      fprintf(fpsr, "%s\n", strfile[itotalcount]);
      itotalcount++;
    }
  }

  fclose(fpsr);
  closedir(dir);

  FILE *fp = 0;

  if ((fp = fopen("makefile", "w")) == 0)
  {
    printf("open the fie failed!\n");
    return -1;
  }

  int acc = 0;

  fprintf(fp, "all:");

  for (acc = 0; acc < itotalcount; acc++)
    fprintf(fp, "%s ", strfile[acc]);

  fprintf(fp, "\n\n");

  //以下是通用的文件
  fprintf(fp, "# 警告处理\n");
  fprintf(fp, "CFLAGS = -Wno-write-strings -g#-Wno-unused-variable\n\n");

  //这个暂时用不到
  fprintf(fp, "# CFLAGS = -O2\n");
  fprintf(fp, "#CFLAGS = -O2 -Wall\n\n");

  fprintf(fp, "#库文件目录路径\n");
  fprintf(fp, "PUBLICHOME = /htidc/public/lib\n\n");

  //新目录下需要改动
  fprintf(fp, "#可执行程序目录路径\n");
  fprintf(fp, "BINHOME = /htidc/shqx/bin\n\n");

  //新目录下需要改动
  fprintf(fp, "#备份目录路径\n");
  fprintf(fp, "CBAK = /htidc/shqx/c_bak\n\n");

  fprintf(fp, "#libfreecplus.so的引用\n");
  fprintf(fp, "FREECPLUSLIB = -l_freecplus -lcJSON \n\n");

  fprintf(fp, "#通用库ftp路径\n");
  fprintf(fp, "FTPLIB = -l_ftp\n\n");

  //以下是oracle库文件的部分
  fprintf(fp, "# oracle头文件路径\n");
  fprintf(fp, "ORACLEHEAD = -I$(ORACLE_HOME)/rdbms/public\n\n");

  fprintf(fp, "# oracle库文件路径\n");
  fprintf(fp, "ORACLELIB =  -L$(ORACLE_HOME)/lib -L.\n\n");

  fprintf(fp, "# oracle的OCI库\n");
  fprintf(fp, "OCILIB = -lclntsh\n\n");

  fprintf(fp, "#_occi.h和_mylib.h的路径\n");
  fprintf(fp, "OCICPP = $(PUBLICHOME)/_ooci.cpp $(PUBLICHOME)/_mylib.cpp\n\n");

  //框架整合
  fprintf(fp, "# _freecplus框架整合\n");
  fprintf(fp, "IFREECPLUS = $(FREECPLUSLIB)\n\n");

  fprintf(fp, "#_ftp框架整合\n");
  fprintf(fp, "IFTP = $(FTPLIB)\n\n");

  fprintf(fp, "#occi框架整合\n");
  fprintf(fp, "IOCCI = $(ORACLELIB) $(ORACLEHEAD) $(OCILIB) $(OCICPP)\n\n");

  for (acc = 0; acc < itotalcount; acc++)
  {
    if (strncmp(strfile[acc], "_", 1) == 0)
      continue;

    fprintf(fp, "%s:%s.cpp\n", strfile[acc], strfile[acc]);

    char matchfile[256];
    memset(matchfile, 0, sizeof(matchfile));
    sprintf(matchfile, "\tg++ $(CFLAGS) -o %s %s.cpp -lm -lc", strfile[acc], strfile[acc]);

    //指定库文件目录
    strcat(matchfile, " -I$(PUBLICHOME) -L$(PUBLICHOME) ");

    //添加freecplus依赖
    if (strstr(strfile[acc], "_FR") != NULL)
      strcat(matchfile, "$(IFREECPLUS)");

    //添加ooci依赖
    if (strstr(strfile[acc], "_OR") != NULL)
      strcat(matchfile, "$(IOCCI)");

    //添加ftp依赖
    if (strstr(strfile[acc], "_FT") != NULL)
      strcat(matchfile, "$(IFTP)");

    //添加多线程函数库的依赖
    if (strstr(strfile[acc], "_MP") != NULL)
      strcat(matchfile, " -lpthread ");

    fprintf(fp, "%s\n", matchfile);
    fprintf(fp, "\tcp %s $(BINHOME)/%s \n", strfile[acc], strfile[acc]);
    fprintf(fp, "\tcp %s.cpp $(CBAK)/%s.cpp \n\n", strfile[acc], strfile[acc]);
  }

  fprintf(fp, "clean:\n\trm -rf ");
  for (acc = 0; acc < itotalcount; acc++)
  {
    if (strcmp(strfile[acc], "a") == 0)
      continue;
      
    fprintf(fp, "%s ", strfile[acc]);
  }

  fprintf(fp, "\n");

  fclose(fp);

  return 0;
}
