#ifndef _MYLIB_H
#define _MYLIB_H 1

#include "_freecplus.h"
#include "_ooci.h"

class CQuickBind
{
public:
  //�������������
  CQuickBind();
  CQuickBind(char *tablename, connection &fconn);
  CQuickBind(char *tablename, char *connpar, char *connenvpar = "Simplified Chinese_China.ZHS16GBK", int autocommitopt = 0);
  ~CQuickBind();

  //ʵ����Ҫ�õ�����
  class sqlstatement stmtinsert;
  class sqlstatement stmtquery;
  class sqlstatement stmtupdate;
  class connection conn;

  string tablenamestring; //������ʱ��ű�����string���
  string insertstring;    //�������insert����string���
  string updatestring;    //�������update����string���
  string querystring;     //�������query����string���

  //����ṹ���������char���͵ĵ�ַ���ַ�������
  struct m_charstore
  {
    char *charaddress;
    unsigned int stringlen;
  } stm_charstore;

  //����ṹ��������Ű�Ԫ�صı�����������������
  struct m_eleinfo
  {
    unsigned int tmark; //ָ���󶨵�Ԫ�ص�����type
    unsigned int pmark; //ָ���󶨵�Ԫ�ص�λ��position
    string cloumnname;  //ָ���󶨵�Ԫ�ص�����Ӧ������
  } stm_eleinfo;

  //����һ���ṹ����������������е�Ԫ�صĴ洢��Ϣ
  vector<struct m_eleinfo> m_vstentire;

  //����һ���ṹ����������������е������Ĵ洢��Ϣ
  vector<struct m_eleinfo> m_vstkey;

  //����һ���ṹ����������������еķ������Ĵ洢��Ϣ
  vector<struct m_eleinfo> m_vstnkey;

  //��������������ű����ĵ�ַ
  vector<int *> m_vintaddress;               //�������int���͵���Ϣ
  vector<double *> m_vdoubleaddress;         //�������double���͵���Ϣ
  vector<struct m_charstore> m_vcharststore; //�������char���͵���Ϣ
  vector<string> m_vdatestring;              //����������ڸ�ʽ��string�����Ϣ
  vector<string> m_vconststring;             //������Ź̶����ݵ�string�����Ϣ����sysdate����

  //�����ڲ��Ĳ���
  unsigned int keymark; //�ж��Ƿ�������ͻ�Ĺؼ���

  //����ͳ�ƵĲ���
  unsigned int inserttotalcount; //���������
  unsigned int updatetotalcount; //���µ�����
  unsigned int failedtotalcount; //ʧ�ܵ�����

  //��������������Ͽ����Աclass connection conn�����ӵ�
  void Disconnect();

  //�����ʼ��
  void Init();

  //connect������������ʵ�ַ�ʽ����Ӧ���ֹ��캯��
  bool Connect(char *connpar, char *connenvpar, int autocommitopt);
  bool Connect(connection &fconn);

  //�󶨱����ĺ��� ��������õļ���
  bool Bind(int &par, const char *columnname, bool keyid = false);
  bool Bind(double &par, const char *columnname, bool keyid = false);
  bool Bind(const char *par, const char *columnname, unsigned int len = 0, bool keyid = false);
  bool Bind(const char *par, const char *columnname, const char *datetype, bool keyid = false, unsigned int len = 32);
  //bool Bind(string&     par,const char* columnname,bool keyid=false);  //���ݿ����Ҫһ���̶��ĵ�ַ��string���޷�����
  //bool Bind(long          *value,bool keyid=false);
  //bool Bind(unsigned int  *value,bool keyid=false);
  //bool Bind(unsigned long *value,bool keyid=false);
  //bool Bind(float         *value,bool keyid=false);

  //׼��prepare���ĺ���
  bool Prepare();

  //�󶨱�����prepare���
  bool Bindin();

  //Ĭ�ϵĵ�ִ����䣬�Զ�����������ͻ�Ĳ���
  bool Execute();

  //Insert�ĵ���ִ�����
  bool InsertExecute();

  //query�ĵ���ִ����䣬����ֵΪ�������ļ�¼������ʧ�ܷ���-1
  int QueryExecute();

  //update�ĵ���ִ�����
  bool UpdateExecute();

  //��������������ز���ɹ�������
  int Insertcount();

  //��������������ظ��³ɹ�������
  int Updatecount();

  //���������������ʧ�ܵ�����
  int Failedcount();

  //���������������ʱ����͵���ã�
  void Error();

  //���������������ʱͳ�ƣ�͵���ã�
  void Report();
};

