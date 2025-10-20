#!/usr/bin/expect

set sessionName [lindex $argv 0]

spawn screen -d -r $sessionName

interact

