#ifndef _MYLIB_H
#define _MYLIB_H 1

#include "_freecplus.h"
#include "_ooci.h"

class CQuickBind
{
public:
  //构造和析构函数
  CQuickBind();
  CQuickBind(char *tablename, connection &fconn);
  CQuickBind(char *tablename, char *connpar, char *connenvpar = "Simplified Chinese_China.ZHS16GBK", int autocommitopt = 0);
  ~CQuickBind();

  //实现需要用到的类
  class sqlstatement stmtinsert;
  class sqlstatement stmtquery;
  class sqlstatement stmtupdate;
  class connection conn;

  string tablenamestring; //用来临时存放表名的string语句
  string insertstring;    //用来存放insert语句的string语句
  string updatestring;    //用来存放update语句的string语句
  string querystring;     //用来存放query语句的string语句

  //这个结构体用来存放char类型的地址和字符串长度
  struct m_charstore
  {
    char *charaddress;
    unsigned int stringlen;
  } stm_charstore;

  //这个结构体用来存放绑定元素的变量所在容器和列名
  struct m_eleinfo
  {
    unsigned int tmark; //指明绑定的元素的类型type
    unsigned int pmark; //指明绑定的元素的位置position
    string cloumnname;  //指明绑定的元素的所对应的列名
  } stm_eleinfo;

  //定义一个结构体容器用来存放所有的元素的存储信息
  vector<struct m_eleinfo> m_vstentire;

  //定义一个结构体容器用来存放所有的主键的存储信息
  vector<struct m_eleinfo> m_vstkey;

  //定义一个结构体容器用来存放所有的非主键的存储信息
  vector<struct m_eleinfo> m_vstnkey;

  //下列容器用来存放变量的地址
  vector<int *> m_vintaddress;               //用来存放int类型的信息
  vector<double *> m_vdoubleaddress;         //用来存放double类型的信息
  vector<struct m_charstore> m_vcharststore; //用来存放char类型的信息
  vector<string> m_vdatestring;              //用来存放日期格式的string类的信息
  vector<string> m_vconststring;             //用来存放固定内容的string类的信息例如sysdate这种

  //函数内部的参数
  unsigned int keymark; //判断是否主键冲突的关键字

  //用来统计的参数
  unsigned int inserttotalcount; //插入的总数
  unsigned int updatetotalcount; //更新的总数
  unsigned int failedtotalcount; //失败的总数

  //这个函数是用来断开类成员class connection conn的连接的
  void Disconnect();

  //将类初始化
  void Init();

  //connect函数，有两种实现方式，对应两种构造函数
  bool Connect(char *connpar, char *connenvpar, int autocommitopt);
  bool Connect(connection &fconn);

