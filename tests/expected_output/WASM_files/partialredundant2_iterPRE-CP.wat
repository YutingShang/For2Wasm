(module
(import "console" "log" (func $log (param i32)))
(func (export "main")
(local $_s4 i32)
(local $_s3 i32)
(local $_s2 i32)
(local $_s1 i32)
(local $_s0 i32)
(local $a i32)
(local $b i32)
(local $x i32)
(local $y i32)
(local $z i32)
(local $k i32)
i32.const 5
local.set $x
i32.const 6
local.set $y
i32.const 10
local.set $z
i32.const 3
local.set $k
local.get $a
i32.const 2
i32.eq
(if
(then
local.get $x
local.get $y
i32.mul
local.set $_s0
local.get $_s0
local.get $k
i32.div_s
local.set $_s2
local.get $y
local.get $z
i32.mul
local.set $_s1
local.get $_s1
local.get $x
i32.div_s
local.set $_s3
local.get $_s2
local.get $_s3
i32.mul
local.set $_s4
local.get $_s4
local.set $a
)
(else
local.get $x
local.get $y
i32.mul
local.set $_s0
local.get $y
local.get $z
i32.mul
local.set $_s1
local.get $_s0
local.get $k
i32.div_s
local.set $_s2
local.get $_s1
local.get $x
i32.div_s
local.set $_s3
local.get $_s2
local.get $_s3
i32.mul
local.set $_s4
)
)
local.get $_s4
local.set $b
local.get $a
call $log
local.get $_s4
call $log
)
)