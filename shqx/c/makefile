all:wgetrain24_FR test_OR_FR PhoneClient_FR autorecord_FR_OR psurfdbtopic_FR_OR psurfpictodb_FR_OR psurfdata_old_FR_OR a wgettemp24_FR 

# 警告处理
CFLAGS = -Wno-write-strings -g#-Wno-unused-variable

# CFLAGS = -O2
#CFLAGS = -O2 -Wall

#库文件目录路径
PUBLICHOME = /htidc/public/lib

#可执行程序目录路径
BINHOME = /htidc/shqx/bin

#备份目录路径
CBAK = /htidc/shqx/c_bak

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

wgetrain24_FR:wgetrain24_FR.cpp
	g++ $(CFLAGS) -o wgetrain24_FR wgetrain24_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp wgetrain24_FR $(BINHOME)/wgetrain24_FR 
	cp wgetrain24_FR.cpp $(CBAK)/wgetrain24_FR.cpp 

test_OR_FR:test_OR_FR.cpp
	g++ $(CFLAGS) -o test_OR_FR test_OR_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp test_OR_FR $(BINHOME)/test_OR_FR 
	cp test_OR_FR.cpp $(CBAK)/test_OR_FR.cpp 

PhoneClient_FR:PhoneClient_FR.cpp
	g++ $(CFLAGS) -o PhoneClient_FR PhoneClient_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp PhoneClient_FR $(BINHOME)/PhoneClient_FR 
	cp PhoneClient_FR.cpp $(CBAK)/PhoneClient_FR.cpp 

autorecord_FR_OR:autorecord_FR_OR.cpp
	g++ $(CFLAGS) -o autorecord_FR_OR autorecord_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp autorecord_FR_OR $(BINHOME)/autorecord_FR_OR 
	cp autorecord_FR_OR.cpp $(CBAK)/autorecord_FR_OR.cpp 

psurfdbtopic_FR_OR:psurfdbtopic_FR_OR.cpp
	g++ $(CFLAGS) -o psurfdbtopic_FR_OR psurfdbtopic_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp psurfdbtopic_FR_OR $(BINHOME)/psurfdbtopic_FR_OR 
	cp psurfdbtopic_FR_OR.cpp $(CBAK)/psurfdbtopic_FR_OR.cpp 

psurfpictodb_FR_OR:psurfpictodb_FR_OR.cpp
	g++ $(CFLAGS) -o psurfpictodb_FR_OR psurfpictodb_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp psurfpictodb_FR_OR $(BINHOME)/psurfpictodb_FR_OR 
	cp psurfpictodb_FR_OR.cpp $(CBAK)/psurfpictodb_FR_OR.cpp 

psurfdata_old_FR_OR:psurfdata_old_FR_OR.cpp
	g++ $(CFLAGS) -o psurfdata_old_FR_OR psurfdata_old_FR_OR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)$(IOCCI)
	cp psurfdata_old_FR_OR $(BINHOME)/psurfdata_old_FR_OR 
	cp psurfdata_old_FR_OR.cpp $(CBAK)/psurfdata_old_FR_OR.cpp 

a:a.cpp
	g++ $(CFLAGS) -o a a.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) 
	cp a $(BINHOME)/a 
	cp a.cpp $(CBAK)/a.cpp 

wgettemp24_FR:wgettemp24_FR.cpp
	g++ $(CFLAGS) -o wgettemp24_FR wgettemp24_FR.cpp -lm -lc -I$(PUBLICHOME) -L$(PUBLICHOME) $(IFREECPLUS)
	cp wgettemp24_FR $(BINHOME)/wgettemp24_FR 
	cp wgettemp24_FR.cpp $(CBAK)/wgettemp24_FR.cpp 

clean:
	rm -rf wgetrain24_FR test_OR_FR PhoneClient_FR autorecord_FR_OR psurfdbtopic_FR_OR psurfpictodb_FR_OR psurfdata_old_FR_OR wgettemp24_FR 
