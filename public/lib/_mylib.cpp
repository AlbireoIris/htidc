#include "_mylib.h"

//��һ�ֹ��캯�� ɶҲû��
CQuickBind::CQuickBind()
{
  Init();
}

//�ڶ��ֹ��캯���б����������Ӻõľ��
CQuickBind::CQuickBind(char *tablename, connection &fconn)
{
  Init();
  conn = fconn;
  Connect(conn);
  tablenamestring = tablename;
}

//�����ֹ��캯���б��������Ӳ���
CQuickBind::CQuickBind(char *tablename, char *connpar, char *connenvpar, int autocommitopt)
{
  Init();
  Connect(connpar, connenvpar, autocommitopt);
  tablenamestring = tablename;
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

  inserttotalcount = failedtotalcount = updatetotalcount = 0;
}

//������ݿ�����״̬�������������״̬���ύ���Ͽ�����
//���ڿ��Կ������һ���ع�ѡ��
void CQuickBind::Disconnect()
{
  if (conn.m_state != 0)
  {
    conn.commit();
    conn.disconnect();
  }
}

bool CQuickBind::Connect(char *connpar, char *connenvpar, int autocommitopt)
{
  //����������ӣ��ͶϿ���������
  if (conn.m_state != 0)
    Disconnect();

  //�ж����ӽ��
  if (conn.connecttodb(connpar, connenvpar, autocommitopt) != 0)
    return false;

  if (stmtinsert.connect(&conn) != 0)
    return false;
  if (stmtupdate.connect(&conn) != 0)
    return false;
  if (stmtquery.connect(&conn) != 0)
    return false;

  return true;
}

bool CQuickBind::Connect(connection &fconn)
{
  if (fconn.m_state != 1)
    return false;

  if (stmtinsert.connect(&fconn) != 0)
    return false;
  if (stmtupdate.connect(&fconn) != 0)
    return false;
  if (stmtquery.connect(&fconn) != 0)
    return false;
  return true;
}

bool CQuickBind::Bind(int &par, const char *columnname, bool keyid)
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
  if (keyid == true)
    m_vstkey.push_back(stm_eleinfo);
  else
    m_vstnkey.push_back(stm_eleinfo);

  return true;
}

bool CQuickBind::Bind(double &par, const char *columnname, bool keyid)
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
  if (keyid == true)
    m_vstkey.push_back(stm_eleinfo);
  else
    m_vstnkey.push_back(stm_eleinfo);

  return true;
}

bool CQuickBind::Bind(const char *par, const char *columnname, unsigned int len, bool keyid)
{
  if (len != 0)
  {
    //�洢�ַ����ĵ�ַ�ͳ���
    memset(&stm_charstore, 0, sizeof(struct m_charstore));
    stm_charstore.charaddress = (char *)par;
    stm_charstore.stringlen = len;

    //�Դ�����Ϣ�Ľṹ�帳ֵ
    stm_eleinfo.tmark = 2;
    stm_eleinfo.pmark = m_vcharststore.size();
    stm_eleinfo.cloumnname = columnname;

    //�ѽṹ������Ӧ����
    m_vcharststore.push_back(stm_charstore);

    //�����Ƿֱ����ȫ��������
    m_vstentire.push_back(stm_eleinfo);

    //���keyid=true�Ͱ�����������������
    if (keyid == true)
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
    if (keyid == true)
      m_vstkey.push_back(stm_eleinfo);
    else
      m_vstnkey.push_back(stm_eleinfo);
  }
  return true;
}

