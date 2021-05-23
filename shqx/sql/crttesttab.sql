drop table T_TEST;

create table T_TEST
(
  testint1    number(5),
  testint2    number(5),
  testdouble3    number(5,1),
  teststring4    varchar2(50),
  teststring5    varchar2(50),
  testdate6      date,
  testdate7      date,
  primary key(testint1)
);

purge recyclebin; 
exit;
