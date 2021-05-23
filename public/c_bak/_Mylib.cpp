#include"_Mylib.h"

//第一种构造函数 啥也没有
CQuickBind::CQuickBind()
{
  Init();
}

//第二种构造函数有表名和已连接好的句柄
CQuickBind::CQuickBind(char* tablename,connection &fconn)
{
  Init();
  conn=fconn;  
  Connect(conn);
  tablenamestring=tablename;
}

//第三种构造函数有表名和连接参数
CQuickBind::CQuickBind(char* tablename,char* connpar,char* connenvpar,int autocommitopt)
{
  Init();
  Connect(connpar,connenvpar,autocommitopt);
  tablenamestring=tablename;
}

//我知道这是析构函数，这个注释的目的是为了保持队形统一
CQuickBind::~CQuickBind()
{
  Init();
  Disconnect();
}

//进行一些初始化工作
void CQuickBind::Init()
{
  tablenamestring.clear();
  insertstring.clear();
  updatestring.clear();
  querystring.clear();

  m_vintaddress.clear();
  m_vdoubleaddress.clear();
  m_vcharststore.clear();

  inserttotalcount=failedtotalcount=updatetotalcount=0;
}

//检查数据库连接状态，如果还是连接状态就提交并断开连接
//后期可以考虑添加一个回滚选项
void CQuickBind::Disconnect()
{
  if(conn.m_state != 0)
  {
    conn.commit();
    conn.disconnect();
  }
}

bool CQuickBind::Connect(char* connpar,char* connenvpar,int autocommitopt)
{
  //如果已有连接，就断开现有连接
  if(conn.m_state != 0)
    Disconnect();
 
  //判断连接结果
  if(conn.connecttodb(connpar,connenvpar,autocommitopt) != 0)
    return false;

  if(stmtinsert.connect(&conn) != 0) return false;
  if(stmtupdate.connect(&conn) != 0) return false;
  if(stmtquery.connect(&conn)  != 0) return false;

  return true; 
}

bool CQuickBind::Connect(connection &fconn)
{
  if(fconn.m_state != 1)
    return false;

  if(stmtinsert.connect(&fconn) != 0) return false;
  if(stmtupdate.connect(&fconn) != 0) return false;
  if(stmtquery.connect(&fconn)  != 0) return false;
  return true;
}

bool CQuickBind::Bind(int &par,const char* columnname,bool keyid)
{
  //对储存信息的结构体赋值
  stm_eleinfo.tmark = 0;
  stm_eleinfo.pmark = m_vintaddress.size();
  stm_eleinfo.cloumnname = columnname; 

  //把变量地址存入对应容器
  m_vintaddress.push_back(&par);

  //把他们分别放入全体容器中
  m_vstentire.push_back(stm_eleinfo);

  //如果keyid=true就把他放入主键容器中
  if(keyid == true)
    m_vstkey.push_back(stm_eleinfo);
  else
    m_vstnkey.push_back(stm_eleinfo);

  return true;
}

bool CQuickBind::Bind(double& par,const char* columnname,bool keyid)
{ 
  //对储存信息的结构体赋值
  stm_eleinfo.tmark = 1;
  stm_eleinfo.pmark = m_vdoubleaddress.size();
  stm_eleinfo.cloumnname = columnname;   

  //把变量地址存入对应容器
  m_vdoubleaddress.push_back(&par);

  //把他们分别放入全体容器中
  m_vstentire.push_back(stm_eleinfo);

  //如果keyid=true就把他放入主键容器中
  if(keyid == true)
    m_vstkey.push_back(stm_eleinfo);
  else
    m_vstnkey.push_back(stm_eleinfo);

  return true;
}

