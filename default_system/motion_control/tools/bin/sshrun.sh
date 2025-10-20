#!/usr/bin/expect -f
# 사용법 : sshrun.sh <user@target> <password> <cmd> ...

set target [lindex $argv 0]
set password  [lindex $argv 1]
set cmd [lrange $argv 2 2]

spawn ssh $target $cmd [lindex $argv 3] [lindex $argv 4]  \
    [lindex $argv 5]  [lindex $argv 6]  [lindex $argv 7] \
    [lindex $argv 8]  [lindex $argv 9]  [lindex $argv 10] \
    [lindex $argv 11]  [lindex $argv 12]  [lindex $argv 13] \
    [lindex $argv 14]  [lindex $argv 15]  [lindex $argv 16] 
    
expect "*?assword:*"
send "$password\r"
expect eof