//��������������ǰ��ļ������ݿ���ȡ��
//����һ�¸�������������
//connection *conn �����Ӻõ��� CLogFile *logfile��׼���õ���־�� char *tname ׼��ȡ���ļ��ı��� char *colname׼��ȡ���ļ�������
//char* condition  ׼��ȡ���ļ���������䣬�����ǵü�''����Ȼ�ᱨ�� char* localname ׼�����������ļ�·��
int TableToFile(connection *conn, char *tname, char *colname, char *condition, char *localname, CLogFile *logfile = 0);

//������
//int FileToTable(connection *conn,char* tname,char* colname,char* condition,char* filename,CLogFile *logfile=0);

//�Ѽ���Freecplus���
//class CIncc
//{
//public:
//  CIncc(){}
//  CIncc(char* FileName,char* DirName,char* DirMatch,const unsigned int in_MaxCount=10000,const bool bAndChild=false,bool bSort=false);
//
//  struct st_fileinfo stfileinfo;//�洢�ļ���Ϣ�Ľṹ��
//  int Mode;                     //ѡ��ģʽ
//  char sFileName[301];             //���xml�ļ���string��
//  char sDirName[301];
//  char sDirMatch[301];
//  unsigned int MaxCount;
//  bool AndChild;
//  bool Sort;
//
//  int m_pos;                   //�����vlistfile1�ļ���ȡ����λ��
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
//                     /*��Ҫ��������*/
//  //1:��okfilename�ļ����ѳɹ���������ݼ��ص�����vokfilename��
//  //2:����Ҫ�ȶԵ��ļ����ڵ��ļ���Ϣ���ص�vlistfile������
//  //3:���жԱȣ���������������Ϣ��ȫһ�µ��ļ����ص�vlistfile1�����У���������������Ϣ���ݲ���ȫһ�»���û�еļ��ص�vokfilename1�С�
//
//  //�ܽ�һ����ʱ���������ڵ��ļ���Ϣ
//  //m_vEnTireProcessedList:�Ѿ��ɹ�ƥ��Ĳ��ҿ���д���ļ�����ʷ�ļ���Ϣ��
//  //m_vDirList:���ļ������ƥ����ɵ��ļ���Ϣ.
//  //m_vWaitProcessList:�Ѿ��ı���ļ���Ϣ����Ҫ���´���ĵ��ļ���Ϣ��
//  //m_vPartialProcessedList:�Ѿ�ƥ��ɹ����Ҳ���Ҫ�ٴδ�����ļ���Ϣ��
//
//  bool LoadInitFile();   //���س�ʼ���ļ��б���Ϣ
//  bool LoadDir();        //���ص�ǰĿ��Ŀ¼�ļ���Ϣ�б�
//  bool CompVector();     //�Ƚ���������
//  bool ReadList();       //��ȡ��Ҫ�ı��б���ļ�����Ϣ
//  bool WriteToFile();    //���ļ���Ϣд���ļ�
//
//  bool AppendToFile(char* inFileName,char* inModityTime);   //���ļ���׷�Ӽ�¼
//
//  //��ȡm_vWaitProcessList�����ڵ����ݣ�falseΪ��ȡ���,��CDir���е�ReadDir()����ʹ�÷�����ͬ
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
//  //��ӵ�������
//  void Add_Item(char *, char *);
//  void Add_Item(char *, bool);
//  void Add_Item(char *, int);
//  void Add_Item(char *, double);
//  void Add_Item(char *); //��������һ��null��
//
//  //�������
//  void Add_Item(char *, const char *[], int);
//  void Add_Item(char *, int[], int);
//  void Add_Item(char *, double[], int);
//
//  //���һ����װ��ɵ�����
//  void Add_Item(char *, class CJson_Array &);
//  void Add_Item(char *, class CJson_Object &);
//
//  //��Ҳ��һ�������õ��ķ�������Ϊcjson����ɹ�ϵ��������໥����������
//  //������������������罫ǰ�����ɹ�ϵ�ı�
//  void Remove_Item(char *);
//  void Add_Item(char *, cJSON *); //����һ�����ѱ��õ��ķ�������Ϊ��ȫ����������ĺ���������
//
//  int ItemCount(); //��������
//
//  char *Printf(); //����ʽ�����
//  char *Print();  //������ʽ�����
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
//  //���һ����
//  void Add_Item(); //��������һ��null��
//  void Add_Item(char *);
//  void Add_Item(double);
//  void Add_Item(bool);
//  void Add_Item(int);
//
//  //���һ������
//  void Add_Item(const char *[], int);
//  void Add_Item(int[], int);
//  void Add_Item(double[], int);
//
//  //���һ����װ��ɵ�����
//  void Add_Item(class CJson_Array &);
//  void Add_Item(class CJson_Object &);
//};

#endif
