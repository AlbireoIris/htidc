#include"_freecplus.h"

CTcpClient TcpClient;

char strSendBuffer[256];
char strRecvBuffer[256];

//bool biz1000();

bool biz1001();

int main(int argc, char const *argv[])
{
    if(TcpClient.ConnectToServer("119.29.18.109",5052) == false)
    {
        printf("Connect To Server Failed!\n");
        return -1;
    }
    
    //if(biz1000() == false)
    //  return false;

    CTimer Timer;
    if(biz1001() == false)
      return 0;
    
    printf("biz1001=%lf\n",Timer.Elapsed());
    
    
    

    sleep(1);

    return 0;
}


bool biz1001()
{
  memset(strSendBuffer,0,sizeof(strSendBuffer));
  memset(strRecvBuffer,0,sizeof(strRecvBuffer));

  strcpy(strSendBuffer,"<bizid>10001</bizid><userid>52:54:00:83:0f:c1</userid><ttytype>1</ttytype><lat>20.234518</lat><lon>115.90832</lon><height>150.5</height>");

  printf("send=%s=\n",strSendBuffer);
  if (TcpClient.Write(strSendBuffer)==false) { printf("send failed.\n"); return false; }

  // 接收全部的站点信息
  while (1)
  {
    memset(strRecvBuffer,0,sizeof(strRecvBuffer));
    if (TcpClient.Read(strRecvBuffer,20)==false)  { printf("recv failed.\n"); return false; }
    // printf("recv=%s=\n",strRecvBuffer);

    if (strcmp(strRecvBuffer,"ok")==0) break;
  }

  return true;
}









