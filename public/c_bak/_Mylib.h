#ifndef  _MYLIB_H
#define  _MYLIB_H 1

#include "_cmpublic.h"
#include "_freecplus.h"
#include "_ooci.h"
class CQuickBind
{
public:
  //�������������
  CQuickBind();
  CQuickBind(char* tablename,connection &fconn);
  CQuickBind(char* tablename,char* connpar,char* connenvpar="Simplified Chinese_China.ZHS16GBK",int autocommitopt=0);
 ~CQuickBind();

//ʵ����Ҫ�õ�����
  class sqlstatement stmtinsert;  
  class sqlstatement stmtquery;
  class sqlstatement stmtupdate;
  class connection conn;

  string tablenamestring;  //������ʱ��ű�����string���
  string insertstring;     //�������insert����string���
  string updatestring;     //�������update����string���
  string querystring;      //�������query����string���

  //����ṹ���������char���͵ĵ�ַ���ַ�������
  struct m_charstore
  {
    char*        charaddress;
    unsigned int stringlen;
  }stm_charstore;

  //����ṹ��������Ű�Ԫ�صı�����������������
  struct m_eleinfo
  {
    unsigned int tmark;       //ָ���󶨵�Ԫ�ص�����type
    unsigned int pmark;       //ָ���󶨵�Ԫ�ص�λ��position
    string       cloumnname;  //ָ���󶨵�Ԫ�ص�����Ӧ������
  }stm_eleinfo;

  //����һ���ṹ����������������е�Ԫ�صĴ洢��Ϣ
  vector<struct m_eleinfo> m_vstentire;
  
  //����һ���ṹ����������������е������Ĵ洢��Ϣ
  vector<struct m_eleinfo> m_vstkey;

  //����һ���ṹ����������������еķ������Ĵ洢��Ϣ
  vector<struct m_eleinfo> m_vstnkey;

  //��������������ű����ĵ�ַ
  vector<int*> m_vintaddress;                 //�������int���͵���Ϣ
  vector<double*> m_vdoubleaddress;           //�������double���͵���Ϣ
  vector<struct m_charstore> m_vcharststore;  //�������char���͵���Ϣ
  vector<string> m_vdatestring;               //����������ڸ�ʽ��string�����Ϣ
  vector<string> m_vconststring;              //������Ź̶����ݵ�string�����Ϣ����sysdate����

//�����ڲ��Ĳ���
  unsigned int keymark;           //�ж��Ƿ�������ͻ�Ĺؼ���

//����ͳ�ƵĲ���
  unsigned int inserttotalcount;  //���������
  unsigned int updatetotalcount;  //���µ�����
  unsigned int failedtotalcount;  //ʧ�ܵ�����

  //��������������Ͽ����Աclass connection conn�����ӵ�
  void Disconnect();

  //�����ʼ��
  void Init();

  //connect������������ʵ�ַ�ʽ����Ӧ���ֹ��캯��
  bool Connect(char* connpar,char* connenvpar,int autocommitopt);
  bool Connect(connection& fconn);

  //�󶨱����ĺ��� ��ֻ��������õļ���
  bool Bind(int&        par,const char* columnname,bool keyid=false);
  bool Bind(double&     par,const char* columnname,bool keyid=false);
  bool Bind(const char* par,const char* columnname,unsigned int len=0,bool keyid=false);
  bool Bind(const char* par,const char* columnname,const char* datetype,bool keyid=false,unsigned int len=32);
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

#endif
