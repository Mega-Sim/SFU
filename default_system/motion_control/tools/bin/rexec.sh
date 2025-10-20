#!/usr/bin/expect 

#Usage: ./rexec.sh <ip> <session name> <path> <exec file> [args]

set targetip    [lindex $argv 0]
set sessionName [lindex $argv 1]
set procPath    [lindex $argv 2]
set procName    [lindex $argv 3]
set procArgs    [lindex $argv 4]

spawn ssh root@$targetip

expect "*?assword:*" 
send "master123\r";
sleep 0.1;
send "/root/.checkproc $procName\r";

expect "exist" {    
    send "`/root/.attach2screen $sessionName`\r"
    interact
    exit
} "none" {
    send "`/root/.attach2screen $sessionName`\r"
    sleep 0.1
    send "cd $procPath\r"
    sleep 0.1
    send "./$procName $procArgs\r"
    sleep 0.1
}
interact

