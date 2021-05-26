#include "_freecplus.h"
#include "_ooci.h"

//�������
CLogFile logfile;
connection conn;

vector<int>    vcmdstrnum; //��������ֶε�
vector<string> vcmdstrstr; //������ų��ȵ�

//�����ṹ�������
struct st_arg
{
   char connstr[64];
   char charset[64];
   char tname[64];
   char cols[64];
   char fieldname[256];
   char filedlen[128];
   char andstr[256];
   char bname[256];
   char ename[256];
   char taskname[256];
   char exppath[256];
   int  exptype;
   int  timetvl;
}starg;

//����������
void  EXIT(int sig);          //�˳��źŵĴ���
void  _help(char* argv[]);    //��ʾ��Ϣ
bool  _exptables();           //����������
bool  getargs(char* argv);    //�������������ṹ��
bool  GetHisKey();            //��ȡ��ʷ��������
bool  UpdateKey();            //������ʷ��������

//һЩȫ�ֱ���
int HisKey;                   //���������������ʷ��ȡ����keyid������ģʽΪ��������ʱ�������á�
int maxfieldlen=0;            //������������fieldlen�����ֵ


int main(int argc,char* argv[])
{

  if(argc != 3)
  { 
    _help(argv);
    return -1;
  }

//  CloseIOAndSignal(); 
  signal(SIGINT,EXIT);
  signal(SIGTERM,EXIT);

  if(logfile.Open(argv[1],"a+") == false)
  {
    printf("open the logfile: %s failed\n",argv[3]);
    return -1;
  }

  if(getargs(argv[2]) == false)  
    return -1;

  if(conn.connecttodb(starg.connstr,starg.charset) != 0)
  {
    logfile.Write_P("conn.connecttodb %s failed!\n",starg.connstr);
    return -1;
  }

  while(true)
  {
    _exptables();
 
//    sleep(starg.timetvl);
    break;
  } 

  conn.commit();

  return 0;
}


//�����Ǻ����Ķ��岿��
//
//
//
//���������˳��źŵĺ�������
void EXIT(int sig)
{
  logfile.Write("�����˳� sig=%d\n",sig);
  exit(0);
}

