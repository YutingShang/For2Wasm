(module
(import "console" "log" (func $log (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "console" "promptSync" (func $read (result i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "This program performs summations. Enter 0 to stop.")
(data (i32.const 50) "Add:")
(data (i32.const 54) "Summation =")
(func (export "main")
(local $sum i32)
(local $a i32)
i32.const 0
i32.const 50
call $logString
i32.const 0
local.set $sum
(block $exit0
(loop $body0
i32.const 50
i32.const 4
call $logString
call $read
local.set $a
local.get $a
i32.const 0
i32.eq
(if
(then
br $exit0
)
(else
local.get $sum
local.get $a
i32.add
local.set $sum
)
)
br $body0
)
)
i32.const 54
i32.const 11
call $logString
local.get $sum
call $log
)
)