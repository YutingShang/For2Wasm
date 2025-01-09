(module
(import "console" "log" (func $log (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "console" "promptSync" (func $read (result i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Enter a positive integer:")
(data (i32.const 25) "Factorial is not defined for negative numbers.")
(data (i32.const 71) " is: ")
(data (i32.const 76) "The factorial of ")
(func (export "main")
(local $num i32)
(local $i i32)
(local $factorial_res i32)
i32.const 0
i32.const 25
call $logString
call $read
local.set $num
local.get $num
i32.const 0
i32.lt_s
(if
(then
i32.const 25
i32.const 46
call $logString
)
(else
i32.const 1
local.set $factorial_res
i32.const 1
local.set $i
(block $endloop0
(loop $body0
local.get $factorial_res
local.get $i
i32.mul
local.set $factorial_res
local.get $i
i32.const 1
i32.add
local.set $i
local.get $i
local.get $num
i32.gt_s
(if
(then
br $endloop0
)
)
br $body0
)
)
i32.const 76
i32.const 17
call $logString
local.get $num
call $log
i32.const 71
i32.const 5
call $logString
local.get $factorial_res
call $log
)
)
)
)