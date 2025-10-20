#!/usr/bin/expect -f
# 사용법 : scp.sh <Local복사대상디렉토리> <Remote복사할위치 예) root@192.168.200.3:/root/work> <password>

set from [lrange $argv 0 0]
set to [lrange $argv 1 1]
set password  [lrange $argv 2 2]

spawn -noecho scp -r $from $to
expect "*?password:*"
send "$password\r"
expect eof
