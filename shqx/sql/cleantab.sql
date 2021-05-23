truncate table T_SURFDATA;
truncate table T_OBTCODE;
truncate table T_OBTINFO;
truncate table T_OBTSIGNAL;
truncate table T_LPDATARECORD_R;
truncate table T_CHBST_H


drop sequence SEQ_SURFDATA;
create sequence SEQ_SURFDATA increment by 1 start with 1 ;


drop sequence SEQ_SIGNAL;
create sequence SEQ_SIGNAL increment by 1 start with 1 ;

purge recyclebin;



exit; 
