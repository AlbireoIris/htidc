all:deletetables_FR_OR wgetpic_FR incregather_OR_FR deletetablesF_FR_OR tcpputfiles_FR tcpfilesservermpt_FR_MP tcpgetfiles_FR ftpgetfile_FR_FT batchclean_FR exptables_FR_OR HelloVSCode a tcpfilesservermp_FR ftpputfile_FR_FT deletefiles_FR deletefilesR_FR test_FR_OR 

# 警告处理
CFLAGS = -Wno-write-strings -g#-Wno-unused-variable

# CFLAGS = -O2
#CFLAGS = -O2 -Wall

#库文件目录路径
PUBLICHOME = /htidc/public/lib

#可执行程序目录路径
BINHOME = /htidc/public/bin

#备份目录路径
CBAK = /htidc/public/c_bak

#libfreecplus.so的引用
FREECPLUSLIB = -l_freecplus -lcJSON 

#通用库ftp路径
FTPLIB = -l_ftp

# oracle头文件路径
ORACLEHEAD = -I$(ORACLE_HOME)/rdbms/public

# oracle库文件路径
ORACLELIB =  -L$(ORACLE_HOME)/lib -L.

# oracle的OCI库
OCILIB = -lclntsh

#_occi.h和_mylib.h的路径
OCICPP = $(PUBLICHOME)/_ooci.cpp $(PUBLICHOME)/_mylib.cpp

# _freecplus框架整合
IFREECPLUS = $(FREECPLUSLIB)

#_ftp框架整合
IFTP = $(FTPLIB)

#occi框架整合
IOCCI = $(ORACLELIB) $(ORACLEHEAD) $(OCILIB) $(OCICPP)

deletetables_FR_OR:deletetables_FR_OR.cpp
	g++ $(CFLAGS) -o deletetables_FR_OR deletetables_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp deletetables_FR_OR $(BINHOME)/deletetables_FR_OR 
	cp deletetables_FR_OR.cpp $(CBAK)/deletetables_FR_OR.cpp 

wgetpic_FR:wgetpic_FR.cpp
	g++ $(CFLAGS) -o wgetpic_FR wgetpic_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp wgetpic_FR $(BINHOME)/wgetpic_FR 
	cp wgetpic_FR.cpp $(CBAK)/wgetpic_FR.cpp 

incregather_OR_FR:incregather_OR_FR.cpp
	g++ $(CFLAGS) -o incregather_OR_FR incregather_OR_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp incregather_OR_FR $(BINHOME)/incregather_OR_FR 
	cp incregather_OR_FR.cpp $(CBAK)/incregather_OR_FR.cpp 

deletetablesF_FR_OR:deletetablesF_FR_OR.cpp
	g++ $(CFLAGS) -o deletetablesF_FR_OR deletetablesF_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp deletetablesF_FR_OR $(BINHOME)/deletetablesF_FR_OR 
	cp deletetablesF_FR_OR.cpp $(CBAK)/deletetablesF_FR_OR.cpp 

tcpputfiles_FR:tcpputfiles_FR.cpp
	g++ $(CFLAGS) -o tcpputfiles_FR tcpputfiles_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp tcpputfiles_FR $(BINHOME)/tcpputfiles_FR 
	cp tcpputfiles_FR.cpp $(CBAK)/tcpputfiles_FR.cpp 

tcpfilesservermpt_FR_MP:tcpfilesservermpt_FR_MP.cpp
	g++ $(CFLAGS) -o tcpfilesservermpt_FR_MP tcpfilesservermpt_FR_MP.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS) -lpthread 
	cp tcpfilesservermpt_FR_MP $(BINHOME)/tcpfilesservermpt_FR_MP 
	cp tcpfilesservermpt_FR_MP.cpp $(CBAK)/tcpfilesservermpt_FR_MP.cpp 

tcpgetfiles_FR:tcpgetfiles_FR.cpp
	g++ $(CFLAGS) -o tcpgetfiles_FR tcpgetfiles_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp tcpgetfiles_FR $(BINHOME)/tcpgetfiles_FR 
	cp tcpgetfiles_FR.cpp $(CBAK)/tcpgetfiles_FR.cpp 

ftpgetfile_FR_FT:ftpgetfile_FR_FT.cpp
	g++ $(CFLAGS) -o ftpgetfile_FR_FT ftpgetfile_FR_FT.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IFTP)
	cp ftpgetfile_FR_FT $(BINHOME)/ftpgetfile_FR_FT 
	cp ftpgetfile_FR_FT.cpp $(CBAK)/ftpgetfile_FR_FT.cpp 

batchclean_FR:batchclean_FR.cpp
	g++ $(CFLAGS) -o batchclean_FR batchclean_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp batchclean_FR $(BINHOME)/batchclean_FR 
	cp batchclean_FR.cpp $(CBAK)/batchclean_FR.cpp 

exptables_FR_OR:exptables_FR_OR.cpp
	g++ $(CFLAGS) -o exptables_FR_OR exptables_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp exptables_FR_OR $(BINHOME)/exptables_FR_OR 
	cp exptables_FR_OR.cpp $(CBAK)/exptables_FR_OR.cpp 

HelloVSCode:HelloVSCode.cpp
	g++ $(CFLAGS) -o HelloVSCode HelloVSCode.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) 
	cp HelloVSCode $(BINHOME)/HelloVSCode 
	cp HelloVSCode.cpp $(CBAK)/HelloVSCode.cpp 

a:a.cpp
	g++ $(CFLAGS) -o a a.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) 
	cp a $(BINHOME)/a 
	cp a.cpp $(CBAK)/a.cpp 

tcpfilesservermp_FR:tcpfilesservermp_FR.cpp
	g++ $(CFLAGS) -o tcpfilesservermp_FR tcpfilesservermp_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp tcpfilesservermp_FR $(BINHOME)/tcpfilesservermp_FR 
	cp tcpfilesservermp_FR.cpp $(CBAK)/tcpfilesservermp_FR.cpp 

ftpputfile_FR_FT:ftpputfile_FR_FT.cpp
	g++ $(CFLAGS) -o ftpputfile_FR_FT ftpputfile_FR_FT.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IFTP)
	cp ftpputfile_FR_FT $(BINHOME)/ftpputfile_FR_FT 
	cp ftpputfile_FR_FT.cpp $(CBAK)/ftpputfile_FR_FT.cpp 

deletefiles_FR:deletefiles_FR.cpp
	g++ $(CFLAGS) -o deletefiles_FR deletefiles_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp deletefiles_FR $(BINHOME)/deletefiles_FR 
	cp deletefiles_FR.cpp $(CBAK)/deletefiles_FR.cpp 

deletefilesR_FR:deletefilesR_FR.cpp
	g++ $(CFLAGS) -o deletefilesR_FR deletefilesR_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp deletefilesR_FR $(BINHOME)/deletefilesR_FR 
	cp deletefilesR_FR.cpp $(CBAK)/deletefilesR_FR.cpp 

test_FR_OR:test_FR_OR.cpp
	g++ $(CFLAGS) -o test_FR_OR test_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp test_FR_OR $(BINHOME)/test_FR_OR 
	cp test_FR_OR.cpp $(CBAK)/test_FR_OR.cpp 

clean:
	rm -rf deletetables_FR_OR wgetpic_FR incregather_OR_FR deletetablesF_FR_OR tcpputfiles_FR tcpfilesservermpt_FR_MP tcpgetfiles_FR ftpgetfile_FR_FT batchclean_FR exptables_FR_OR HelloVSCode tcpfilesservermp_FR ftpputfile_FR_FT deletefiles_FR deletefilesR_FR test_FR_OR 
