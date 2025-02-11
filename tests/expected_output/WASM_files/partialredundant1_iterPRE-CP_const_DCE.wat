(module
(import "console" "log" (func $log_i32 (param i32)))
(func (export "main")
(local $_s1 i32)
(local $_s0 i32)
(local $a i32)
(local $b i32)
(local $c i32)
(local $d i32)
(local $e i32)
(local $i i32)
i32.const 2
local.set $a
i32.const 4
local.set $c
i32.const 2
i32.const 2
i32.eq
local.set $_s0
local.get $_s0
(if
(then
i32.const 2
local.set $c
i32.const 3
i32.const 2
i32.add
local.set $_s1
)
(else
i32.const 3
i32.const 4
i32.add
local.set $_s1
local.get $_s1
local.set $a
local.get $_s1
i32.const 2
i32.eq
local.set $_s0
)
)
local.get $_s0
(if
(then
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
i32.const 10
i32.gt_s
(if 
(then 
 br $endloop0
)
)
local.get $_s1
local.set $e
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
)
)
local.get $a
call $log_i32
i32.const 3
call $log_i32
local.get $c
call $log_i32
local.get $_s1
call $log_i32
local.get $e
call $log_i32
)
)