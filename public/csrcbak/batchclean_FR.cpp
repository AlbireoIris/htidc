#include "_freecplus.h"

CFile File;
CDir Dir;
CLogFile logfile;

vector<struct st_cleanrule> cleanarg;

struct st_cleanrule
{
  char TargetDir[256];
  char MatchRule[256];
  long int CleanTime;
  int MinNum;
  int MaxSize;
} stcleanrule;

double evaluate(char[]);
int precedence(char);
double calculate(char, double, double);
char checknumber(char);
int FileCount(char *Dir, char *MatchRule = "*", bool OpenChild = false);

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("这是一个指定清理历史文件的程序\n");
    printf("命令实例：./batchclean_FR /htidc/public/c/cleanarg.xml /log/publiclog/batchclear_FR.log\n");
    printf("解释一下参数的作用        ^删除的参数文件              ^日志文件\n");
    return -1;
  }

  if (logfile.Open(argv[2]) == false)
  {
    printf("打开日志文件 %s 失败！\n", argv[2]);
    return -1;
  }

  if (File.Open(argv[1], "r") == false)
  {
    logfile.Write_P("打开参数文件 %s 失败！\n", argv[1]);
    return -1;
  }

  //  void CloseIOAndSignal();

  while (true)
  {
    char strbuffer[1024];
    memset(strbuffer, 0, sizeof(strbuffer));

    if (File.Fgets(strbuffer, 1023) == false)
      break;

    if (sizeof(strbuffer) == 0)
      continue;

    if (strncmp(strbuffer, "//", 2) == 0)
      continue;

    char timetemp[64];
    memset(timetemp, 0, sizeof(timetemp));
    memset(&stcleanrule, 0, sizeof(struct st_cleanrule));
    //设置一个默认值
    stcleanrule.MaxSize = -1;

    GetXMLBuffer(strbuffer, "TargetDir", stcleanrule.TargetDir);
    GetXMLBuffer(strbuffer, "MatchRule", stcleanrule.MatchRule);
    GetXMLBuffer(strbuffer, "CleanTime", timetemp);
    GetXMLBuffer(strbuffer, "MinNum", &stcleanrule.MinNum);
    GetXMLBuffer(strbuffer, "MaxSize", &stcleanrule.MaxSize);
    stcleanrule.CleanTime = evaluate(timetemp) * 60;

    if (Dir.OpenDir(stcleanrule.TargetDir, stcleanrule.MatchRule, 5000, false, true) == false)
    {
      logfile.Write_P("OpenDir %s failed!\n", stcleanrule.TargetDir);
      continue;
    }

    logfile.Write_P("开始清理目录 %s ...\n", stcleanrule.TargetDir);
    logfile.Write_P("清理规则：%s", strbuffer);

    if (stcleanrule.MaxSize > 1)
    {
      CDir Dirsize;
      if (Dirsize.OpenDir(stcleanrule.TargetDir, stcleanrule.MatchRule, 5000, false, true) == false)
      {
        logfile.Write_P("OpenDir %s failed!\n", stcleanrule.TargetDir);
        continue;
      }

      while (true)
      {
        if (Dirsize.ReadDir() == false)
          break;

        if (Dirsize.m_FileSize > stcleanrule.MaxSize)
        {
          REMOVE(Dirsize.m_FullFileName);
          logfile.Write_P("删除文件 %s\n", Dirsize.m_FullFileName);
        }
      }
    }

    while (true)
    {
      if (Dir.ReadDir() == false || (Dir.m_vFileName.size() - Dir.m_pos) < stcleanrule.MinNum)
        break;

      time_t tnow;
      tnow = time(0) - stcleanrule.CleanTime;
      if (strtotime(Dir.m_AccessTime) < tnow)
      {
        REMOVE(Dir.m_FullFileName);
        logfile.Write_P("删除文件 %s\n", Dir.m_FullFileName);
      }
    }

    logfile.Write_P("清理目录 %s 完成！\n\n", stcleanrule.TargetDir);
  }

  return 0;
}

double evaluate(char expr[])
{
  double numbers[5];
  int nsi = 0;
  char operators[5];
  int osi = 0;
  char numbuf[16];
  int nbi = 0;
  char ch;
  int i = 0;

  while ((ch = expr[i]) != 0)
  {
    if (checknumber(ch))
    {
      numbuf[nbi++] = ch;
      if (!checknumber(expr[i + 1]))
      {
        numbuf[nbi] = 0;
        nbi = 0;
        sscanf(numbuf, "%lf", &numbers[nsi++]);
      }
    }
    else
    {
      while ((osi > 0) && (precedence(ch) <= precedence(operators[osi - 1])))
      {
        numbers[nsi - 2] = calculate(operators[osi - 1], numbers[nsi - 2], numbers[nsi - 1]);
        osi--;
        nsi--;
      }
      operators[osi++] = ch;
    }
    i++;
  }
  while (osi > 0)
  {
    numbers[nsi - 2] = calculate(operators[osi - 1], numbers[nsi - 2], numbers[nsi - 1]);
    osi--;
    nsi--;
  }
  return numbers[0];
}

char checknumber(char ch)
{
  if ((ch >= '0' && ch <= '9') || ch == '.')
    return 1;
  else
    return 0;
}

int precedence(char ch)
{
  int precedence;
  switch (ch)
  {
  case '+':
  case '-':
    precedence = 0;
    break;
  case '*':
  case '/':
    precedence = 1;
    break;
  case '^':
    precedence = 2;
  }
  return precedence;
}

double calculate(char moperator, double num1, double num2)
{
  double result;
  switch (moperator)
  {
  case '+':
    result = num1 + num2;
    break;
  case '-':
    result = num1 - num2;
    break;
  case '*':
    result = num1 * num2;
    break;
  case '/':
    result = num1 / num2;
    break;
  case '^':
    result = pow(num1, num2);
    break;
  default:
    logfile.Write_P("Invalid Operator\n");
    return -1;
  }
  return result;
}

int FileCount(char *Dir, char *MatchRule, bool OpenChild)
{
  int count;
  CDir LocalDir;
  LocalDir.OpenDir(Dir, MatchRule, OpenChild);
  while (true)
  {
    if (LocalDir.ReadDir() == false)
      break;
    count++;
  }

  return count;
}
