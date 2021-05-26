#include"cJSON.h"
#include"_cmpublic.h"

class CJson_Object
{
public:
  CJson_Object();
  ~CJson_Object();

  cJSON *m_Json;

  //��ӵ�������
  void Add_Item(char *, char *);
  void Add_Item(char *, bool);
  void Add_Item(char *, int);
  void Add_Item(char *, double);
  void Add_Item(char *); //��������һ��null��

  //�������
  void Add_Item(char *, const char *[], int);
  void Add_Item(char *, int[], int);
  void Add_Item(char *, double[], int);

  //���һ����װ��ɵ�����
  void Add_Item(char *, class CJson_Array &);
  void Add_Item(char *, class CJson_Object &);

  //��Ҳ��һ�������õ��ķ�������Ϊcjson����ɹ�ϵ��������໥����������
  //������������������罫ǰ�����ɹ�ϵ�ı�
  void Remove_Item(char *);
  void Add_Item(char *, cJSON *); //����һ�����ѱ��õ��ķ�������Ϊ��ȫ����������ĺ���������

  int ItemCount(); //��������

  char *Printf(); //����ʽ�����
  char *Print();  //������ʽ�����
};

class CJson_Array
{
public:
  CJson_Array(/* args */);
  ~CJson_Array();

  cJSON *m_cJson;

  char *Print();
  char *Printf();

  int ItemCount();

  //���һ����
  void Add_Item(); //��������һ��null��
  void Add_Item(char *);
  void Add_Item(double);
  void Add_Item(bool);
  void Add_Item(int);

  //���һ������
  void Add_Item(const char *[], int);
  void Add_Item(int[], int);
  void Add_Item(double[], int);

  //���һ����װ��ɵ�����
  void Add_Item(class CJson_Array &);
  void Add_Item(class CJson_Object &);
};