//����������������������͵�
bool CQuickBind::Bind(const char *par, const char *columnname, const char *datetype, bool keyid, unsigned int len)
{
  //�洢�ַ����ĵ�ַ�ͳ���
  memset(&stm_charstore, 0, sizeof(struct m_charstore)); //��ʼ���ṹ��
  stm_charstore.charaddress = (char *)par;               //��Ų�����ַ
  stm_charstore.stringlen = len;                         //����ַ������ȣ�Ĭ��Ϊ32
  string tdatetype = datetype;                           //������ڸ�ʽ

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
  if (keyid == true)
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
  char temp[16]; //�����ݴ�����ת�ַ�����
  int datecount = 0;

  //������׼��insert��prepare���
  insertstring += "insert into ";
  insertstring += tablenamestring;
  insertstring += " (";

  //д�����������ͷ�����������
  for (acc = 0, count = 0; count < m_vstentire.size(); acc++, count++)
  {
    insertstring += m_vstentire[acc].cloumnname;

    //����ǹ̶����;Ͳ�Ҫ����
    if (m_vstentire[acc].tmark == 4)
      acc--;

    if (count != m_vstentire.size() - 1)
      insertstring += ",";
  }

  insertstring += ") values(";

  //���������ͷ�����������
  for (acc = 0, count = 0; count < m_vstentire.size(); acc++, count++)
  {
    //����ת�ַ���
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", acc + 1);

    switch (m_vstentire[acc].tmark)
    {
    case 0:
    {
    } //��0-2����һ�ִ�����
    case 1:
    {
    }
    case 2:
    {
      insertstring += ":";
      insertstring += temp;
      break;
    }
    case 3:
    {
      insertstring += "to_date(:";
      insertstring += temp;
      insertstring += ",'";
      insertstring += m_vdatestring[datecount];
      insertstring += "')";
      break;
    }
    case 4:
    {
      insertstring += m_vconststring[m_vstentire[acc].pmark];
      acc--;
      break;
    }
    default:
      return false;
    }

    if (count != m_vstkey.size() + m_vstnkey.size() - 1)
      insertstring += ",";
  }

  insertstring += ")";
  if (stmtinsert.prepare(insertstring.c_str()) != 0)
    return false;

  //������׼��query��prepare���
  querystring += "select count(*) from ";
  querystring += tablenamestring;
  querystring += " where ";
  for (acc = 0, count = 0; count < m_vstkey.size(); acc++, count++)
  {
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", acc + 1);

    switch (m_vstkey[acc].tmark)
    {
    case 0:
    {
    }
    case 1:
    {
    }
    case 2:
    {
      querystring += m_vstkey[acc].cloumnname;
      querystring += "=:";
      querystring += temp;
      break;
    }
    case 3:
    {
      querystring += m_vstkey[acc].cloumnname;
      querystring += "=";
      querystring += "to_date(:";
      querystring += temp;
      querystring += ",'";
      querystring += m_vdatestring[datecount];
      querystring += "')";
      break;
    }
    case 4:
    {
      querystring += m_vconststring[m_vstkey[acc].pmark];
      acc--;
      break;
    }
    default:
      return false;
    }

    if (count != m_vstkey.size() - 1)
      querystring += " and ";
  }
  if (stmtquery.prepare(querystring.c_str()) != 0)
    return false;

  //������׼��update��prepare���
  updatestring += "update ";
  updatestring += tablenamestring;
  updatestring += " set ";

  int posmark = 1;
  for (acc = 0, count = 0; count < m_vstnkey.size(); acc++, count++, posmark++)
  {
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", posmark);

    switch (m_vstnkey[acc].tmark)
    {
    case 0:
    {
    }
    case 1:
    {
    }
    case 2:
    {
      updatestring += m_vstnkey[acc].cloumnname;
      updatestring += "=:";
      updatestring += temp;
      break;
    }
    case 3:
    {
      updatestring += m_vstnkey[acc].cloumnname;
      updatestring += "=";
      updatestring += "to_date(:";
      updatestring += temp;
      updatestring += ",'";
      updatestring += m_vdatestring[datecount];
      updatestring += "')";
      break;
    }
    case 4:
    {
      updatestring += m_vstnkey[acc].cloumnname;
      updatestring += "=";
      updatestring += m_vconststring[m_vstnkey[acc].pmark];
      acc--;
      posmark--;
      break;
    }
    default:
      return false;
    }

    if (count != m_vstnkey.size() - 1)
      updatestring += ",";

    updatestring += " ";
  }

  updatestring += " where ";

  for (acc = 0, count = 0; count < m_vstkey.size(); acc++, posmark++, count++)
  {
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "%d", posmark);

    switch (m_vstkey[acc].tmark)
    {
    case 0:
    {
    }
    case 1:
    {
    }
    case 2:
    {
      updatestring += m_vstkey[acc].cloumnname;
      updatestring += "=:";
      updatestring += temp;
      break;
    }
    case 3:
    {
      updatestring += m_vstkey[acc].cloumnname;
      updatestring += "=";
      updatestring += "to_date(:";
      updatestring += temp;
      updatestring += ",'";
      updatestring += m_vdatestring[datecount];
      updatestring += "')";
      break;
    }
    case 4:
    {
      updatestring += m_vstkey[acc].cloumnname;
      updatestring += "=";
      updatestring += m_vconststring[m_vstkey[acc].pmark];
      acc--;
      posmark--;
      break;
    }
    default:
      return false;
    }

    if (count != m_vstkey.size() - 1)
      updatestring += " and ";
  }

  if (stmtupdate.prepare(updatestring.c_str()) != 0)
    return false;

  return true;
}

