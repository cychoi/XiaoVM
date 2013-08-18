#!/usr/bin/env tclsh
package require Tcl 8.6
foreach file $argv {
    set f [open $file r]
    set fo [open $file.xbc w]
    set contents [read $f]
    foreach line [split $contents \n] {
        set cmd [lindex [split $line { }] 0]
        if { $cmd == "" } continue
        switch -exact -- $cmd {
            ";" {
                continue }
            LOADSTRING {
                set str [lrange [split $line { }] 1 end]
                puts -nonewline $fo "\x10"
                puts -nonewline $fo [format %c [string length $str]]
                puts -nonewline $fo $str }
            LOADSPECIAL {
                set specialname [lindex [split $line { }] 1]
                set special {}
                puts -nonewline $fo "\x11"
                switch -exact -- $specialname {
                    STOP {
                       set special "\x00" } }
                puts -nonewline $fo $special }
            LOADNUMBER {
                set number [lindex [split $line { }] 1]
                puts -nonewline $fo "\x12"
                puts -nonewline $fo [format %c $number] }
            CALL {
                set name [lindex [split $line { }] 1]
                puts -nonewline $fo "\x05"
                puts -nonewline $fo "$name\x00" }
            default {
                error "No such command - $cmd" } } }
    close $f
    close $fo }
