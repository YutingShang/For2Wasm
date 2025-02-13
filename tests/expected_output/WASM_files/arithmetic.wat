(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "log_i64" (func $log_i64 (param i64)))
(import "console" "log" (func $log_f32 (param f32)))
(import "console" "log" (func $log_f64 (param f64)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "pi + e = ")
(data (i32.const 9) "-2147483647 + -2147483647 = ")
(data (i32.const 37) "214748364700000 + 2147483647 = ")
(data (i32.const 68) "(20-10)*5/3/7 = ")
(data (i32.const 84) "2147483647 + 2147483647 = ")
(data (i32.const 110) "3 + 8 / ( 1 - 5 ) = ")
(data (i32.const 130) "pi - 2147483647 = ")
(data (i32.const 148) "42*10^42 + phi = ")
(data (i32.const 165) "2147483647 / pi  = ")
(data (i32.const 184) "20-(10*5)/3/7 = ")
(data (i32.const 200) "42*10^42 / 2147483647 = ")
(data (i32.const 224) "20-10*5/3/7 = ")
(data (i32.const 238) "(20-10)*(5/3/7) = ")
(data (i32.const 256) "20-10*(5/(3/7)) = ")
(data (i32.const 274) "-3 = ")
(data (i32.const 279) "(-1.234567890_8) * (-100.001_8) / ((-1.234567890_8) + (-100.001_8)) = ")
(data (i32.const 349) "-(-3.14) = ")
(data (i32.const 360) "12/ (16/(8-4)) = ")
(func (export "main")
(local $tempSwap_i32 i32)
(local $tempSwap_f32 f32)
(local $p i32)
(local $q i32)
(local $r i32)
(local $i i32)
(local $s i64)
(local $t i64)
(local $u f32)
(local $v f32)
(local $w f32)
(local $x f64)
(local $y f64)
(local $z f64)
(local $a f32)
(local $b f32)
(local $c f32)
(local $d f32)
(local $e f32)
(local $k f32)
i32.const 2147483647
local.set $p
i32.const 2147483647
local.set $q
local.get $p
local.get $q
i32.add
local.set $r
i32.const 84
i32.const 26
call $logString
local.get $r
call $log_i32
i64.const 214748364700000
local.set $s
local.get $s
local.get $q
i64.extend_i32_s
i64.add
local.set $t
i32.const 37
i32.const 31
call $logString
local.get $t
call $log_i64
f32.const 3.141592653589793238462643383279502884197
local.set $u
f32.const 2.718281828459045235360287471352662497757
local.set $v
local.get $u
local.get $v
f32.add
local.set $w
i32.const 0
i32.const 9
call $logString
local.get $w
call $log_f32
f64.const 42
local.set $x
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
i32.const 42
i32.gt_s
(if 
(then 
 br $endloop0
)
)
local.get $x
f64.const 10
f64.mul
local.set $x
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
f64.const 1.618033988749894848204586834365638117720
local.set $y
local.get $x
local.get $y
f64.add
local.set $z
i32.const 148
i32.const 17
call $logString
local.get $z
call $log_f64
local.get $u
local.get $q
f32.convert_i32_s
f32.sub
local.set $w
i32.const 130
i32.const 18
call $logString
local.get $w
call $log_f32
local.get $x
local.get $p
f64.convert_i32_s
f64.div
local.set $z
i32.const 200
i32.const 24
call $logString
local.get $z
call $log_f64
local.get $p
f32.convert_i32_s
local.get $u
f32.div
i64.trunc_f32_s
local.set $t
i32.const 165
i32.const 19
call $logString
local.get $t
call $log_i64
f32.const 20
local.set $a
f32.const 10
local.set $b
f32.const 5
local.set $c
f32.const 3
local.set $d
f32.const 7
local.set $e
local.get $b
local.get $c
f32.mul
local.get $d
f32.div
local.get $e
f32.div
local.set $tempSwap_f32
local.get $a
local.get $tempSwap_f32
f32.sub
local.set $k
i32.const 224
i32.const 14
call $logString
local.get $k
call $log_f32
local.get $a
local.get $b
f32.sub
local.get $c
f32.mul
local.get $d
f32.div
local.get $e
f32.div
local.set $k
i32.const 68
i32.const 16
call $logString
local.get $k
call $log_f32
local.get $b
local.get $c
f32.mul
local.get $d
f32.div
local.get $e
f32.div
local.set $tempSwap_f32
local.get $a
local.get $tempSwap_f32
f32.sub
local.set $k
i32.const 184
i32.const 16
call $logString
local.get $k
call $log_f32
local.get $a
local.get $b
f32.sub
local.get $c
local.get $d
f32.div
local.get $e
f32.div
f32.mul
local.set $k
i32.const 238
i32.const 18
call $logString
local.get $k
call $log_f32
local.get $d
local.get $e
f32.div
local.set $tempSwap_f32
local.get $c
local.get $tempSwap_f32
f32.div
local.get $b
f32.mul
local.set $tempSwap_f32
local.get $a
local.get $tempSwap_f32
f32.sub
local.set $k
i32.const 256
i32.const 18
call $logString
local.get $k
call $log_f32
i32.const 0
i32.const 2147483647
i32.sub
local.set $p
i32.const 0
i32.const 2147483647
i32.sub
local.set $q
local.get $p
local.get $q
i32.add
local.set $r
i32.const 9
i32.const 28
call $logString
local.get $r
call $log_i32
f32.const 0
f32.const 1.234567890
f32.sub
f64.promote_f32
local.set $x
f32.const 0
f32.const 100.001
f32.sub
f64.promote_f32
local.set $y
local.get $x
local.get $y
f64.mul
local.get $x
local.get $y
f64.add
f64.div
local.set $z
i32.const 279
i32.const 70
call $logString
local.get $z
call $log_f64
i32.const 0
i32.const 3
i32.sub
local.set $p
f32.const 0
f32.const 3.14
f32.sub
local.set $tempSwap_f32
f32.const 0
local.get $tempSwap_f32
f32.sub
f64.promote_f32
local.set $x
i32.const 1
i32.const 5
i32.sub
local.set $tempSwap_i32
i32.const 8
local.get $tempSwap_i32
i32.div_s
i32.const 3
i32.add
local.set $q
i32.const 274
i32.const 5
call $logString
local.get $p
call $log_i32
i32.const 349
i32.const 11
call $logString
local.get $x
call $log_f64
i32.const 110
i32.const 20
call $logString
local.get $q
call $log_i32
f32.const 1
f32.const 9.999
f32.sub
call $log_f32
local.get $x
call $log_f64
i32.const 3
local.get $p
i32.add
call $log_i32
i32.const 0
i32.const 3
i32.sub
local.set $tempSwap_i32
i32.const 4
local.get $tempSwap_i32
i32.div_s
call $log_i32
i32.const 0
i32.const 3
i32.sub
local.set $tempSwap_i32
i32.const 0
local.get $tempSwap_i32
i32.sub
call $log_i32
local.get $q
call $log_i32
f32.const 8
f32.const 4.01
f32.sub
local.set $tempSwap_f32
f32.const 16
local.get $tempSwap_f32
f32.div
local.set $tempSwap_f32
f32.const 12
local.get $tempSwap_f32
f32.div
f64.promote_f32
local.set $z
i32.const 360
i32.const 17
call $logString
local.get $z
call $log_f64
)
)