bool CQuickBind::Bindin()
{
  int insertcount = 1;
  int querycount = 1;
  int updatecount = 1;

  //��һ����insert�İ󶨲���
  for (int acc = 0; acc < m_vstentire.size(); acc++, insertcount++)
  {
    switch (m_vstentire[acc].tmark)
    {
    case 0:
    {
      int *tp = 0;
      tp = m_vintaddress[m_vstentire[acc].pmark];
      if (stmtinsert.bindin(insertcount, tp) != 0)
        return false;
      break;
    }
    case 1:
    {
      double *tp = 0;
      tp = m_vdoubleaddress[m_vstentire[acc].pmark];
      if (stmtinsert.bindin(insertcount, tp) != 0)
        return false;
      break;
    }
    case 2:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstentire[acc].pmark].charaddress;
      if (stmtinsert.bindin(insertcount, tp, m_vcharststore[m_vstentire[acc].pmark].stringlen) != 0)
        return false;
      break;
    }
    case 3:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstentire[acc].pmark].charaddress;
      if (stmtinsert.bindin(insertcount, tp, m_vcharststore[m_vstentire[acc].pmark].stringlen) != 0)
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
  for (int acc = 0; acc < m_vstkey.size(); acc++, querycount++)
  {
    switch (m_vstkey[acc].tmark)
    {
    case 0:
    {
      int *tp = 0;
      tp = m_vintaddress[m_vstkey[acc].pmark];
      if (stmtquery.bindin(querycount, tp) != 0)
        return false;
      break;
    }
    case 1:
    {
      double *tp = 0;
      tp = m_vdoubleaddress[m_vstkey[acc].pmark];
      if (stmtquery.bindin(querycount, tp) != 0)
        return false;
      break;
    }
    case 2:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
      if (stmtquery.bindin(querycount, tp, m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)
        return false;
      break;
    }
    case 3:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
      if (stmtquery.bindin(querycount, tp, m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)
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

  if (stmtquery.bindout(1, &keymark) != 0)
    return false;

  //��һ����update�İ󶨲���
  for (int acc = 0; acc < m_vstnkey.size(); acc++, updatecount++)
  {
    switch (m_vstnkey[acc].tmark)
    {
    case 0:
    {
      int *tp = 0;
      tp = m_vintaddress[m_vstnkey[acc].pmark];
      if (stmtupdate.bindin(updatecount, tp) != 0)
        return false;
      break;
    }
    case 1:
    {
      double *tp = 0;
      tp = m_vdoubleaddress[m_vstnkey[acc].pmark];
      if (stmtupdate.bindin(updatecount, tp) != 0)
        return false;
      break;
    }
    case 2:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstnkey[acc].pmark].charaddress;
      if (stmtupdate.bindin(updatecount, tp, m_vcharststore[m_vstnkey[acc].pmark].stringlen) != 0)
        return false;
      break;
    }
    case 3:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstnkey[acc].pmark].charaddress;
      if (stmtupdate.bindin(updatecount, tp, m_vcharststore[m_vstnkey[acc].pmark].stringlen) != 0)
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

  for (int acc = 0; acc < m_vstkey.size(); acc++, updatecount++)
  {
    switch (m_vstkey[acc].tmark)
    {
    case 0:
    {
      int *tp = 0;
      tp = m_vintaddress[m_vstkey[acc].pmark];
      if (stmtupdate.bindin(updatecount, tp) != 0)
        return false;
      break;
    }
    case 1:
    {
      double *tp = 0;
      tp = m_vdoubleaddress[m_vstkey[acc].pmark];
      if (stmtupdate.bindin(updatecount, tp) != 0)
        return false;
      break;
    }
    case 2:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
      if (stmtupdate.bindin(updatecount, tp, m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)
        return false;
      break;
    }
    case 3:
    {
      char *tp = 0;
      tp = m_vcharststore[m_vstkey[acc].pmark].charaddress;
      if (stmtupdate.bindin(updatecount, tp, m_vcharststore[m_vstkey[acc].pmark].stringlen) != 0)
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
  keymark = 0;

  if (stmtquery.execute() != 0)
  {
    failedtotalcount++;
    return false;
  }

  stmtquery.next();

  if (keymark != 0)
  {
    if (stmtupdate.execute() != 0)
    {
      failedtotalcount++;
      return false;
    }
    updatetotalcount++;
    return true;
  }
  else
  {
    if (stmtinsert.execute() != 0)
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
  if (stmtinsert.execute() != 0)
  {
    failedtotalcount++;
    return false;
  }

  inserttotalcount++;
  return true;
}

int CQuickBind::QueryExecute()
{
  if (stmtquery.execute() != 0)
    return -1;

  keymark = 0;
  stmtquery.next();
  return keymark;
}

bool CQuickBind::UpdateExecute()
{
  if (stmtupdate.execute() != 0)
  {
    failedtotalcount++;
    return false;
  }

  updatetotalcount++;
  return true;
}

void CQuickBind::Error()
{
  printf("\nstmtinsert.execute() failed.\n%s\n", stmtinsert.m_cda.message);
  printf("\nstmtquery.execute() failed.\n%s\n", stmtquery.m_cda.message);
  printf("\nstmtupdate.execute() failed.\n%s\n", stmtupdate.m_cda.message);
}

void CQuickBind::Report()
{
  printf("\n������%d������,������%d�����ݣ�ʧ����%d������\n", inserttotalcount, updatetotalcount, failedtotalcount);
}

inline int CQuickBind::Insertcount() { return inserttotalcount; }
inline int CQuickBind::Updatecount() { return updatetotalcount; }
inline int CQuickBind::Failedcount() { return failedtotalcount; }

int TableToFile(connection *conn, char *tname, char *colname, char *condition, char *localname, CLogFile *logfile)
{
  sqlstatement stmt(conn);
  stmt.prepare("select %s from %s where %s", colname, tname, condition);
  stmt.bindblob();

  if (stmt.execute() != 0)
  {
    logfile->Write_P("TableToFile.execute() Failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
    return stmt.m_cda.rc;
  }

  if (stmt.next() != 0)
  {
    logfile->Write_P("TableToFile.next() Failed!\n");
    logfile->Write_P("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
    return stmt.m_cda.rc;
  }

  if (stmt.lobtofile((char *)localname) != 0)
  {
    logfile->Write_P("TableToFile.lobtofile() Failed!\n");
    logfile->Write_P("%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
    return stmt.m_cda.rc;
  }

  return 0;
}

//������
//int FileToTable(connection *conn,char* tname,char* colname,char* condition,char* filename,CLogFile *logfile)
//{
//  sqlstatement stmtins(conn);
////stmtins.prepare("select filecon from T_SURFPIC where filename=:1 for update");
//  stmtins.prepare("select %s from %s where %s for update",colname,tname,condition);
////stmtins.bindin(1,starg.filename,255);
//  stmtins.bindblob();
//
//  if(stmtins.execute() != 0)
//  {
//    logfile->Write_P("FileToTable().stmt.execute() Failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
//    return stmtins.m_cda.rc;
//  }
//
//  �˴����ֶδ���
//  if (stmtins.next() != 0)
//  {
//    logfile->Write_P("FileToTable().stmt.execute() Failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
//    return stmtins.m_cda.rc;
//  }
//
//  if (stmtins.filetolob((char *)filename) != 0)
//  {
//    logfile->Write_P("FileToTable().stmt.filetolob() Failed.\n%s\n",stmtins.m_cda.message);
//    return -1;
//  }
//
//  conn->commit();
//
//  return 0;
//}

//�����������ɼ�Incc�������
//CIncc::CIncc(char* FileName,char* DirName,char* DirMatch,const unsigned int in_MaxCount,const bool bAndChild,bool bSort)
//{
//  //Mode=inMode;
//  strcpy(sFileName,FileName);
//  strcpy(sDirName,DirName);
//  strcpy(sDirMatch,DirMatch);
//  MaxCount=in_MaxCount;
//  AndChild=bAndChild;
//  Sort=bSort;
//
//  LoadInitFile();
//}
//
//// ��xml�ļ����ݼ��ص�m_vEnTireProcessedList������
//bool CIncc::LoadInitFile()
//{
//  m_vEnTireProcessedList.clear();
//
//  CFile File;
//
//  // ע�⣺��������ǵ�һ�βɼ���okfilename�ǲ����ڵģ������Ǵ�������Ҳ����true��
//  if (File.Open(sFileName,"r",false) == false)
//    return true;
//
//  char strbuffer[301];
//
//  while (true)
//  {
//    memset(&stfileinfo,0,sizeof(struct st_fileinfo));
//
//    if (File.Fgets(strbuffer,300,true)==false)
//      break;
//
//    GetXMLBuffer(strbuffer,"filename",stfileinfo.filename,300);
//    GetXMLBuffer(strbuffer,"mtime",stfileinfo.mtime,20);
//
//    m_vEnTireProcessedList.push_back(stfileinfo);
//  }
//  return true;
//}
//
////���ļ�����Ϣ����m_vDirList������
//bool CIncc::LoadDir()
//{
//  m_vDirList.clear();
//
//  if(Dir.OpenDir(sDirName,sDirMatch,MaxCount,AndChild,Sort) == false)
//    return false;
//
//  while(true)
//  {
//    if(Dir.ReadDir() == false)
//      break;
//
//    memset(&stfileinfo,0,sizeof(struct st_fileinfo));
//
//    strcpy(stfileinfo.filename,Dir.m_FullFileName);
//    strcpy(stfileinfo.mtime,Dir.m_ModifyTime);
//    stfileinfo.filesize=Dir.m_FileSize;
//
//    m_vDirList.push_back(stfileinfo);
//  }
//  return true;
//}
//
//
//// ��m_vDirList�����е��ļ���m_vEnTireProcessedList�������ļ��Աȣ��õ���������
//// һ����m_vDirList�д��ڣ����Ѿ��ɼ��ɹ����ļ�m_vPartialProcessedList
//// ������m_vDirList�д��ڣ����ļ�����Ҫ���²ɼ����ļ�m_vWaitProcessList
//bool CIncc::CompVector()
//{
//  m_vPartialProcessedList.clear();
//  m_vWaitProcessList.clear();
//
//  for (int ii=0;ii<m_vDirList.size();ii++)
//  {
//    int jj=0;
//    for (jj=0;jj<m_vEnTireProcessedList.size();jj++)
//    {
//      if ( (strcmp(m_vDirList[ii].filename,m_vEnTireProcessedList[jj].filename)==0) &&
//           (strcmp(m_vDirList[ii].mtime,m_vEnTireProcessedList[jj].mtime)==0) )
//      {
//        m_vPartialProcessedList.push_back(m_vDirList[ii]);
//        break;
//      }
//    }
//
//    if (jj == m_vEnTireProcessedList.size())
//    {
//      m_vWaitProcessList.push_back(m_vDirList[ii]);
//    }
//  }
//  return true;
//}
//
//bool CIncc::ReadList()
//{
//  int localsize=m_vWaitProcessList.size();
//
//  if(m_pos >= localsize)
//  {
//    m_pos=0;
//
//    m_vEnTireProcessedList.swap(m_vPartialProcessedList);
//
//    //ÿһ�ζ�ȡ��϶�дһ��
//    WriteToFile();
//
//    m_vWaitProcessList.clear();
//
//    return false;
//  }
//
//  memset(m_FullFileName,0,sizeof(m_FullFileName));
//  strcpy(m_FullFileName,m_vWaitProcessList[m_pos].filename);
//
//  memset(m_ModityTime,0,sizeof(m_ModityTime));
//  strcpy(m_ModityTime,m_vWaitProcessList[m_pos].mtime);
//
//  m_FileSize=m_vWaitProcessList[m_pos].filesize;
//
//  m_vPartialProcessedList.push_back(m_vWaitProcessList[m_pos]);
//
//  m_pos++;
//
//  return true;
//}
//
//// ��m_vEnTireProcessedList�����е�����д��xml�ļ��У�����֮ǰ�ľ�xml�ļ�
//bool CIncc::WriteToFile()
//{
//  if(m_vEnTireProcessedList.size() == 0)
//    return true;
//
//  CFile File;
//
//  // ע�⣬���ļ���Ҫ���û������
//  if (File.Open(sFileName,"w",false) == false)
//    return false;
//
//  for (int ii=0;ii<m_vEnTireProcessedList.size();ii++)
//  {
//    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",m_vEnTireProcessedList[ii].filename,m_vEnTireProcessedList[ii].mtime);
//  }
//
//  return true;
//}
//
////���ļ���׷�Ӽ�¼
//bool CIncc::AppendToFile(char* inFileName,char* inModityTime)
//{
//  CFile File;
//
//  if(File.Open(sFileName,"a",false) == false)
//    return false;
//
//  File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n",inFileName,inModityTime);
//
//  return true;
//}
//
//bool CIncc::IncFind()
//{
//  if(LoadDir() == false)
//    return false;
//
//  if(CompVector() == false)
//    return false;
//
//  return true;
//}
//
//
//CIncc::~CIncc()
//{
////  WriteToFile();
//
////  sFileName.clear();
////  sDirName.clear();
////  sDirMatch.clear();
//
//  m_vEnTireProcessedList.clear();
//  m_vDirList.clear();
//  m_vWaitProcessList.clear();
//  m_vPartialProcessedList.clear();
//}

////������CJson_Object�������
///*�ڳ�ʼ����ͬʱ����һ������*/
////ÿһ�����ʵ������������Json��һ��Object
//CJson_Object::CJson_Object()
//{
//  m_Json = cJSON_CreateObject();
//}
//
//CJson_Object::~CJson_Object()
//{
//  if (m_Json != NULL)
//    cJSON_Delete(m_Json);
//}
//
//int CJson_Object::ItemCount()
//{
//  cJSON_GetArraySize(m_Json);
//}
//
//char *CJson_Object::Printf()
//{
//  return cJSON_Print(m_Json);
//}
//
//char *CJson_Object::Print()
//{
//  return cJSON_PrintUnformatted(m_Json);
//}
//
//void CJson_Object::Add_Item(char *ItemName, char *strBuffer)
//{
//  cJSON_AddItemToObjectCS(m_Json, ItemName, cJSON_CreateString(strBuffer));
//}
//
//void CJson_Object::Add_Item(char *ItemName, int NumBuffer)
//{
//  cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateNumber(NumBuffer));
//}
//
//void CJson_Object::Add_Item(char *ItemName, double NumBuffer)
//{
//  cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateNumber(NumBuffer));
//}
//
//void CJson_Object::Add_Item(char *ItemName)
//{
//  cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateNull());
//}
//
//void CJson_Object::Add_Item(char *ItemName, const char *StrArry[], int ArryNum)
//{
//  cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateStringArray(StrArry, ArryNum));
//}
//
//void CJson_Object::Add_Item(char *ItemName, int IntArry[], int ArryNum)
//{
//  cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateIntArray(IntArry, ArryNum));
//}
//
//void CJson_Object::Add_Item(char *ItemName, double DoubleArry[], int ArryNum)
//{
//  cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateDoubleArray(DoubleArry, ArryNum));
//}
//
//void CJson_Object::Add_Item(char *ItemName, bool Bool)
//{
//  cJSON_AddItemToObject(m_Json, ItemName, cJSON_CreateBool(Bool));
//}
//
//void CJson_Object::Add_Item(char *ItemName, cJSON *Item)
//{
//  cJSON_AddItemToObjectCS(m_Json, ItemName, Item);
//}
//
//void CJson_Object::Add_Item(char *ItemName, class CJson_Object &Add_cJson)
//{
//  cJSON_AddItemReferenceToObject(m_Json, ItemName, Add_cJson.m_Json);
//}
//
//void CJson_Object::Remove_Item(char *ItemName)
//{
//  cJSON_DeleteItemFromObject(m_Json, ItemName);
//}
//
//void CJson_Object::Add_Item(char *ItemName, class CJson_Array &Add_cJson)
//{
//  cJSON_AddItemReferenceToObject(m_Json, ItemName, Add_cJson.m_cJson);
//}
//
////������CJson_Array�������
//CJson_Array::CJson_Array()
//{
//  m_cJson = cJSON_CreateArray();
//}
//
//CJson_Array::~CJson_Array()
//{
//  if (m_cJson != NULL)
//    cJSON_Delete(m_cJson);
//}
//
//char *CJson_Array::Print()
//{
//  return cJSON_PrintUnformatted(m_cJson);
//}
//
//char *CJson_Array::Printf()
//{
//  return cJSON_Print(m_cJson);
//}
//
//int CJson_Array::ItemCount()
//{
//  return cJSON_GetArraySize(m_cJson);
//}
//
//void CJson_Array::Add_Item(class CJson_Array &Item)
//{
//  cJSON_AddItemReferenceToArray(m_cJson, Item.m_cJson);
//}
//
//void CJson_Array::Add_Item(class CJson_Object &Item)
//{
//  cJSON_AddItemReferenceToArray(m_cJson, Item.m_Json);
//}
//
//void CJson_Array::Add_Item()
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateNull());
//}
//
//void CJson_Array::Add_Item(int IntNum)
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateNumber(IntNum));
//}
//
//void CJson_Array::Add_Item(double DouNum)
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateNumber(DouNum));
//}
//
//void CJson_Array::Add_Item(bool Bool)
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateBool(Bool));
//}
//
//void CJson_Array::Add_Item(char *String)
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateString(String));
//}
//
//void CJson_Array::Add_Item(const char *StringArray[], int StringCount)
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateStringArray(StringArray, StringCount));
//}
//
//void CJson_Array::Add_Item(int IntArray[], int IntCount)
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateIntArray(IntArray, IntCount));
//}
//
//void CJson_Array::Add_Item(double DouArray[], int DouCount)
//{
//  cJSON_AddItemToArray(m_cJson, cJSON_CreateDoubleArray(DouArray, DouCount));
//}
