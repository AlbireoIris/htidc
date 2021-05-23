#include"_Mylib.h"

//��һ�ֹ��캯�� ɶҲû��
CQuickBind::CQuickBind()
{
  Init();
}

//�ڶ��ֹ��캯���б����������Ӻõľ��
CQuickBind::CQuickBind(char* tablename,connection &fconn)
{
  Init();
  conn=fconn;  
  Connect(conn);
  tablenamestring=tablename;
}

//�����ֹ��캯���б��������Ӳ���
CQuickBind::CQuickBind(char* tablename,char* connpar,char* connenvpar,int autocommitopt)
{
  Init();
  Connect(connpar,connenvpar,autocommitopt);
  tablenamestring=tablename;
}

//��֪�������������������ע�͵�Ŀ����Ϊ�˱��ֶ���ͳһ
CQuickBind::~CQuickBind()
{
  Init();
  Disconnect();
}

//����һЩ��ʼ������
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

//������ݿ�����״̬�������������״̬���ύ���Ͽ�����
//���ڿ��Կ������һ���ع�ѡ��
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
  //����������ӣ��ͶϿ���������
  if(conn.m_state != 0)
    Disconnect();
 
  //�ж����ӽ��
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
  //�Դ�����Ϣ�Ľṹ�帳ֵ
  stm_eleinfo.tmark = 0;
  stm_eleinfo.pmark = m_vintaddress.size();
  stm_eleinfo.cloumnname = columnname; 

  //�ѱ�����ַ�����Ӧ����
  m_vintaddress.push_back(&par);

  //�����Ƿֱ����ȫ��������
  m_vstentire.push_back(stm_eleinfo);

  //���keyid=true�Ͱ�����������������
  if(keyid == true)
    m_vstkey.push_back(stm_eleinfo);
  else
    m_vstnkey.push_back(stm_eleinfo);

  return true;
}

bool CQuickBind::Bind(double& par,const char* columnname,bool keyid)
{ 
  //�Դ�����Ϣ�Ľṹ�帳ֵ
  stm_eleinfo.tmark = 1;
  stm_eleinfo.pmark = m_vdoubleaddress.size();
  stm_eleinfo.cloumnname = columnname;   

  //�ѱ�����ַ�����Ӧ����
  m_vdoubleaddress.push_back(&par);

  //�����Ƿֱ����ȫ��������
  m_vstentire.push_back(stm_eleinfo);

  //���keyid=true�Ͱ�����������������
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
    //�洢�ַ����ĵ�ַ�ͳ���
    memset(&stm_charstore,0,sizeof(struct m_charstore));
    stm_charstore.charaddress=(char*)par;
    stm_charstore.stringlen=len;
  
    //�Դ�����Ϣ�Ľṹ�帳ֵ
    stm_eleinfo.tmark = 2;
    stm_eleinfo.pmark = m_vcharststore.size();
    stm_eleinfo.cloumnname = columnname;
  
    //�ѽṹ������Ӧ����
    m_vcharststore.push_back(stm_charstore);
  
    //�����Ƿֱ����ȫ��������
    m_vstentire.push_back(stm_eleinfo);
  
    //���keyid=true�Ͱ�����������������
    if(keyid == true)
      m_vstkey.push_back(stm_eleinfo);
    else
      m_vstnkey.push_back(stm_eleinfo);
  }
  else
  {
    //�Դ�����Ϣ�Ľṹ�帳ֵ
    stm_eleinfo.tmark = 4;
    stm_eleinfo.pmark = m_vconststring.size();
    stm_eleinfo.cloumnname = columnname;
  
    //�洢�̶��ַ���������
    string tempstring = par;
    m_vconststring.push_back(tempstring);
  
    //�ѽṹ������Ӧ����
    m_vcharststore.push_back(stm_charstore);
  
    //�����Ƿֱ����ȫ��������
    m_vstentire.push_back(stm_eleinfo);
  
    //���keyid=true�Ͱ�����������������
    if(keyid == true)
      m_vstkey.push_back(stm_eleinfo);
    else
      m_vstnkey.push_back(stm_eleinfo);   
  }
  return true;
}

