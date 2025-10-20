#!/usr/bin/expect 

# USAGE: ./settargettime.sh <target ip> <설정시간 string. date --set 옵션에 준하게>

set target [lindex $argv 0]
set timestring [lindex $argv 1]

spawn ssh root@$target

expect "*?assword:*" { 
    send "master123\r"    
    exp_continue
} "Last login" { 
    sleep 0.1       
    send "date --set=\"$timestring\"\r"
    sleep 0.1
    send "hwclock -w && which which\r"    
    exp_continue
} "/usr/bin/which"