  //绑定变量的函数 重载了最常用的几个
  bool Bind(int &par, const char *columnname, bool keyid = false);
  bool Bind(double &par, const char *columnname, bool keyid = false);
  bool Bind(const char *par, const char *columnname, unsigned int len = 0, bool keyid = false);
  bool Bind(const char *par, const char *columnname, const char *datetype, bool keyid = false, unsigned int len = 32);
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

//这个函数的作用是把文件从数据库中取出
//解释一下各个参数的作用
//connection *conn 已连接好的类 CLogFile *logfile已准备好的日志类 char *tname 准备取出文件的表名 char *colname准备取出文件的类名
//char* condition  准备取出文件的条件语句，参数记得加''，不然会报错。 char* localname 准备重命名的文件路径
int TableToFile(connection *conn, char *tname, char *colname, char *condition, char *localname, CLogFile *logfile = 0);

//已弃用
//int FileToTable(connection *conn,char* tname,char* colname,char* condition,char* filename,CLogFile *logfile=0);

//已加入Freecplus框架
//class CIncc
//{
//public:
//  CIncc(){}
//  CIncc(char* FileName,char* DirName,char* DirMatch,const unsigned int in_MaxCount=10000,const bool bAndChild=false,bool bSort=false);
//
//  struct st_fileinfo stfileinfo;//存储文件信息的结构体
//  int Mode;                     //选择模式
//  char sFileName[301];             //存放xml文件的string类
//  char sDirName[301];
//  char sDirMatch[301];
//  unsigned int MaxCount;
//  bool AndChild;
//  bool Sort;
//
//  int m_pos;                   //存放了vlistfile1文件读取到的位置
//
//  char m_FullFileName[301];
//  char m_ModityTime[21];
//  int  m_FileSize;
//
//  vector<struct st_fileinfo> m_vEnTireProcessedList;
//  vector<struct st_fileinfo> m_vDirList;
//  vector<struct st_fileinfo> m_vWaitProcessList;
//  vector<struct st_fileinfo> m_vPartialProcessedList;
//
//  CDir  Dir;
//
//  bool InitCDir(char *in_DirName,const char *in_MatchStr,const unsigned int in_MaxCount=10000,const bool bAndChild=false,bool bSort=false);
//  bool InitCFile(char *FileName);
//  bool InitMode(int inMode);
//
//                     /*主要工作流程*/
//  //1:将okfilename文件里已成功传输的内容加载到容器vokfilename中
//  //2:将想要比对的文件夹内的文件信息加载到vlistfile容器中
//  //3:进行对比，将两个容器内信息完全一致的文件加载到vlistfile1容器中，将两个容器内信息内容不完全一致或者没有的加载到vokfilename1中。
//
//  //总结一下这时各个容器内的文件信息
//  //m_vEnTireProcessedList:已经成功匹配的并且可以写入文件的历史文件信息。
//  //m_vDirList:打开文件夹里的匹配完成的文件信息.
//  //m_vWaitProcessList:已经改变的文件信息既需要重新处理的的文件信息。
//  //m_vPartialProcessedList:已经匹配成功并且不需要再次处理的文件信息。
//
//  bool LoadInitFile();   //加载初始的文件列表信息
//  bool LoadDir();        //加载当前目标目录文件信息列表
//  bool CompVector();     //比较两个容器
//  bool ReadList();       //获取需要改变列表的文件的信息
//  bool WriteToFile();    //将文件信息写入文件
//
//  bool AppendToFile(char* inFileName,char* inModityTime);   //向文件中追加记录
//
//  //获取m_vWaitProcessList容器内的数据，false为读取完毕,和CDir类中的ReadDir()方法使用方法相同
//  bool IncFind();
//
// ~CIncc();
//};

//class CJson_Object
//{
//public:
//  CJson_Object();
//  ~CJson_Object();
//
//  cJSON *m_Json;
//
//  //添加单独的项
//  void Add_Item(char *, char *);
//  void Add_Item(char *, bool);
//  void Add_Item(char *, int);
//  void Add_Item(char *, double);
//  void Add_Item(char *); //这个是添加一个null项
//
//  //添加数组
//  void Add_Item(char *, const char *[], int);
//  void Add_Item(char *, int[], int);
//  void Add_Item(char *, double[], int);
//
//  //添加一个组装完成的子类
//  void Add_Item(char *, class CJson_Array &);
//  void Add_Item(char *, class CJson_Object &);
//
//  //这也是一个难以用到的方法，因为cjson的组成关系是由类的相互包含决定的
//  //与其用这个函数，不如将前面的组成关系改变
//  void Remove_Item(char *);
//  void Add_Item(char *, cJSON *); //这是一个很难被用到的方法，因为完全可以用下面的函数来代替
//
//  int ItemCount(); //返回项数
//
//  char *Printf(); //带格式的输出
//  char *Print();  //不带格式的输出
//};
//
//
//class CJson_Array
//{
//public:
//  CJson_Array(/* args */);
//  ~CJson_Array();
//
//  cJSON *m_cJson;
//
//  char *Print();
//  char *Printf();
//
//  int ItemCount();
//
//  //添加一个项
//  void Add_Item(); //这个是添加一个null项
//  void Add_Item(char *);
//  void Add_Item(double);
//  void Add_Item(bool);
//  void Add_Item(int);
//
//  //添加一个数组
//  void Add_Item(const char *[], int);
//  void Add_Item(int[], int);
//  void Add_Item(double[], int);
//
//  //添加一个组装完成的子类
//  void Add_Item(class CJson_Array &);
//  void Add_Item(class CJson_Object &);
//};

#endif