bool CQuickBind::Bind(const char* par,const char* columnname,unsigned int len,bool keyid)
{
  if(len != 0)
  {
    //存储字符串的地址和长度
    memset(&stm_charstore,0,sizeof(struct m_charstore));
    stm_charstore.charaddress=(char*)par;
    stm_charstore.stringlen=len;
  
    //对储存信息的结构体赋值
    stm_eleinfo.tmark = 2;
    stm_eleinfo.pmark = m_vcharststore.size();
    stm_eleinfo.cloumnname = columnname;
  
    //把结构体存入对应容器
    m_vcharststore.push_back(stm_charstore);
  
    //把他们分别放入全体容器中
    m_vstentire.push_back(stm_eleinfo);
  
    //如果keyid=true就把他放入主键容器中
    if(keyid == true)
      m_vstkey.push_back(stm_eleinfo);
    else
      m_vstnkey.push_back(stm_eleinfo);
  }
  else
  {
    //对储存信息的结构体赋值
    stm_eleinfo.tmark = 4;
    stm_eleinfo.pmark = m_vconststring.size();
    stm_eleinfo.cloumnname = columnname;
  
    //存储固定字符串的内容
    string tempstring = par;
    m_vconststring.push_back(tempstring);
  
    //把结构体存入对应容器
    m_vcharststore.push_back(stm_charstore);
  
    //把他们分别放入全体容器中
    m_vstentire.push_back(stm_eleinfo);
  
    //如果keyid=true就把他放入主键容器中
    if(keyid == true)
      m_vstkey.push_back(stm_eleinfo);
    else
      m_vstnkey.push_back(stm_eleinfo);   
  }
  return true;
}

//这个函数是用来绑定日期类型的
bool CQuickBind::Bind(const char* par,const char* columnname,const char* datetype,bool keyid,unsigned int len)
{ 
  //存储字符串的地址和长度
  memset(&stm_charstore,0,sizeof(struct m_charstore)); //初始化结构体
  stm_charstore.charaddress=(char*)par;                //存放参数地址
  stm_charstore.stringlen=len;                         //存放字符串长度，默认为32
  string tdatetype = datetype;                         //存放日期格式
  
  //对储存信息的结构体赋值
  stm_eleinfo.tmark = 3;
  stm_eleinfo.pmark = m_vcharststore.size();
  stm_eleinfo.cloumnname = columnname;
  
  //把结构体存入对应容器
  m_vcharststore.push_back(stm_charstore);
  
  //把日期格式存入m_vdatestring容器中
  m_vdatestring.push_back(tdatetype);

  //把他们分别放入全体容器中
  m_vstentire.push_back(stm_eleinfo);
  
  //如果keyid=true就把他放入主键容器中
  if(keyid == true)
    m_vstkey.push_back(stm_eleinfo);
  else
    m_vstnkey.push_back(stm_eleinfo);
  
  return true;
}

//这是失败的string类绑定尝试
//bool CQuickBind::Bind(string&  par,const char* columnname,bool keyid)
//{
////  memset(temp_string_str,0,sizeof(temp_string_str));
////  strcpy(temp_string_str,par.c_str());
//  
//
//  //存储字符串的地址和长度
//  memset(&stm_charstore,0,sizeof(struct m_charstore));
////  stm_charstore.charaddress=(char*)temp_string_str;
////  stm_charstore.stringlen=1024;
//  stm_charstore.charaddress=(char*)par.c_str();
//  stm_charstore.stringlen=par.length();
//
//  //对储存元素信息的结构体赋值
//  stm_eleinfo.tmark = 2;
//  stm_eleinfo.pmark = m_vstrststore.size();
//  stm_eleinfo.cloumnname = columnname;
//  
//  //把存储字符串信息的结构体存入容器
//  m_vstrststore.push_back(stm_charstore);
//  
//  //把储存元素信息的结构体放入全体容器中
//  m_vstentire.push_back(stm_eleinfo);
//  
//  //如果keyid=true就把他放入主键容器中否则放入非主键容器
//  if(keyid == true)
//    m_vstkey.push_back(stm_eleinfo);
//  else 
//    m_vstnkey.push_back(stm_eleinfo);
//   
//  return true;
//}