//����������������������͵�
bool CQuickBind::Bind(const char* par,const char* columnname,const char* datetype,bool keyid,unsigned int len)
{ 
  //�洢�ַ����ĵ�ַ�ͳ���
  memset(&stm_charstore,0,sizeof(struct m_charstore)); //��ʼ���ṹ��
  stm_charstore.charaddress=(char*)par;                //��Ų�����ַ
  stm_charstore.stringlen=len;                         //����ַ������ȣ�Ĭ��Ϊ32
  string tdatetype = datetype;                         //������ڸ�ʽ
  
  //�Դ�����Ϣ�Ľṹ�帳ֵ
  stm_eleinfo.tmark = 3;
  stm_eleinfo.pmark = m_vcharststore.size();
  stm_eleinfo.cloumnname = columnname;
  
  //�ѽṹ������Ӧ����
  m_vcharststore.push_back(stm_charstore);
  
  //�����ڸ�ʽ����m_vdatestring������
  m_vdatestring.push_back(tdatetype);

  //�����Ƿֱ����ȫ��������
  m_vstentire.push_back(stm_eleinfo);
  
  //���keyid=true�Ͱ�����������������
  if(keyid == true)
    m_vstkey.push_back(stm_eleinfo);
  else
    m_vstnkey.push_back(stm_eleinfo);
  
  return true;
}

//����ʧ�ܵ�string��󶨳���
//bool CQuickBind::Bind(string&  par,const char* columnname,bool keyid)
//{
////  memset(temp_string_str,0,sizeof(temp_string_str));
////  strcpy(temp_string_str,par.c_str());
//  
//
//  //�洢�ַ����ĵ�ַ�ͳ���
//  memset(&stm_charstore,0,sizeof(struct m_charstore));
////  stm_charstore.charaddress=(char*)temp_string_str;
////  stm_charstore.stringlen=1024;
//  stm_charstore.charaddress=(char*)par.c_str();
//  stm_charstore.stringlen=par.length();
//
//  //�Դ���Ԫ����Ϣ�Ľṹ�帳ֵ
//  stm_eleinfo.tmark = 2;
//  stm_eleinfo.pmark = m_vstrststore.size();
//  stm_eleinfo.cloumnname = columnname;
//  
//  //�Ѵ洢�ַ�����Ϣ�Ľṹ���������
//  m_vstrststore.push_back(stm_charstore);
//  
//  //�Ѵ���Ԫ����Ϣ�Ľṹ�����ȫ��������
//  m_vstentire.push_back(stm_eleinfo);
//  
//  //���keyid=true�Ͱ����������������з���������������
//  if(keyid == true)
//    m_vstkey.push_back(stm_eleinfo);
//  else 
//    m_vstnkey.push_back(stm_eleinfo);
//   
//  return true;
//}


//�������������׼�������ѯ��������
bool CQuickBind::Prepare()
{
  int acc;
  int count;
  char temp[16];   //�����ݴ�����ת�ַ�����
  int  datecount=0;
  
//������׼��insert��prepare���
  insertstring +="insert into ";
  insertstring +=tablenamestring;
  insertstring +=" (";
  
  //д�����������ͷ�����������
  for(acc=0,count=0;count<m_vstentire.size();acc++,count++)
  {
    insertstring+=m_vstentire[acc].cloumnname;

    //����ǹ̶����;Ͳ�Ҫ����
    if(m_vstentire[acc].tmark == 4)
      acc--;

    if(count != m_vstentire.size()-1)
      insertstring+=",";  
  }

  insertstring+=") values(";

  //���������ͷ�����������
  for(acc=0,count=0;count<m_vstentire.size();acc++,count++)
  {
    //����ת�ַ���
    memset(temp,0,sizeof(temp));
    sprintf(temp,"%d",acc+1);

    switch(m_vstentire[acc].tmark)
    {
      case 0:{}   //��0-2����һ�ִ�����
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

  //������׼��query��prepare���
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
  
  //������׼��update��prepare���
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

  //��һ����insert�İ󶨲���
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

  //��һ����query�İ󶨲���
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

  //��һ����update�İ󶨲���
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
  //��־λ����
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
  printf("\n������%d������,������%d�����ݣ�ʧ����%d������\n",inserttotalcount,updatetotalcount,failedtotalcount);
}


int CQuickBind::Insertcount()  { return inserttotalcount; }
int CQuickBind::Updatecount()  { return updatetotalcount; }
int CQuickBind::Failedcount()  { return failedtotalcount; }
