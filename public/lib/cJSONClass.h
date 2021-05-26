#include"cJSON.h"
#include"_cmpublic.h"

class CJson_Object
{
public:
  CJson_Object();
  ~CJson_Object();

  cJSON *m_Json;

  //添加单独的项
  void Add_Item(char *, char *);
  void Add_Item(char *, bool);
  void Add_Item(char *, int);
  void Add_Item(char *, double);
  void Add_Item(char *); //这个是添加一个null项

  //添加数组
  void Add_Item(char *, const char *[], int);
  void Add_Item(char *, int[], int);
  void Add_Item(char *, double[], int);

  //添加一个组装完成的子类
  void Add_Item(char *, class CJson_Array &);
  void Add_Item(char *, class CJson_Object &);

  //这也是一个难以用到的方法，因为cjson的组成关系是由类的相互包含决定的
  //与其用这个函数，不如将前面的组成关系改变
  void Remove_Item(char *);
  void Add_Item(char *, cJSON *); //这是一个很难被用到的方法，因为完全可以用下面的函数来代替

  int ItemCount(); //返回项数

  char *Printf(); //带格式的输出
  char *Print();  //不带格式的输出
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

  //添加一个项
  void Add_Item(); //这个是添加一个null项
  void Add_Item(char *);
  void Add_Item(double);
  void Add_Item(bool);
  void Add_Item(int);

  //添加一个数组
  void Add_Item(const char *[], int);
  void Add_Item(int[], int);
  void Add_Item(double[], int);

  //添加一个组装完成的子类
  void Add_Item(class CJson_Array &);
  void Add_Item(class CJson_Object &);
};