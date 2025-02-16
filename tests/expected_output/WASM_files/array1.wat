(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "console" "promptSync" (func $read (result i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Array z is")
(data (i32.const 10) "Array y in row major order is")
(data (i32.const 39) "Array z is now")
(data (i32.const 53) "Setting z(3) to 10")
(data (i32.const 71) "The array y in column major order is ")
(data (i32.const 108) "The array y is now")
(data (i32.const 126) "Enter a value to store in y(1, 2)")
(data (i32.const 159) "\01\00\00\00\02\00\00\00\03\00\00\00\04\00\00\00\05\00\00\00")
(data (i32.const 179) "\01\00\00\00\00\00\00\00\01\00\00\00\fc\ff\ff\ff\06\00\00\00\09\00\00\00")
(func (export "main")
(local $i i32)
(local $j i32)
(local $k i32)
i32.const 0
i32.const 10
call $logString
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
i32.const 5
i32.gt_s
(if 
(then 
 br $endloop0
)
)
local.get $i
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 159
i32.add
local.get $i
i32.store
local.get $i
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 159
i32.add
i32.load
call $log_i32
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
i32.const 53
i32.const 18
call $logString
i32.const 3
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 159
i32.add
i32.const 10
i32.store
i32.const 39
i32.const 14
call $logString
i32.const 159
i32.load
call $log_i32
i32.const 163
i32.load
call $log_i32
i32.const 167
i32.load
call $log_i32
i32.const 171
i32.load
call $log_i32
i32.const 175
i32.load
call $log_i32
i32.const 10
i32.const 29
call $logString
(block $endloop1
i32.const 1
local.set $i
(loop $body1
local.get $i
i32.const 2
i32.gt_s
(if 
(then 
 br $endloop1
)
)
(block $endloop2
i32.const 1
local.set $j
(loop $body2
local.get $j
i32.const 3
i32.gt_s
(if 
(then 
 br $endloop2
)
)
local.get $j
i32.const 1
i32.sub
i32.const 2
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 179
i32.add
i32.load
call $log_i32
local.get $j
i32.const 1
i32.add
local.set $j
br $body2
)
)
local.get $i
i32.const 1
i32.add
local.set $i
br $body1
)
)
i32.const 71
i32.const 37
call $logString
i32.const 179
i32.load
call $log_i32
i32.const 183
i32.load
call $log_i32
i32.const 187
i32.load
call $log_i32
i32.const 191
i32.load
call $log_i32
i32.const 195
i32.load
call $log_i32
i32.const 199
i32.load
call $log_i32
i32.const 126
i32.const 33
call $logString
call $read
local.set $k
i32.const 2
i32.const 1
i32.sub
i32.const 2
i32.mul
i32.const 1
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 179
i32.add
local.get $k
i32.store
i32.const 108
i32.const 18
call $logString
i32.const 179
i32.load
call $log_i32
i32.const 183
i32.load
call $log_i32
i32.const 187
i32.load
call $log_i32
i32.const 191
i32.load
call $log_i32
i32.const 195
i32.load
call $log_i32
i32.const 199
i32.load
call $log_i32
)
)