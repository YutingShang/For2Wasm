(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "7^7 / 6^6 = ")
(func (export "main")
(local $_s12 i32)
(local $_s11 i32)
(local $_s10 i32)
(local $_s9 i32)
(local $_s8 i32)
(local $_s7 i32)
(local $_s6 i32)
(local $_s5 i32)
(local $_s4 i32)
(local $_s3 i32)
(local $_s2 i32)
(local $_s1 i32)
(local $_s0 i32)
(local $z i32)
(local $p i32)
(local $q i32)
(local $a i32)
(local $y i32)
(local $i i32)
(local $terminate i32)
i32.const 7
local.set $p
i32.const 6
local.set $q
i32.const 1
local.set $terminate
(block $endloop0
i32.const 1
local.set $i
local.get $p
local.get $p
i32.mul
local.set $_s1
local.get $q
local.get $q
i32.mul
local.set $_s2
local.get $_s1
local.get $p
i32.mul
local.set $_s3
local.get $_s2
local.get $q
i32.mul
local.set $_s4
local.get $_s3
local.get $p
i32.mul
local.set $_s5
local.get $_s4
local.get $q
i32.mul
local.set $_s6
local.get $_s5
local.get $p
i32.mul
local.set $_s7
local.get $_s6
local.get $q
i32.mul
local.set $_s8
local.get $_s7
local.get $p
i32.mul
local.set $_s9
local.get $_s8
local.get $q
i32.mul
local.set $_s10
local.get $_s9
local.get $p
i32.mul
local.set $_s11
local.get $_s11
local.get $_s10
i32.div_s
local.set $_s12
(loop $body0
local.get $i
i32.const 6
i32.gt_s
(if 
(then 
 br $endloop0
)
)
i32.const 0
local.set $a
local.get $a
i32.const 1000000000
i32.eq
local.set $_s0
(block $endloop1
(loop $body1
local.get $_s12
local.set $z
local.get $_s0
(if
(then
br $endloop1
)
)
local.get $_s12
local.set $y
local.get $terminate
i32.const 1
i32.eq
(if
(then
local.get $a
i32.const 1
i32.add
local.set $a
local.get $a
i32.const 1000000000
i32.eq
local.set $_s0
)
)
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
local.get $_s12
local.set $z
i32.const 0
i32.const 12
call $logString
local.get $_s12
call $log_i32
local.get $y
call $log_i32
)
)