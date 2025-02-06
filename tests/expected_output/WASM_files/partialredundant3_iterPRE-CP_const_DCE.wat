(module
(import "console" "log" (func $log (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "2+3 = ")
(func (export "main")
(local $_s0 i32)
(local $sum i32)
(local $a i32)
(local $b i32)
(local $x i32)
(local $y i32)
(local $i i32)
(block $endloop0
i32.const 1
local.set $i
i32.const 2
i32.const 3
i32.add
local.set $_s0
(loop $body0
local.get $i
i32.const 10
i32.gt_s
(if 
(then 
 br $endloop0
)
)
i32.const 3
i32.const 3
i32.add
local.set $_s0
local.get $_s0
local.set $b
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
i32.const 0
i32.const 6
call $logString
local.get $_s0
call $log
local.get $b
call $log
)
)