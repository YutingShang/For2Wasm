(module
(import "console" "log" (func $log (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "z=")
(func (export "main")
(local $a i32)
(local $b i32)
(local $c i32)
(local $x i32)
(local $y i32)
(local $z i32)
i32.const 3
local.set $z
i32.const 0
i32.const 2
call $logString
local.get $z
call $log
)
)