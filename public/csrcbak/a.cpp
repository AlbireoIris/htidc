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
        printf("%s ׷����ɣ� \n", strfile[itotalcount]);
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

  fprintf(fp, "# oracleͷ�ļ�·��\n");
  fprintf(fp, "ORAINCL = -I$(ORACLE_HOME)/rdbms/public\n\n");

  fprintf(fp, "# oracle���ļ�·��\n");
  fprintf(fp, "ORALIB =  -L$(ORACLE_HOME)/lib \n\n");

  fprintf(fp, "# oracle��oci��\n");
  fprintf(fp, "ORALIBS = -lclntsh\n\n");

  fprintf(fp, "#CFLAGS = -O2\n");
  fprintf(fp, "#CFLAGS = -O2 -Wall\n\n");

  fprintf(fp, "#�ַ������洦��\n");
  fprintf(fp, "CFLAGS = -Wno-write-strings -g#-Wno-unused-variable\n\n");

  fprintf(fp, "#ͨ�ÿ���ļ���Ŀ¼\n");
  fprintf(fp, "PUBHOM = /htidc/public/lib\n\n");

  fprintf(fp, "#��ִ�г�����Ŀ¼\n");
  fprintf(fp, "EXEHOM = /htidc/public/bin\n\n");

  fprintf(fp, "#ͨ�ÿ��ͷ�ļ�·��\n");
  fprintf(fp, "PUBINCL=-I$(PUBHOM)\n\n");

  fprintf(fp, "#ͨ�ÿ�ܿ��ļ�·��\n");
  fprintf(fp, "LIBHOM=-L$(PUBHOM)\n\n");

  // û��Ҫ���Ѿ������˶�̬����
  // fprintf(fp,"#ͨ�ÿ��freecplus·��\n");
  // fprintf(fp,"PUBCPP = $(PUBHOM)/_freecplus.cpp\n\n");

  fprintf(fp, "#ͨ�ÿ�freecplus·��\n");
  fprintf(fp, "PUBLIB = -l_freecplus\n\n");

  fprintf(fp, "#ͨ�ÿ��ftp·��\n");
  fprintf(fp, "FTPCPP = $(PUBHOM)/_ftp.cpp\n\n");

  fprintf(fp, "#ͨ�ÿ�ftp·��\n");
  fprintf(fp, "FTPLIB = -lftp\n\n");

  fprintf(fp, "#ͨ�ÿ��OCI·��\n");
  fprintf(fp, "OCICPP = $(PUBHOM)/_ooci.cpp $(PUBHOM)/_mylib.cpp\n\n");

  //������µ�Ŀ¼��ʹ�üǵ�Ҫ������Ŀ¼�ı�һ��
  fprintf(fp, "#����Ŀ¼·��\n");
  fprintf(fp, "CBAK = /htidc/public/c_bak\n\n");

  for (acc = 0; acc < itotalcount; acc++)
  {
    if (strncmp(strfile[acc], "_", 1) == 0)
      continue;

    fprintf(fp, "%s:%s.cpp\n", strfile[acc], strfile[acc]);
    char matchfile[256];
    memset(matchfile, 0, sizeof(matchfile));
    sprintf(matchfile, "\tg++ $(CFLAGS) -o %s %s.cpp -lm -lc", strfile[acc], strfile[acc]);
    if (strstr(strfile[acc], "_FR") != NULL)
      strcat(matchfile, " $(PUBINCL) $(LIBHOM) $(PUBLIB)");
    if (strstr(strfile[acc], "_OR") != NULL)
      strcat(matchfile, " $(ORAINCL) $(ORALIB) $(ORALIBS) $(OCICPP)");
    if (strstr(strfile[acc], "_FT") != NULL)
      strcat(matchfile, " $(LIBHOM) $(FTPCPP) $(FTPLIB)");
    fprintf(fp, "%s\n", matchfile);
    fprintf(fp, "\tcp %s $(EXEHOM)/%s \n", strfile[acc], strfile[acc]);
    fprintf(fp, "\tcp %s.cpp $(CBAK)/%s.cpp \n\n", strfile[acc], strfile[acc]);
  }

  fprintf(fp, "clean:\n\trm -rf ");
  for (acc = 0; acc < itotalcount; acc++)
  {
    if (strcmp(strfile[acc], "a") == 0)
      continue;
    fprintf(fp, "%s ", strfile[acc]);
  }
  fprintf(fp, "%s ", "temp.cpp");
  fprintf(fp, "\n");

  fclose(fp);

  return 0;
}
