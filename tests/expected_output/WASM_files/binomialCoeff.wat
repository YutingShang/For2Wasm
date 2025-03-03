(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "log_i64" (func $log_i64 (param i64)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Starting binomial coefficient calculation")
(data (i32.const 41) "C(")
(data (i32.const 43) "k =")
(data (i32.const 46) "n =")
(data (i32.const 49) ",")
(data (i32.const 50) "Calculation complete")
(data (i32.const 70) ") =")
(func (export "main")
(local $n i32)
(local $k i32)
(local $i i32)
(local $trials i32)
(local $j i32)
(local $res i64)
i32.const 20
local.set $n
i32.const 10
local.set $k
i32.const 1000
local.set $trials
i32.const 0
i32.const 41
call $logString
i32.const 46
i32.const 3
call $logString
local.get $n
call $log_i32
i32.const 43
i32.const 3
call $logString
local.get $k
call $log_i32
(block $endloop0
i32.const 1
local.set $j
(loop $body0
local.get $j
local.get $trials
i32.gt_s
(if 
(then 
 br $endloop0
)
)
i64.const 1
local.set $res
(block $endloop1
i32.const 1
local.set $i
(loop $body1
local.get $i
local.get $k
i32.gt_s
(if 
(then 
 br $endloop1
)
)
local.get $n
local.get $i
i32.sub
i32.const 1
i32.add
i64.extend_i32_s
local.get $res
i64.mul
local.get $i
i64.extend_i32_s
i64.div_s
local.set $res
local.get $i
i32.const 1
i32.add
local.set $i
br $body1
)
)
local.get $j
i32.const 1
i32.add
local.set $j
br $body0
)
)
i32.const 41
i32.const 2
call $logString
local.get $n
call $log_i32
i32.const 49
i32.const 1
call $logString
local.get $k
call $log_i32
i32.const 70
i32.const 3
call $logString
local.get $res
call $log_i64
i32.const 50
i32.const 20
call $logString
)
)