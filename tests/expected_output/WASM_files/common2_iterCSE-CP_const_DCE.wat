(module
(import "console" "log" (func $log (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "console" "promptSync" (func $read (result i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Enter 1 or 2 to determine which branch is executed")
(data (i32.const 50) "(1+2+3+4+5+6+7+8) ^ 2 =")
(data (i32.const 73) "Invalid input")
(data (i32.const 86) "(1+2+3+4+5+6+7+8+9) ^ 2 =")
(func (export "main")
(local $_s32 i32)
(local $_s31 i32)
(local $_s30 i32)
(local $_s29 i32)
(local $_s28 i32)
(local $_s27 i32)
(local $_s26 i32)
(local $_s25 i32)
(local $_s24 i32)
(local $_s23 i32)
(local $_s22 i32)
(local $_s21 i32)
(local $_s20 i32)
(local $_s19 i32)
(local $_s18 i32)
(local $_s17 i32)
(local $_s16 i32)
(local $_s15 i32)
(local $_s14 i32)
(local $_s13 i32)
(local $_s12 i32)
(local $_s11 i32)
(local $_s10 i32)
(local $_s9 i32)
(local $_s8 i32)
(local $_s7 i32)
(local $_s6 i32)
(local $_s5 i32)
(local $_s4 i32)
(local $_s3 i32)
(local $_s2 i32)
(local $_s1 i32)
(local $_s0 i32)
(local $flag i32)
(local $y i32)
(local $z i32)
(local $variable i32)
(local $temp i32)
i32.const 0
i32.const 50
call $logString
call $read
local.set $flag
local.get $flag
i32.const 1
i32.eq
(if
(then
i32.const 0
local.set $variable
i32.const 1
i32.const 2
i32.add
local.set $_s2
local.get $_s2
i32.const 3
i32.add
local.set $_s5
local.get $_s5
i32.const 4
i32.add
local.set $_s9
local.get $_s9
i32.const 5
i32.add
local.set $_s13
local.get $_s13
i32.const 6
i32.add
local.set $_s17
local.get $_s17
i32.const 7
i32.add
local.set $_s21
local.get $_s21
i32.const 8
i32.add
local.set $_s25
local.get $_s25
i32.const 0
i32.add
local.set $_s29
local.get $_s29
local.get $_s29
i32.mul
local.set $y
local.get $y
local.set $temp
i32.const 50
i32.const 23
call $logString
)
(else
local.get $flag
i32.const 2
i32.eq
(if
(then
i32.const 9
local.set $variable
i32.const 1
i32.const 2
i32.add
local.set $_s3
local.get $_s3
i32.const 3
i32.add
local.set $_s7
local.get $_s7
i32.const 4
i32.add
local.set $_s11
local.get $_s11
i32.const 5
i32.add
local.set $_s15
local.get $_s15
i32.const 6
i32.add
local.set $_s19
local.get $_s19
i32.const 7
i32.add
local.set $_s23
local.get $_s23
i32.const 8
i32.add
local.set $_s27
local.get $_s27
i32.const 9
i32.add
local.set $_s31
local.get $_s3
local.set $_s2
local.get $_s31
local.get $_s31
i32.mul
local.set $y
local.get $y
local.set $temp
i32.const 86
i32.const 25
call $logString
)
(else
i32.const 1
i32.const 2
i32.add
local.set $_s4
local.get $_s4
i32.const 3
i32.add
local.set $_s8
local.get $_s8
i32.const 4
i32.add
local.set $_s12
local.get $_s12
i32.const 5
i32.add
local.set $_s16
local.get $_s16
i32.const 6
i32.add
local.set $_s20
local.get $_s20
i32.const 7
i32.add
local.set $_s24
local.get $_s24
i32.const 8
i32.add
local.set $_s28
local.get $_s28
i32.const 100
i32.add
local.set $_s32
local.get $_s4
local.set $_s2
local.get $_s32
local.get $_s32
i32.mul
local.set $z
local.get $z
local.set $temp
i32.const 73
i32.const 13
call $logString
i32.const 10
local.set $variable
)
)
)
)
local.get $temp
call $log
local.get $_s2
i32.const 3
i32.add
local.set $_s6
local.get $_s6
i32.const 4
i32.add
local.set $_s10
local.get $_s10
i32.const 5
i32.add
local.set $_s14
local.get $_s14
i32.const 6
i32.add
local.set $_s18
local.get $_s18
i32.const 7
i32.add
local.set $_s22
local.get $_s22
i32.const 8
i32.add
local.set $_s26
local.get $_s26
local.get $variable
i32.add
local.set $_s30
local.get $_s30
local.get $_s30
i32.mul
local.set $z
local.get $z
call $log
)
)