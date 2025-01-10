(module
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Never gonna give you up")
(func (export "main")
(local $a i32)
(local $i i32)
(local $j i32)
(local $k i32)
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
i32.const 10000
i32.gt_s
(if 
(then 
 br $endloop0
)
)
(block $endloop1
i32.const 1
local.set $j
(loop $body1
local.get $j
i32.const 1000
i32.gt_s
(if 
(then 
 br $endloop1
)
)
(block $endloop2
i32.const 1
local.set $k
(loop $body2
local.get $k
i32.const 1000
i32.gt_s
(if 
(then 
 br $endloop2
)
)
i32.const 3
local.set $a
local.get $k
i32.const 1
i32.add
local.set $k
br $body2
)
)
local.get $j
i32.const 1
i32.add
local.set $j
br $body1
)
)
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
i32.const 0
i32.const 23
call $logString
)
)