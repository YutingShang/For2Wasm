(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "console" "promptSync" (func $read (result i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) " (3 > 5) AND (7 < 10) =")
(data (i32.const 23) "Enter option 0 or 1 :")
(data (i32.const 44) " (3 > 5) AND (7 < 10) =")
(data (i32.const 67) " (3 > 2) AND (7 < 10) =")
(func (export "main")
(local $_s0 i32)
(local $P i32)
(local $Q i32)
(local $R i32)
(local $option i32)
i32.const 3
i32.const 5
i32.gt_s
local.set $P
i32.const 7
i32.const 10
i32.lt_s
local.set $Q
local.get $P
local.get $Q
i32.and
local.set $R
i32.const 0
i32.const 23
call $logString
local.get $R
call $log_i32
i32.const 23
i32.const 21
call $logString
call $read
local.set $option
local.get $option
i32.const 1
i32.eq
local.set $_s0
local.get $_s0
(if
(then
i32.const 3
i32.const 2
i32.gt_s
local.set $P
)
)
local.get $P
local.get $Q
i32.and
local.set $R
local.get $_s0
(if
(then
i32.const 67
i32.const 23
call $logString
local.get $R
call $log_i32
)
(else
i32.const 44
i32.const 23
call $logString
local.get $R
call $log_i32
)
)
)
)