//����������̺���
bool _exptables()
{
  //��ȡ������ʷ ��ʷ����key����HisKey��
  if(starg.exptype == 1)
  {
    if(GetHisKey() == false)
    {
      logfile.Write_P("_exptables()/GetHisKey() failed!\n");
      return false; 
    }
  }

  
  //׼��prepare���
  char pretemp[2048];
  memset(pretemp,0,sizeof(pretemp));
  char fieldvalue[vcmdstrstr.size()][maxfieldlen+1]; //��ά�����������get����

  sprintf(pretemp,"select %s ,keyid from %s where 1=1",starg.cols,starg.tname);

  if(starg.exptype == 1)
  {
    char addtemp[1024];
    memset(addtemp,0,sizeof(addtemp));
    sprintf(addtemp," and keyid > %d order by keyid",HisKey);
    strcat(pretemp,addtemp);
  }
  
  sqlstatement stmtget(&conn);
  if(stmtget.prepare(pretemp) != 0)
  {
    logfile.Write_P("exptables()/stmtget.prepare failed!\n%s\n",stmtget.m_cda.message);
    return false;
  }
   
//printf("pretemp = %s\n",pretemp);

  //������󶨵���ά������
  for (int acc=0;acc<vcmdstrstr.size();acc++)
    stmtget.bindout(acc+1,fieldvalue[acc],vcmdstrnum[acc]);
  
  //���Ϊ���������Ҫ��HisKey�ֶκ�keyid��
  if (starg.exptype==1) 
    stmtget.bindout(vcmdstrstr.size()+1,&HisKey);

  if(stmtget.execute() != 0)
  {
    logfile.Write_P("exptables()/stmtget.execute() failed!\n%s\n",stmtget.m_cda.message);
    return false;
  } 

  int   filesql=1;      //���ɵ��ļ����
  char  filename[256]; //ƴ�ӳɵ��ļ���
  char  datetime[32];  //��ǰʱ��
  CFile File;

  while(true)
  {
    //������ڸ�ֵ
    memset(fieldvalue,0,sizeof(fieldvalue));

    if(stmtget.next() != 0)
      break;
     
    //�ļ�û�д򿪵����
    if (File.IsOpened()==false)
    {
      //��ȡ��ǰʱ��
      memset(datetime,0,sizeof(datetime));
      LocalTime(datetime,"yyyymmddhh24miss");

      //Ԥ�ڵ��ļ���
      memset(filename,0,sizeof(filename));
      sprintf(filename,"%s/%s%s%s_%d.xml",starg.exppath,starg.bname,datetime,starg.ename,filesql++);

      //���ļ���������Ϊfilename.tmp
      if (File.OpenForRename(filename,"w")==false)
      {
        logfile.Write_P("File.OpenForRename(%s) failed.\n",filename); 
        return false;
      }

      File.Fprintf("<data>\n");
    }
   
    //��ʼд������
    for(int acc=0;acc<vcmdstrstr.size();acc++)
      File.Fprintf("<%s>%s</%s>",vcmdstrstr[acc].c_str(),fieldvalue[acc],vcmdstrstr[acc].c_str());

    //һ��д������ĩ��endl
    File.Fprintf("<endl/>\n");

    // 1000����¼д��һ���ļ����
    // stmt.m_cda.rpc����˼��Ӱ��1000������
    // ��%���ǿ��ǵ�һ�ű���������������1000�������
    if (stmtget.m_cda.rpc%1000 ==0)
    {
      //�ļ�ĩβ����</data>
      File.Fprintf("</data>\n");
      
      //�ر��ļ�����filename.tmp��tmpȥ��
      if (File.CloseAndRename()==false)
      {
        logfile.Write_P("exptables/File.CloseAndRename(%s) failed.\n",filename); 
        return false;
      }

      // ����ϵͳ����T_SYSARG�����ѵ������ݵ�����keyid
      if (UpdateKey()==false) 
      { 
        logfile.Write_P("exptables/updatekey() failed.\n"); 
        return false; 
      }
      logfile.Write_P("create file %s ok.\n",filename);
    }
  }
  
  //�������ѭ�����ļ�û�йر�˵��û��1000
  if (File.IsOpened()==true)
  {
    //ͬѭ����һ���Ĵ�������
    File.Fprintf("</data>\n");
    if (File.CloseAndRename()==false)
    {
      logfile.Write("File.CloseAndRename(%s) failed.\n",filename); 
      return false;
    }

    // ����ϵͳ����T_SYSARG�����ѵ������ݵ�����keyid
    if (UpdateKey()==false) 
    { 
      logfile.Write("UptMaxKeyid() failed.\n"); 
      return false; 
    }

    logfile.Write_P("create file %s ok.\n",filename);
  }

//  if (stmtget.m_cda.rpc>0) 
    logfile.Write_P("���ε�����%d����¼��\n",stmtget.m_cda.rpc);

  return true;
}

//������Ĳ�����ȡ����֣���ŵĺ���
bool getargs(char* argv) 
{
  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(argv,"connstr",  starg.connstr);
  GetXMLBuffer(argv,"charset",  starg.charset);
  GetXMLBuffer(argv,"tname",    starg.tname);
  GetXMLBuffer(argv,"cols",     starg.cols);
  GetXMLBuffer(argv,"fieldname",starg.fieldname);
  GetXMLBuffer(argv,"filedlen", starg.filedlen);
  GetXMLBuffer(argv,"andstr",   starg.andstr);
  GetXMLBuffer(argv,"bname",    starg.bname);
  GetXMLBuffer(argv,"ename",    starg.ename);
  GetXMLBuffer(argv,"taskname", starg.taskname);
  GetXMLBuffer(argv,"exppath",  starg.exppath);
  GetXMLBuffer(argv,"exptype", &starg.exptype);
  GetXMLBuffer(argv,"timetvl", &starg.timetvl);

  CCmdStr CmdStrstr; //��������ֶε�
  CCmdStr CmdStrnum; //������ֳ��ȵ�

  CmdStrstr.SplitToCmd(starg.fieldname,",");
  CmdStrnum.SplitToCmd(starg.filedlen,",");

  if(CmdStrstr.CmdCount() != CmdStrnum.CmdCount())
  {
    logfile.Write_P("getargs failed! cols�ֶ���filedlen�ֶβ�ƥ��\n");
    return false;
  }

  for(int acc=0;acc<CmdStrstr.CmdCount();acc++)
  {
    int inttemp=atoi(CmdStrnum.m_vCmdStr[acc].c_str());
    if(maxfieldlen < inttemp)
      maxfieldlen=inttemp; 
    string stringtemp=CmdStrstr.m_vCmdStr[acc];
    vcmdstrstr.push_back(stringtemp);
    vcmdstrnum.push_back(inttemp);
  }
  return true;
}

