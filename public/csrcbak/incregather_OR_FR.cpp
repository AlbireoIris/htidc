#include"_mylib.h"

CLogFile logfile;

void  EXIT(int sig); 

int main()
{
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);

  CIncc Incc("/data/shqx/picdata/picdata.xml","/data/shqx/picdata","*.JPG");

  while(true)
  {
    Incc.IncFind();
    printf("m_vWaitProcessList,size() = %d\n",Incc.m_vWaitProcessList.size());

    while(true)
    {
      if(Incc.ReadList() == false)
        break;

      printf("The FullFileName=%s\n",Incc.m_FullFileName);
    }
   
 //   printf("Find Finished!\n");

    sleep(5);
  }

  return 0;
}

void EXIT(int sig)
{ 
  logfile.Write("³ÌÐòÍË³ö sig=%d\n",sig);
  exit(0);
}
