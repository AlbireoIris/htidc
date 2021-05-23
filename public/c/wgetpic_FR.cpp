#include"_freecplus.h"

int main()
{
  char strbuffer[1024];
  for(int acc=0;acc<=9;acc++)
  {
    memset(strbuffer,0,sizeof(strbuffer));
    snprintf(strbuffer,1023,"wget http://pi.weather.com.cn/i/product/share/pic/l/PWCP_TWC_WEAP_SFER_ER1_TWC_L88_P9_202105080%d0000000.JPG",acc);
    system(strbuffer);
  }

  for(int acc=10;acc<=24;acc++)
  {
    memset(strbuffer,0,sizeof(strbuffer));
    snprintf(strbuffer,1023,"wget http://pi.weather.com.cn/i/product/share/pic/l/PWCP_TWC_WEAP_SFER_ER1_TWC_L88_P9_20210508%d0000000.JPG",acc);
    system(strbuffer);
  }

  return 0;
}


