(module
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Never gonna give you up")
(func (export "main")
(local $a i32)
(local $i i32)
(local $j i32)
(local $k i32)
i32.const 0
i32.const 23
call $logString
)
)