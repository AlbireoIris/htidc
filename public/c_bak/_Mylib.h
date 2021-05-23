#ifndef  _MYLIB_H
#define  _MYLIB_H 1

#include "_cmpublic.h"
#include "_freecplus.h"
#include "_ooci.h"
class CQuickBind
{
public:
  //构造和析构函数
  CQuickBind();
  CQuickBind(char* tablename,connection &fconn);
  CQuickBind(char* tablename,char* connpar,char* connenvpar="Simplified Chinese_China.ZHS16GBK",int autocommitopt=0);
 ~CQuickBind();

//实现需要用到的类
  class sqlstatement stmtinsert;  
  class sqlstatement stmtquery;
  class sqlstatement stmtupdate;
  class connection conn;

  string tablenamestring;  //用来临时存放表名的string语句
  string insertstring;     //用来存放insert语句的string语句
  string updatestring;     //用来存放update语句的string语句
  string querystring;      //用来存放query语句的string语句

  //这个结构体用来存放char类型的地址和字符串长度
  struct m_charstore
  {
    char*        charaddress;
    unsigned int stringlen;
  }stm_charstore;

  //这个结构体用来存放绑定元素的变量所在容器和列名
  struct m_eleinfo
  {
    unsigned int tmark;       //指明绑定的元素的类型type
    unsigned int pmark;       //指明绑定的元素的位置position
    string       cloumnname;  //指明绑定的元素的所对应的列名
  }stm_eleinfo;

  //定义一个结构体容器用来存放所有的元素的存储信息
  vector<struct m_eleinfo> m_vstentire;
  
  //定义一个结构体容器用来存放所有的主键的存储信息
  vector<struct m_eleinfo> m_vstkey;

  //定义一个结构体容器用来存放所有的非主键的存储信息
  vector<struct m_eleinfo> m_vstnkey;

  //下列容器用来存放变量的地址
  vector<int*> m_vintaddress;                 //用来存放int类型的信息
  vector<double*> m_vdoubleaddress;           //用来存放double类型的信息
  vector<struct m_charstore> m_vcharststore;  //用来存放char类型的信息
  vector<string> m_vdatestring;               //用来存放日期格式的string类的信息
  vector<string> m_vconststring;              //用来存放固定内容的string类的信息例如sysdate这种

//函数内部的参数
  unsigned int keymark;           //判断是否主键冲突的关键字

//用来统计的参数
  unsigned int inserttotalcount;  //插入的总数
  unsigned int updatetotalcount;  //更新的总数
  unsigned int failedtotalcount;  //失败的总数

  //这个函数是用来断开类成员class connection conn的连接的
  void Disconnect();

  //将类初始化
  void Init();

  //connect函数，有两种实现方式，对应两种构造函数
  bool Connect(char* connpar,char* connenvpar,int autocommitopt);
  bool Connect(connection& fconn);

  //绑定变量的函数 我只重载了最常用的几个
  bool Bind(int&        par,const char* columnname,bool keyid=false);
  bool Bind(double&     par,const char* columnname,bool keyid=false);
  bool Bind(const char* par,const char* columnname,unsigned int len=0,bool keyid=false);
  bool Bind(const char* par,const char* columnname,const char* datetype,bool keyid=false,unsigned int len=32);
//bool Bind(string&     par,const char* columnname,bool keyid=false);  //数据库绑定需要一个固定的地址，string类无法满足
//bool Bind(long          *value,bool keyid=false);
//bool Bind(unsigned int  *value,bool keyid=false);
//bool Bind(unsigned long *value,bool keyid=false);
//bool Bind(float         *value,bool keyid=false);


  //准备prepare语句的函数
  bool Prepare();

  //绑定变量到prepare语句
  bool Bindin();
 
  //默认的的执行语句，自动更新主键冲突的部分
  bool Execute();

  //Insert的单独执行语句
  bool InsertExecute();

  //query的单独执行语句，返回值为搜索到的记录条数，失败返回-1
  int QueryExecute();

  //update的单独执行语句
  bool UpdateExecute(); 

  //这个函数用来返回插入成功的总数
  int Insertcount();

  //这个函数用来返回更新成功的总数
  int Updatecount();

  //这个函数用来返回失败的总数
  int Failedcount();

  //这个函数用来测试时报错（偷懒用）
  void Error();

  //这个函数用来测试时统计（偷懒用）
  void Report();
};

#endif
