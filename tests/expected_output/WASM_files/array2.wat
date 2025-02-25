(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "log" (func $log_f32 (param f32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "*")
(data (i32.const 1) "*")
(data (i32.const 2) "*")
(func (export "main")
(local $_tempSwap_f32 f32)
(local $i i32)
(local $j i32)
(local $k i32)
(local $l i32)
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
i32.const 2
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
i32.const 2
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
i32.const 3
i32.gt_s
(if 
(then 
 br $endloop2
)
)
(block $endloop3
i32.const 1
local.set $l
(loop $body3
local.get $l
i32.const 2
i32.gt_s
(if 
(then 
 br $endloop3
)
)
local.get $i
call $log_i32
i32.const 0
i32.const 1
call $logString
local.get $j
call $log_i32
i32.const 1
i32.const 1
call $logString
local.get $k
call $log_i32
i32.const 2
i32.const 1
call $logString
local.get $l
call $log_i32
local.get $i
local.get $j
i32.mul
local.get $k
i32.mul
local.get $l
i32.mul
f32.convert_i32_s
f32.const 1.00009
f32.add
local.set $_tempSwap_f32
local.get $l
i32.const 1
i32.sub
i32.const 12
i32.mul
local.get $k
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.add
local.get $j
i32.const 1
i32.sub
i32.const 2
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 3
i32.add
local.get $_tempSwap_f32
f32.store
local.get $l
i32.const 1
i32.sub
i32.const 12
i32.mul
local.get $k
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.add
local.get $j
i32.const 1
i32.sub
i32.const 2
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 3
i32.add
f32.load
call $log_f32
local.get $l
i32.const 1
i32.add
local.set $l
br $body3
)
)
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
i32.const 3
f32.load
call $log_f32
i32.const 7
f32.load
call $log_f32
i32.const 11
f32.load
call $log_f32
i32.const 15
f32.load
call $log_f32
i32.const 19
f32.load
call $log_f32
i32.const 23
f32.load
call $log_f32
i32.const 27
f32.load
call $log_f32
i32.const 31
f32.load
call $log_f32
i32.const 35
f32.load
call $log_f32
i32.const 39
f32.load
call $log_f32
i32.const 43
f32.load
call $log_f32
i32.const 47
f32.load
call $log_f32
i32.const 51
f32.load
call $log_f32
i32.const 55
f32.load
call $log_f32
i32.const 59
f32.load
call $log_f32
i32.const 63
f32.load
call $log_f32
i32.const 67
f32.load
call $log_f32
i32.const 71
f32.load
call $log_f32
i32.const 75
f32.load
call $log_f32
i32.const 79
f32.load
call $log_f32
i32.const 83
f32.load
call $log_f32
i32.const 87
f32.load
call $log_f32
i32.const 91
f32.load
call $log_f32
i32.const 95
f32.load
call $log_f32
)
)