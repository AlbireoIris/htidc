all:deletetables_FR_OR wgetpic_FR incregather_OR_FR AssembleJson_FR_OR deletetablesF_FR_OR tcpputfiles_FR tcpfilesservermpt_FR_MP tcpgetfiles_FR ftpgetfile_FR_FT batchclean_FR exptables_FR_OR a tcpfilesservermp_FR ftpputfile_FR_FT deletefiles_FR deletefilesR_FR 

# 警告忽略处理
CFLAGS = -Wno-write-strings -Wno-unused-variable

# 2级优化，显示所有警告
# CFLAGS = -O2 -Wall

#库文件目录路径
PUBLICHOME = /htidc/public/lib

#可执行程序目录路径
BINHOME = ../bin

#备份目录路径
CBAK = ../csrcbak

#freecplus相关动态库的路径
FREECPLUSLIB = -l_freecplus -l_json -lcJSON 

#ftp相关动态库的路径
FTPLIB = -l_ftp -lcftp 

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
	g++ $(CFLAGS) -g -o deletetables_FR_OR deletetables_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp deletetables_FR_OR $(BINHOME)/deletetables_FR_OR 
	cp deletetables_FR_OR.cpp $(CBAK)/deletetables_FR_OR.cpp 

wgetpic_FR:wgetpic_FR.cpp
	g++ $(CFLAGS) -g -o wgetpic_FR wgetpic_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp wgetpic_FR $(BINHOME)/wgetpic_FR 
	cp wgetpic_FR.cpp $(CBAK)/wgetpic_FR.cpp 

incregather_OR_FR:incregather_OR_FR.cpp
	g++ $(CFLAGS) -g -o incregather_OR_FR incregather_OR_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp incregather_OR_FR $(BINHOME)/incregather_OR_FR 
	cp incregather_OR_FR.cpp $(CBAK)/incregather_OR_FR.cpp 

AssembleJson_FR_OR:AssembleJson_FR_OR.cpp
	g++ $(CFLAGS) -g -o AssembleJson_FR_OR AssembleJson_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) -l_json -lcJSON
	cp AssembleJson_FR_OR $(BINHOME)/AssembleJson_FR_OR 
	cp AssembleJson_FR_OR.cpp $(CBAK)/AssembleJson_FR_OR.cpp 

deletetablesF_FR_OR:deletetablesF_FR_OR.cpp
	g++ $(CFLAGS) -g -o deletetablesF_FR_OR deletetablesF_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp deletetablesF_FR_OR $(BINHOME)/deletetablesF_FR_OR 
	cp deletetablesF_FR_OR.cpp $(CBAK)/deletetablesF_FR_OR.cpp 

tcpputfiles_FR:tcpputfiles_FR.cpp
	g++ $(CFLAGS) -g -o tcpputfiles_FR tcpputfiles_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp tcpputfiles_FR $(BINHOME)/tcpputfiles_FR 
	cp tcpputfiles_FR.cpp $(CBAK)/tcpputfiles_FR.cpp 

tcpfilesservermpt_FR_MP:tcpfilesservermpt_FR_MP.cpp
	g++ $(CFLAGS) -g -o tcpfilesservermpt_FR_MP tcpfilesservermpt_FR_MP.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  -lpthread 
	cp tcpfilesservermpt_FR_MP $(BINHOME)/tcpfilesservermpt_FR_MP 
	cp tcpfilesservermpt_FR_MP.cpp $(CBAK)/tcpfilesservermpt_FR_MP.cpp 

tcpgetfiles_FR:tcpgetfiles_FR.cpp
	g++ $(CFLAGS) -g -o tcpgetfiles_FR tcpgetfiles_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp tcpgetfiles_FR $(BINHOME)/tcpgetfiles_FR 
	cp tcpgetfiles_FR.cpp $(CBAK)/tcpgetfiles_FR.cpp 

ftpgetfile_FR_FT:ftpgetfile_FR_FT.cpp
	g++ $(CFLAGS) -g -o ftpgetfile_FR_FT ftpgetfile_FR_FT.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IFTP) 
	cp ftpgetfile_FR_FT $(BINHOME)/ftpgetfile_FR_FT 
	cp ftpgetfile_FR_FT.cpp $(CBAK)/ftpgetfile_FR_FT.cpp 

batchclean_FR:batchclean_FR.cpp
	g++ $(CFLAGS) -g -o batchclean_FR batchclean_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp batchclean_FR $(BINHOME)/batchclean_FR 
	cp batchclean_FR.cpp $(CBAK)/batchclean_FR.cpp 

exptables_FR_OR:exptables_FR_OR.cpp
	g++ $(CFLAGS) -g -o exptables_FR_OR exptables_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp exptables_FR_OR $(BINHOME)/exptables_FR_OR 
	cp exptables_FR_OR.cpp $(CBAK)/exptables_FR_OR.cpp 

a:a.cpp
	g++ $(CFLAGS) -g -o a a.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) 
	cp a $(BINHOME)/a 
	cp a.cpp $(CBAK)/a.cpp 

tcpfilesservermp_FR:tcpfilesservermp_FR.cpp
	g++ $(CFLAGS) -g -o tcpfilesservermp_FR tcpfilesservermp_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp tcpfilesservermp_FR $(BINHOME)/tcpfilesservermp_FR 
	cp tcpfilesservermp_FR.cpp $(CBAK)/tcpfilesservermp_FR.cpp 

ftpputfile_FR_FT:ftpputfile_FR_FT.cpp
	g++ $(CFLAGS) -g -o ftpputfile_FR_FT ftpputfile_FR_FT.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IFTP) 
	cp ftpputfile_FR_FT $(BINHOME)/ftpputfile_FR_FT 
	cp ftpputfile_FR_FT.cpp $(CBAK)/ftpputfile_FR_FT.cpp 

deletefiles_FR:deletefiles_FR.cpp
	g++ $(CFLAGS) -g -o deletefiles_FR deletefiles_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp deletefiles_FR $(BINHOME)/deletefiles_FR 
	cp deletefiles_FR.cpp $(CBAK)/deletefiles_FR.cpp 

deletefilesR_FR:deletefilesR_FR.cpp
	g++ $(CFLAGS) -g -o deletefilesR_FR deletefilesR_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp deletefilesR_FR $(BINHOME)/deletefilesR_FR 
	cp deletefilesR_FR.cpp $(CBAK)/deletefilesR_FR.cpp 

clean:
	rm -rf deletetables_FR_OR wgetpic_FR incregather_OR_FR AssembleJson_FR_OR deletetablesF_FR_OR tcpputfiles_FR tcpfilesservermpt_FR_MP tcpgetfiles_FR ftpgetfile_FR_FT batchclean_FR exptables_FR_OR tcpfilesservermp_FR ftpputfile_FR_FT deletefiles_FR deletefilesR_FR 
