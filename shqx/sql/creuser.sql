--把用户试错次数改为无限制
alter profile DEFAULT limit FAILED_LOGIN_ATTEMPTS UNLIMITED;
alter profile DEFAULT limit PASSWORD_LIFE_TIME UNLIMITED;


--数据中心用户的主用户
--drop user shqx cascade;

create user shqx profile default identified by pwdidc default tablespace users account unlock;
grant connect to shqx;
grant dba to shqx;

exit;
