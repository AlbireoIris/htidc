all:lib_freecplus.so lib_ftp.so

libftp.so:ftplib.h ftplib.c
	gcc -fPIC -shared -o libftp.so ftplib.c

lib_freecplus.so:_freecplus.h _freecplus.cpp
	g++ -fPIC -shared -o lib_freecplus.so _freecplus.cpp

lib_ftp.so:_ftp.h _ftp.cpp ftplib.c 
	g++ -fPIC -shared -o lib_ftp.so _ftp.cpp ftplib.c

clean:
	rm -f lib_freecplus.so lib_ftp.so