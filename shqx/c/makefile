all:wgetrain24_FR test_OR_FR PhoneClient_FR autorecord_FR_OR psurfdbtopic_FR_OR psurfpictodb_FR_OR psurfdata_old_FR_OR a wgettemp24_FR 

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

wgetrain24_FR:wgetrain24_FR.cpp
	g++ $(CFLAGS) -g -o wgetrain24_FR wgetrain24_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp wgetrain24_FR $(BINHOME)/wgetrain24_FR 
	cp wgetrain24_FR.cpp $(CBAK)/wgetrain24_FR.cpp 

test_OR_FR:test_OR_FR.cpp
	g++ $(CFLAGS) -g -o test_OR_FR test_OR_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp test_OR_FR $(BINHOME)/test_OR_FR 
	cp test_OR_FR.cpp $(CBAK)/test_OR_FR.cpp 

PhoneClient_FR:PhoneClient_FR.cpp
	g++ $(CFLAGS) -g -o PhoneClient_FR PhoneClient_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp PhoneClient_FR $(BINHOME)/PhoneClient_FR 
	cp PhoneClient_FR.cpp $(CBAK)/PhoneClient_FR.cpp 

autorecord_FR_OR:autorecord_FR_OR.cpp
	g++ $(CFLAGS) -g -o autorecord_FR_OR autorecord_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp autorecord_FR_OR $(BINHOME)/autorecord_FR_OR 
	cp autorecord_FR_OR.cpp $(CBAK)/autorecord_FR_OR.cpp 

psurfdbtopic_FR_OR:psurfdbtopic_FR_OR.cpp
	g++ $(CFLAGS) -g -o psurfdbtopic_FR_OR psurfdbtopic_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp psurfdbtopic_FR_OR $(BINHOME)/psurfdbtopic_FR_OR 
	cp psurfdbtopic_FR_OR.cpp $(CBAK)/psurfdbtopic_FR_OR.cpp 

psurfpictodb_FR_OR:psurfpictodb_FR_OR.cpp
	g++ $(CFLAGS) -g -o psurfpictodb_FR_OR psurfpictodb_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp psurfpictodb_FR_OR $(BINHOME)/psurfpictodb_FR_OR 
	cp psurfpictodb_FR_OR.cpp $(CBAK)/psurfpictodb_FR_OR.cpp 

psurfdata_old_FR_OR:psurfdata_old_FR_OR.cpp
	g++ $(CFLAGS) -g -o psurfdata_old_FR_OR psurfdata_old_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS)  $(IOCCI) 
	cp psurfdata_old_FR_OR $(BINHOME)/psurfdata_old_FR_OR 
	cp psurfdata_old_FR_OR.cpp $(CBAK)/psurfdata_old_FR_OR.cpp 

a:a.cpp
	g++ $(CFLAGS) -g -o a a.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) 
	cp a $(BINHOME)/a 
	cp a.cpp $(CBAK)/a.cpp 

wgettemp24_FR:wgettemp24_FR.cpp
	g++ $(CFLAGS) -g -o wgettemp24_FR wgettemp24_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME)  $(IFREECPLUS) 
	cp wgettemp24_FR $(BINHOME)/wgettemp24_FR 
	cp wgettemp24_FR.cpp $(CBAK)/wgettemp24_FR.cpp 

clean:
	rm -rf wgetrain24_FR test_OR_FR PhoneClient_FR autorecord_FR_OR psurfdbtopic_FR_OR psurfpictodb_FR_OR psurfdata_old_FR_OR wgettemp24_FR 