//这个函数是用来准备插入查询更新语句的
bool CQuickBind::Prepare()
{
  int acc;
  int count;
  char temp[16];   //用来暂存数字转字符串的
  int  datecount=0;
  
//这里是准备insert的prepare语句
  insertstring +="insert into ";
  insertstring +=tablenamestring;
  insertstring +=" (";
  
  //写入所有主键和非主键的列名
  for(acc=0,count=0;count<m_vstentire.size();acc++,count++)
  {
    insertstring+=m_vstentire[acc].cloumnname;

    //如果是固定类型就不要绑定了
    if(m_vstentire[acc].tmark == 4)
      acc--;

    if(count != m_vstentire.size()-1)
      insertstring+=",";  
  }

  insertstring+=") values(";

  //所有主键和非主键的总数
  for(acc=0,count=0;count<m_vstentire.size();acc++,count++)
  {
    //数字转字符串
    memset(temp,0,sizeof(temp));
    sprintf(temp,"%d",acc+1);

    switch(m_vstentire[acc].tmark)
    {
      case 0:{}   //从0-2都是一种处理方法
      case 1:{}
      case 2:
      {
        insertstring+=":";
        insertstring+=temp;
        break;
      }
      case 3:
      {
        insertstring +="to_date(:";
        insertstring +=temp;
        insertstring +=",'";
        insertstring +=m_vdatestring[datecount];
        insertstring +="')";
        break;
      }
      case 4:
      {
        insertstring+=m_vconststring[m_vstentire[acc].pmark];
        acc--;    
        break;
      }
      default:
        return false;
    }

    if(count != m_vstkey.size()+m_vstnkey.size()-1)
      insertstring+=",";
  }

  insertstring+=")";
  if(stmtinsert.prepare(insertstring.c_str()) != 0)  
    return false;

  //这里是准备query的prepare语句
  querystring +="select count(*) from ";
  querystring +=tablenamestring;
  querystring +=" where ";
  for(acc=0,count=0;count<m_vstkey.size();acc++,count++)
  {
    memset(temp,0,sizeof(temp));
    sprintf(temp,"%d",acc+1);

    switch(m_vstkey[acc].tmark)
    {
      case 0:{}
      case 1:{}
      case 2:
      {
        querystring +=m_vstkey[acc].cloumnname;
        querystring +="=:";
        querystring +=temp;
        break;
      }
      case 3:
      {
        querystring +=m_vstkey[acc].cloumnname;
        querystring +="=";
        querystring +="to_date(:";
        querystring +=temp;
        querystring +=",'";
        querystring +=m_vdatestring[datecount];
        querystring +="')";
        break;
      }
      case 4:
      {
        querystring+=m_vconststring[m_vstkey[acc].pmark];
        acc--;    
        break;  
      }
      default:
        return false;
    }

    if(count != m_vstkey.size()-1)
      querystring+=" and ";
  }
  if(stmtquery.prepare(querystring.c_str()) != 0)
    return false;
  
  //这里是准备update的prepare语句
  updatestring +="update ";
  updatestring +=tablenamestring;
  updatestring +=" set ";

  int posmark=1;
  for(acc=0,count=0;count<m_vstnkey.size();acc++,count++,posmark++)
  {
    memset(temp,0,sizeof(temp));
    sprintf(temp,"%d",posmark);

    switch(m_vstnkey[acc].tmark)
    {
      case 0:{}
      case 1:{}
      case 2:
      {
        updatestring +=m_vstnkey[acc].cloumnname;
        updatestring +="=:";
        updatestring +=temp;
        break;
      }
      case 3:
      {
        updatestring +=m_vstnkey[acc].cloumnname;
        updatestring +="=";
        updatestring +="to_date(:";
        updatestring +=temp;
        updatestring +=",'";
        updatestring +=m_vdatestring[datecount];
        updatestring +="')";
        break;
      }
      case 4:
      {
        updatestring +=m_vstnkey[acc].cloumnname;
        updatestring +="=";
        updatestring+=m_vconststring[m_vstnkey[acc].pmark];
        acc--;
        posmark--;  
        break;   
      }
      default:
        return false;
    }    

    if(count != m_vstnkey.size()-1)
      updatestring +=",";

    updatestring +=" ";
  }

  updatestring +=" where ";

  for(acc=0,count=0;count<m_vstkey.size();acc++,posmark++,count++)
  {
    memset(temp,0,sizeof(temp));
    sprintf(temp,"%d",posmark);

    switch(m_vstkey[acc].tmark)
    {
      case 0:{}
      case 1:{}
      case 2:
      {
        updatestring +=m_vstkey[acc].cloumnname;
        updatestring +="=:";
        updatestring +=temp;
        break;
      }
      case 3:
      {
        updatestring +=m_vstkey[acc].cloumnname;
        updatestring +="=";
        updatestring +="to_date(:";
        updatestring +=temp;
        updatestring +=",'";
        updatestring +=m_vdatestring[datecount];
        updatestring +="')";
        break;
      }
      case 4:
      {
        updatestring +=m_vstkey[acc].cloumnname;
        updatestring +="=";
        updatestring+=m_vconststring[m_vstkey[acc].pmark];
        acc--;
        posmark--;
        break;
      }
      default:
        return false;
    }
    
    if(count != m_vstkey.size()-1)
      updatestring +=" and ";
  }

  if(stmtupdate.prepare( updatestring.c_str() ) != 0)
    return false;

  return true;
}


