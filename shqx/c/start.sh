/htidc/shqx/bin/crtsurfdata /htidc/shqx/ini/stcode.ini /data/shqx/ftp/surfdata /log/shqx/crtsurfdata.log 60 &

/htidc/public/bin/ftpgetfile_FR_FT /log/shqx/ftpgetfile_surfdata.log "<host>119.29.18.109:21</host><mode>pasv</mode><username>oracle</username><password>zhululin123</password><localpath>/data/shqx/sdata/surfdata</localpath><remotepath>/data/shqx/ftp/surfdata</remotepath><matchname>SURF_*.txt</matchname><ptype>1</ptype><remotepathbak>/data/shqx/ftp/surfdatabak</remotepathbak><listfilename>/data/shqx/ftplist/ftpgetfile_surfdata.list</listfilename><okfilename>/data/shqx/ftplist/ftpgetfile_surfdata.xml</okfilename><timetvl>30</timetvl>" &

#/htidc/shqx/bin/psurfdata_old /data/shqx/sdata/surfdata /log/shqx/psurfdata_old.log shqx/pwdidc@snorcl11g_119 10 &
