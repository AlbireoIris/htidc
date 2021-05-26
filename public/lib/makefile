all:libcftp.so libcJSON.so lib_ftp.so lib_json.so lib_freecplus.so

#编译由c写成的cftp库
libcftp.so:ftplib.h ftplib.c
	gcc -fPIC -shared -o libcftp.so ftplib.c

#编译由c写成的cJSON库
libcJSON.so:cJSON.h cJSON.c
	gcc -fPIC -shared -o libcJSON.so  cJSON.c

#编译_ftp库，使用时必须和cftp库一同使用
lib_ftp.so:_ftp.h _ftp.cpp
	g++ -fPIC -shared -o lib_ftp.so _ftp.cpp

#编译_json库，使用时必须和cJSON库一起使用
lib_json.so:_json.h _json.cpp
	g++ -fPIC -shared -o lib_json.so _json.cpp

#编译_freecplus的库
lib_freecplus.so:_freecplus.h _freecplus.cpp
	g++ -fPIC -shared -o lib_freecplus.so _freecplus.cpp

##编译_ooci的库，需要搭配orac官方的oci库一起使用
#lib_ooci.so:_ooci.h _ooci.cpp
#	g++ -fPIC -shared -o lib_ooci.so _ooci.cpp

##编译_mylib的库，需要搭配_freecplus.so和_ooci.so一起使用
#lib_mylib.so:_mylib.h _mylib.cpp
#	g++ -fPIC -shared -o lib_mylib.so _mylib.cpp

#一些clean的工作
clean:
	rm -rf lib_ftp.so lib_json.so libcftp.so libcJSON.so lib_freecplus.so
 

	

#lib_freecplus.so:_freecplus.h _freecplus.cpp
#	g++ -fPIC -shared -o lib_freecplus.so _freecplus.cpp
#
#lib_ftp.so:_ftp.h _ftp.cpp ftplib.c 
#	g++ -fPIC -shared -o lib_ftp.so _ftp.cpp ftplib.c

