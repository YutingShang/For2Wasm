(module
(import "console" "log" (func $log_i32 (param i32)))
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
local.get $a
i32.const 2
i32.eq
(if
(then
i32.const 5
i32.const 6
i32.mul
local.set $_s0
local.get $_s0
i32.const 3
i32.div_s
local.set $_s2
i32.const 6
i32.const 10
i32.mul
local.set $_s1
local.get $_s1
i32.const 5
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
i32.const 5
i32.const 6
i32.mul
local.set $_s0
i32.const 6
i32.const 10
i32.mul
local.set $_s1
local.get $_s0
i32.const 3
i32.div_s
local.set $_s2
local.get $_s1
i32.const 5
i32.div_s
local.set $_s3
local.get $_s2
local.get $_s3
i32.mul
local.set $_s4
)
)
local.get $a
call $log_i32
local.get $_s4
call $log_i32
)
)