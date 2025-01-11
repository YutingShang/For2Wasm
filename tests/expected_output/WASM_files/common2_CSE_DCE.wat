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
(local $_s4 i32)
(local $_s3 i32)
(local $_s2 i32)
(local $_s1 i32)
(local $_s0 i32)
(local $flag i32)
(local $y i32)
(local $z i32)
(local $variable i32)
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
local.set $_s1
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
local.set $_s2
local.get $_s2
local.set $_s1
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
local.set $_s2
local.get $_s2
local.set $_s1
i32.const 73
i32.const 13
call $logString
)
)
)
)
local.get $_s1
i32.const 3
i32.add
i32.const 4
i32.add
i32.const 5
i32.add
i32.const 6
i32.add
i32.const 7
i32.add
i32.const 8
i32.add
local.get $variable
i32.add
local.get $_s2
i32.const 3
i32.add
i32.const 4
i32.add
i32.const 5
i32.add
i32.const 6
i32.add
i32.const 7
i32.add
i32.const 8
i32.add
local.get $variable
i32.add
i32.mul
local.set $z
local.get $z
call $log
)
)