bool CQuickBind::Bindin()
{
  int insertcount=1;   
  int querycount=1;
  int updatecount=1;

  //这一段是insert的绑定部分
  for(int acc=0;acc<m_vstentire.size();acc++,insertcount++)
  {
    switch(m_vstentire[acc].tmark) 
    {
      case 0:
      {
        int* tp=0;
        tp = m_vintaddress[m_vstentire[acc].pmark];
        if(stmtinsert.bindin(insertcount,tp) != 0)
          return false;
        break;
      }
      case 1:
      { 
        double* tp=0;
        tp = m_vdoubleaddress[m_vstentire[acc].pmark]; 
        if(stmtinsert.bindin(insertcount,tp) != 0)
          return false;
        break;
      } 
      case 2:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstentire[acc].pmark].charaddress;
        if(stmtinsert.bindin(insertcount,tp,m_vcharststore[m_vstentire[acc].pmark].stringlen) != 0)
          return false;
        break;
      }
      case 3:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstentire[acc].pmark].charaddress;
        if(stmtinsert.bindin(insertcount,tp,m_vcharststore[m_vstentire[acc].pmark].stringlen) != 0)
          return false; 
        break;
      }
      case 4:
      {
        insertcount--;
        break;
      }
      default:
        return false;
        break;
    }   
  }   

  //这一段是query的绑定部分
  for(int acc=0;acc<m_vstkey.size();acc++,querycount++)
  {
    switch(m_vstkey[acc].tmark)
    {
      case 0:
      {
        int* tp=0;
        tp = m_vintaddress[m_vstkey[acc].pmark];
        if(stmtquery.bindin(querycount,tp) != 0)
          return false;
        break;
      }
      case 1:
      {
        double* tp=0;
        tp = m_vdoubleaddress[m_vstkey[acc].pmark];
        if(stmtquery.bindin(querycount,tp) != 0)
          return false;
        break;
      }
      case 2:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
        if(stmtquery.bindin(querycount,tp,m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)
          return false;
        break;
      }
      case 3:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
        if(stmtquery.bindin(querycount,tp,m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)
          return false;
        break;
      }
      case 4:
      {
        querycount--;
        break;
      }
      default: 
        return false;
        break;
    }  
  }

  if(stmtquery.bindout(1,&keymark) != 0)
    return false;

  //这一段是update的绑定部分
  for(int acc=0;acc<m_vstnkey.size();acc++,updatecount++)
  { 
    switch(m_vstnkey[acc].tmark)
    {
      case 0:
      {
        int* tp=0;
        tp = m_vintaddress[m_vstnkey[acc].pmark];
        if(stmtupdate.bindin(updatecount,tp) != 0)
          return false;
        break;
      }
      case 1:
      {
        double* tp=0;
        tp = m_vdoubleaddress[m_vstnkey[acc].pmark];
        if(stmtupdate.bindin(updatecount,tp) != 0)
          return false;
        break;
      }
      case 2:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstnkey[acc].pmark].charaddress;
        if(stmtupdate.bindin(updatecount,tp,m_vcharststore[m_vstnkey[acc].pmark].stringlen) != 0)
          return false;
        break;
      }
      case 3:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstnkey[acc].pmark].charaddress;
        if(stmtupdate.bindin(updatecount,tp,m_vcharststore[m_vstnkey[acc].pmark].stringlen) != 0)
          return false;
        break; 
      }
      case 4:
      {
        updatecount--;
        break;
      }
      default: 
        return false;
        break;
    }  
  }

  for(int acc=0;acc<m_vstkey.size();acc++,updatecount++)
  { 
    switch(m_vstkey[acc].tmark)
    {
      case 0:
      {
        int* tp=0;
        tp = m_vintaddress[m_vstkey[acc].pmark];
        if(stmtupdate.bindin(updatecount,tp) != 0)
          return false;
        break;
      }
      case 1:
      {
        double* tp=0;
        tp = m_vdoubleaddress[m_vstkey[acc].pmark];
        if(stmtupdate.bindin(updatecount,tp) != 0)
          return false;
        break;
      }
      case 2:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
        if(stmtupdate.bindin(updatecount,tp,m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)        
          return false;
        break;
      }
      case 3:
      {
        char* tp=0;
        tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
        if(stmtupdate.bindin(updatecount,tp,m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)        
          return false;
        break; 
      }
      case 4:
      {
        updatecount--;
        break;
      }
      default: 
        return false;
        break;
    }
  }
  return true;
}