//��ʾ�ĺ���
void _help(char* argv[])
{
  printf("\n");
  printf("����һ��ftp�������ͷ���ĳ����밴���и�ʽ�������ݣ�\n");
  printf("���ܸ����ֶι��ܣ�\n");
  printf("logfilename  ��־�ļ����ļ�·����������þ���·��\n");
  printf("connstr      ���ݿ����Ӳ���\n");
  printf("charset      ���ݿ���ַ�����Ҫ�����ݿ���ַ�������һ�£����������ַ����������\n");
  printf("tname        �ȴ�����������ݿ�ı���\n");
  printf("cols         ��Ҫ�����ֶε��б����Բ��ú���\n");
  printf("fieldname    ��Ҫ�����ֶεı����б�\n");
  printf("filedlen     ��Ҫ�����ֶεĳ��ȣ�������closһһ��Ӧ\n");
  printf("exptype      ����ģʽ��1-����������2-ȫ������\n");
  printf("andstr       �������ݵĸ�������\n");
  printf("bname        �����ļ�������ǰ�벿��\n");
  printf("ename        �����ļ������ĺ�벿��\n");
  printf("taskname     �������������\n");
  printf("exppath      �����ļ���ŵ�·��\n");
  printf("timetvl      �����ļ���ʱ��������λ�룬�������10��\n");
  printf("\n");
  printf("Example:/htidc/public/bin/exptables_FR_OR  /log/shqx/exptables_surfdata.log  \"<connstr>shqx/pwdidc@snorcl11g_119</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_SURFDATA</tname><cols>obtid,to_char(ddatetime,'yyyymmddhh24miss'),t,p,u,wd,wf,r,vis</cols><fieldname>obtid,ddatetime,t,p,u,wd,wf,r,vis</fieldname><filedlen>5,33,8,8,8,8,8,8,8</filedlen><exptype>1</exptype><andstr>and obtid in('59293','50745')</andstr><bname>SURFDATA_</bname><ename>_FOR_HB</ename><taskname>SURFDATA_FOR_HB</taskname><exppath>/data/shqx/exp/tohb</exppath><timetvl>10</timetvl>\"\n\n");
}


//��������������ǻ�ȡ��ʷ����keyֵ
bool GetHisKey()
{
  sqlstatement stmtkey(&conn);
  stmtkey.prepare("select keyid from T_SYSDATA where taskname=:1");
  stmtkey.bindin(1,starg.taskname,20);
  stmtkey.bindout(1,&HisKey);

  if(stmtkey.execute() != 0)
  {
    logfile.Write_P("stmtkey.execute(select keyid from T_SYSDATA) failed!\n%s\n",stmtkey.m_cda.message);
    return false;
  }

  //stmtkey.next() !=0 ��ʾ��ȡ�ַ���ʧ�ܼ��ַ���Ϊ�գ�����һ������
  if(stmtkey.next() != 0)
  {
    HisKey=0;
    stmtkey.prepare("insert into T_SYSDATA (taskname,keyid) values(:1,:2)");
    stmtkey.bindin(1,starg.taskname,20);
    stmtkey.bindin(2,&HisKey);
    if(stmtkey.execute() != 0)
    {
      logfile.Write_P("stmtkey.execute(insert into T_SYSDATA (taskname,keyid)) failed!\n%s\n",stmtkey.m_cda.message);
      return false;
    }
    //һ��Ҫ�ں����ڲ��ύ����Ȼ����Ϊ�ֲ��������ٶ�����������Ԥ�ڽ��
    conn.commit();
  }
  return true;
}

bool UpdateKey()
{
  sqlstatement stmtupdate(&conn);
  stmtupdate.prepare("update T_SYSDATA set keyid=:1 where taskname=:2");
  stmtupdate.bindin(1,&HisKey);
  stmtupdate.bindin(2,starg.taskname,20);

  if(stmtupdate.execute() != 0)
  {
    logfile.Write_P("stmtupdate.execute(select keyid from T_SYSDATA) failed!\n%s\n",stmtupdate.m_cda.message);
    return false;
  }

  conn.commit();

  return true;
}



