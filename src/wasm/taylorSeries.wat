(module
(import "console" "log" (func $log_f64 (param f64)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "Calculating Taylor Series for a value of x around 0")
(data (i32.const 51) "sin(x) = ")
(data (i32.const 60) "e^x = ")
(data (i32.const 66) "For x = ")
(data (i32.const 74) "cos(x) = ")
(data (i32.const 83) "arctan(x) = ")
(data (i32.const 95) "ln(x) = ")
(func (export "main")
(local $_tempSwap_f64 f64)
(local $_s2 f64)
(local $_s1 i32)
(local $_s0 f64)
(local $x f64)
(local $term f64)
(local $e_x f64)
(local $factorial f64)
(local $sign f64)
(local $sin_x f64)
(local $cos_x f64)
(local $tan_x f64)
(local $ln_x f64)
(local $arctan_x f64)
(local $numTerms i32)
(local $i i32)
(local $j i32)
(local $temp i32)
(local $repeat i32)
i32.const 0
i32.const 51
call $logString
f64.const 0.123456789
local.set $x
i32.const 100000000
local.set $repeat
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
local.get $repeat
i32.gt_s
(if 
(then 
 br $endloop0
)
)
local.get $x
f64.const 0.000000001
f64.add
local.set $x
local.get $x
f64.const 1
f64.div
f64.const 1
f64.add
local.get $x
local.get $x
f64.mul
local.set $_s0
local.get $_s0
i32.const 1
i32.const 2
i32.mul
local.set $_s1
local.get $_s1
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
i32.const 8
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
i32.const 8
i32.mul
i32.const 9
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
i32.const 8
i32.mul
i32.const 9
i32.mul
i32.const 10
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.set $e_x
local.get $_s0
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
f64.convert_i32_s
f64.div
local.set $_tempSwap_f64
local.get $x
local.get $_tempSwap_f64
f64.sub
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
f64.convert_i32_s
f64.div
f64.sub
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
i32.const 8
i32.mul
i32.const 9
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
i32.const 8
i32.mul
i32.const 9
i32.mul
i32.const 10
i32.mul
f64.convert_i32_s
f64.div
f64.sub
local.set $sin_x
local.get $_s0
local.get $_s1
f64.convert_i32_s
f64.div
local.set $_tempSwap_f64
f64.const 1
local.get $_tempSwap_f64
f64.sub
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
f64.convert_i32_s
f64.div
f64.sub
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
i32.const 8
i32.mul
i32.const 9
i32.mul
f64.convert_i32_s
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $_s1
i32.const 3
i32.mul
i32.const 4
i32.mul
i32.const 5
i32.mul
i32.const 6
i32.mul
i32.const 7
i32.mul
i32.const 8
i32.mul
i32.const 9
i32.mul
i32.const 10
i32.mul
f64.convert_i32_s
f64.div
f64.sub
local.set $cos_x
local.get $x
f64.const 1
f64.sub
local.set $_s2
local.get $_s2
local.get $_s2
local.get $_s2
f64.mul
f64.const 2
f64.div
f64.sub
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 3
f64.div
f64.add
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 4
f64.div
f64.sub
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 5
f64.div
f64.add
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 6
f64.div
f64.sub
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 7
f64.div
f64.add
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 8
f64.div
f64.sub
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 9
f64.div
f64.add
local.get $_s2
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
local.get $_s2
f64.mul
f64.const 10
f64.div
f64.sub
local.set $ln_x
local.get $_s0
local.get $x
f64.mul
f64.const 3
f64.div
local.set $_tempSwap_f64
local.get $x
local.get $_tempSwap_f64
f64.sub
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
f64.const 5
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
f64.const 7
f64.div
f64.sub
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
f64.const 9
f64.div
f64.add
local.get $_s0
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
local.get $x
f64.mul
f64.const 11
f64.div
f64.sub
local.set $arctan_x
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
i32.const 66
i32.const 8
call $logString
local.get $x
call $log_f64
i32.const 60
i32.const 6
call $logString
local.get $e_x
call $log_f64
i32.const 51
i32.const 9
call $logString
local.get $sin_x
call $log_f64
i32.const 74
i32.const 9
call $logString
local.get $cos_x
call $log_f64
i32.const 95
i32.const 8
call $logString
local.get $ln_x
call $log_f64
i32.const 83
i32.const 12
call $logString
local.get $arctan_x
call $log_f64
)
)