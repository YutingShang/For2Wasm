(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "console" "promptSync" (func $read (result i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "What's the triangular number of?")
(data (i32.const 32) "TADA here is triangular number of ")
(data (i32.const 66) "=")
(func (export "main")
(local $res i32)
(local $q i32)
i32.const 0
i32.const 32
call $logString
call $read
local.set $q
i32.const 0
local.set $res
(block $endloop0
(loop $body0
local.get $q
i32.const 0
i32.eq
(if
(then
br $endloop0
)
)
local.get $res
local.get $q
i32.add
local.set $res
local.get $q
i32.const 1
i32.sub
local.set $q
br $body0
)
)
i32.const 32
i32.const 34
call $logString
local.get $q
call $log_i32
i32.const 66
i32.const 1
call $logString
local.get $res
call $log_i32
)
)