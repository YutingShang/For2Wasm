(module
(import "console" "log" (func $log (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "The answer to the universe is:")
(func (export "main")
(local $b i32)
(local $i i32)
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
i32.const 50000
i32.gt_s
(if 
(then 
 br $endloop0
)
)
i32.const 42
local.set $b
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
i32.const 0
i32.const 30
call $logString
local.get $b
call $log
)
)