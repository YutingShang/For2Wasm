(module
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Never gonna give you up")
(func (export "main")
i32.const 0
i32.const 23
call $logString
)
)