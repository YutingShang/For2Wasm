(module
(import "console" "log" (func $log_f32 (param f32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "a =")
(data (i32.const 3) "b =")
(data (i32.const 6) "c =")
(func (export "main")
(local $_s4 f32)
(local $_s3 f32)
(local $_s2 f32)
(local $_s1 f32)
(local $_s0 f32)
(local $i i32)
(local $a f32)
(local $b f32)
(local $c f32)
(local $x f32)
(local $y f32)
(local $z f32)
(local $k f32)
f32.const 0
f32.const 5.38
f32.sub
local.set $x
f32.const 6.6892
local.set $y
f32.const 10.000000000001
local.set $z
f32.const 3.0
local.set $k
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
i32.const 1000000000
i32.gt_s
(if 
(then 
 br $endloop0
)
)
local.get $x
f32.const 6.6892
f32.mul
local.set $_s0
local.get $_s0
f32.const 3.0
f32.div
local.set $_s2
f32.const 6.6892
f32.const 10.000000000001
f32.mul
local.set $_s1
local.get $_s1
local.get $x
f32.div
local.set $_s3
local.get $_s2
local.get $_s3
f32.mul
local.set $_s4
local.get $_s4
local.set $a
local.get $_s4
local.set $b
local.get $_s4
local.set $c
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
i32.const 0
i32.const 3
call $logString
local.get $a
call $log_f32
i32.const 3
i32.const 3
call $logString
local.get $b
call $log_f32
i32.const 6
i32.const 3
call $logString
local.get $c
call $log_f32
)
)