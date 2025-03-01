(module
(import "console" "logString" (func $logString (param i32 i32)))
(import "console" "promptSync" (func $read_f64 (result f64)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Enter a decimal number, or 0 to stop : ")
(data (i32.const 39) "Not quite - you entered the same number! ")
(data (i32.const 80) "Try AGAIN! ")
(data (i32.const 91) "Enter a smaller decimal number : ")
(data (i32.const 124) "You got it right! ")
(data (i32.const 142) "--------------------------------")
(data (i32.const 174) "Lets play again! ")
(func (export "main")
(local $a f64)
(local $b f64)
(local $j i64)
(local $i i64)
i64.const 0
local.set $i
(block $endloop0
(loop $body0
i32.const 0
i32.const 39
call $logString
call $read_f64
local.set $a
local.get $a
f64.const 0
f64.eq
(if
(then
br $endloop0
)
)
(block $endloop1
i64.const 1
local.set $j
(loop $body1
local.get $j
i64.const 5
i64.gt_s
(if 
(then 
 br $endloop1
)
)
i32.const 91
i32.const 33
call $logString
call $read_f64
local.set $b
local.get $b
local.get $a
f64.lt
(if
(then
br $endloop1
)
(else
local.get $b
local.get $a
f64.gt
(if
(then
i32.const 80
i32.const 11
call $logString
)
(else
local.get $b
local.get $a
f64.eq
(if
(then
i32.const 39
i32.const 41
call $logString
)
)
)
)
)
)
local.get $j
i64.const 1
i64.add
local.set $j
br $body1
)
)
i32.const 124
i32.const 18
call $logString
local.get $i
i64.const 1
i64.add
local.set $i
local.get $i
i64.const 3
i64.eq
(if
(then
br $endloop0
)
)
i32.const 174
i32.const 17
call $logString
i32.const 142
i32.const 32
call $logString
br $body0
)
)
)
)