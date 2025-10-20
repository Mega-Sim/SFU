#!/usr/bin/expect

#attach to remote session 
# ./rattach.sh <ip> <session name>

set targetip    [lindex $argv 0]
set sessionName [lindex $argv 1]
set argCount    [llength $argv]

spawn ssh root@$targetip

expect "*?assword:*" 
send "master123\r";
sleep 0.1;
if {$argCount > 1} {
  send "`/root/.attach2screen $sessionName`\r"
}
interact


