# 设置系统环境变量。
source /etc/profile
# 设置oracle用户的环境变量。
source /oracle/.bash_profile
# 执行
/htidc/public/c/batchclean_FR /htidc/public/c/cleanarg.xml /log/publiclog/batchclear_FR.log 