bool CQuickBind::Execute()
{
  //标志位归零
  keymark=0;

  if(stmtquery.execute() != 0)
  {
    failedtotalcount++;
    return false;
  }
   
  stmtquery.next();  

  if(keymark != 0)
  {
    if(stmtupdate.execute() != 0)
    {
      failedtotalcount++;
      return false;
    }
    updatetotalcount++;
    return true;
  }  
  else
  {
    if(stmtinsert.execute() != 0)
    {
      failedtotalcount++;
      return false;
    }
    inserttotalcount++;
    return true;
  }
}

bool CQuickBind::InsertExecute()
{
  if(stmtinsert.execute() != 0)
  {
    failedtotalcount++;
    return false;
  }
  
  inserttotalcount++;
  return true;
}

int CQuickBind::QueryExecute()
{
  if(stmtquery.execute() != 0)
    return -1;

  keymark=0;
  stmtquery.next();
  return keymark;
}

bool CQuickBind::UpdateExecute()
{
  if(stmtupdate.execute() != 0)
  {
    failedtotalcount++;
    return false;
  }

  updatetotalcount++;
  return true;
}

void CQuickBind::Error()
{
    printf("\nstmtinsert.execute() failed.\n%s\n",stmtinsert.m_cda.message);
    printf("\nstmtquery.execute() failed.\n%s\n",stmtquery.m_cda.message);
    printf("\nstmtupdate.execute() failed.\n%s\n",stmtupdate.m_cda.message);
}

void CQuickBind::Report()
{
  printf("\n插入了%d条数据,更新了%d条数据，失败了%d条数据\n",inserttotalcount,updatetotalcount,failedtotalcount);
}


int CQuickBind::Insertcount()  { return inserttotalcount; }
int CQuickBind::Updatecount()  { return updatetotalcount; }
int CQuickBind::Failedcount()  { return failedtotalcount; }
