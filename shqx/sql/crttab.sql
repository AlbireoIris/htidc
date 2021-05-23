drop table T_SURFDATA;
drop table T_OBTCODE;

create table T_OBTCODE
(
  obtid    char(5),
  obtname  varchar2(30),
  provname varchar2(30),
  lat      number(5,2),
  lon      number(8,2),
  height   number(8,2),
  rsts     number(1), -- 1-∆Ù”√ 2-Ω˚”√ 3-π ’œ
  primary key(obtid)
);

create table T_SURFDATA
( 
  obtid      char(5)    not null,
  ddatetime  date       not null,
  t          number(5)  null,
  p          number(6)  null,
  u          number(3)  null,
  wd         number(3)  null,
  wf         number(4)  null,
  r          number(4)  null,
  vis        number(6)  null,
  crttime    date       null,
  keyid      number(15) null,
  primary key(obtid,ddatetime)
);

drop sequence SEQ_SURFDATA;
create sequence SEQ_SURFDATA minvalue 1;

create index IDX_SURFDATA_1 on T_SURFDATA(ddatetime);
create index IDX_SURFDATA_2 on T_SURFDATA(obtid);
create index IDX_SURFDATA_3 on T_SURFDATA(ddatetime,obtid);
create unique index IDX_SURFDATA_4 on T_SURFDATA(keyid);


purge recyclebin; 
exit;
