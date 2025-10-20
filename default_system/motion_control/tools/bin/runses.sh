#!/usr/bin/expect

set sessionName [lindex $argv 0]
set exePath      [lindex $argv 1]
set username    [lindex $argv 2]

spawn screen -S $sessionName
sleep 0.1
send "su $username\r"
expect "*?assword:*" {
send "master123\r";
} "*?암호:*" {
send "master123\r";
}
sleep 0.1;
send "$exePath\r"
sleep 0.1
expect eof 
