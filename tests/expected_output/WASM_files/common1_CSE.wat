(module
(import "console" "log" (func $log (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "20 * 25 =")
(func (export "main")
(local $_s2 i32)
(local $_s1 i32)
(local $_s0 i32)
(local $a i32)
(local $b i32)
(local $c i32)
(local $d i32)
(local $y i32)
(local $z i32)
i32.const 20
local.set $y
i32.const 25
local.set $z
local.get $y
local.get $z
i32.mul
local.set $_s2
local.get $_s2
local.set $_s1
local.get $_s1
local.set $_s0
local.get $_s0
local.set $a
local.get $_s0
local.set $b
local.get $_s1
local.set $c
local.get $_s2
local.set $d
i32.const 0
i32.const 9
call $logString
local.get $d
call $log
)
)