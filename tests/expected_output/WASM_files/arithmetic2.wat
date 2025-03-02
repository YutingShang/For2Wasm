(module
(import "console" "log" (func $log_f32 (param f32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "a = ")
(data (i32.const 4) "array = ")
(data (i32.const 12) "\00\00\80\3f\00\00\00\40\00\00\40\40")
(func (export "main")
(local $_tempSwap_i64 i64)
(local $_tempSwap_f32 f32)
(local $i i32)
(local $j i32)
(local $k i32)
(local $a f32)
(local $b f32)
(local $c f32)
(local $l i32)
(local $b4 i32)
(local $b8 i64)
(local $b9 i64)
i32.const 1
local.set $i
i32.const 2
local.set $j
local.get $b
local.get $i
f32.convert_i32_s
f32.add
local.get $j
f32.convert_i32_s
f32.add
local.set $a
local.get $i
local.get $j
i32.add
f32.convert_i32_s
local.get $b
f32.add
local.set $a
local.get $i
local.get $j
i32.add
local.get $i
local.get $k
i32.add
i32.add
f32.convert_i32_s
local.set $c
local.get $i
local.get $j
i32.add
local.get $i
local.get $k
i32.add
i32.add
f32.convert_i32_s
local.set $a
local.get $b
local.get $i
f32.convert_i32_s
f32.add
local.get $i
local.get $j
i32.add
f32.convert_i32_s
f32.add
local.set $a
i32.const 1
i32.const 2
i32.add
f32.const 4.5
f32.const 7.1
f32.add
local.set $_tempSwap_f32
f32.convert_i32_s
local.get $_tempSwap_f32
f32.add
local.set $a
i32.const 1
i32.const 2
i32.add
f32.const 4.5
f32.const 7.1
f32.add
local.set $_tempSwap_f32
f32.convert_i32_s
local.get $_tempSwap_f32
f32.sub
local.set $a
i32.const 1
i32.const 2
i32.add
f32.const 4.5
f32.const 7.1
f32.add
local.set $_tempSwap_f32
f32.convert_i32_s
local.get $_tempSwap_f32
f32.eq
local.set $l
i32.const 1
i32.const 2
i32.add
f32.const 4.5
f32.const 7.1
f32.add
local.set $_tempSwap_f32
f32.convert_i32_s
local.get $_tempSwap_f32
f32.ge
local.set $l
f32.const 1.4
f32.const 2
f32.add
f32.const 4.5
f32.const 7.1
f32.add
f32.eq
local.set $l
f32.const 1.4
f32.const 2
f32.add
i32.const 4
i32.const 9
i32.add
f32.convert_i32_s
f32.ne
local.set $l
f32.const 4.1
f32.const 10
f32.add
f32.const 5.42
f32.eq
local.set $l
f32.const 4.1
f32.const 10
f32.add
f32.const 5
f32.eq
local.set $l
i32.const 0
local.set $b4
i64.const 1
local.set $b8
local.get $b4
local.get $b4
i32.and
local.get $b8
local.get $b8
i64.or
local.set $_tempSwap_i64
i64.extend_i32_s
local.get $_tempSwap_i64
i64.and
local.set $b9
local.get $b4
local.get $b4
i32.or
i64.extend_i32_s
local.get $b8
i64.and
local.set $b9
i32.const 0
i32.const 4
call $logString
local.get $a
call $log_f32
i32.const 1
i32.const 2
i32.add
i32.const 5
i32.const 4
i32.sub
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
f32.load
local.set $_tempSwap_f32
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
local.get $_tempSwap_f32
f32.store
i32.const 4
i32.const 8
call $logString
i32.const 12
f32.load
call $log_f32
i32.const 16
f32.load
call $log_f32
i32.const 20
f32.load
call $log_f32
local.get $i
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
f32.const 1.0
f32.store
local.get $j
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
f32.load
local.set $_tempSwap_f32
local.get $i
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
local.get $_tempSwap_f32
f32.store
local.get $i
i32.const 2
i32.add
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
local.get $k
f32.convert_i32_s
f32.store
local.get $i
i32.const 2
i32.add
local.get $i
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
f32.load
local.set $_tempSwap_f32
i32.const 1
i32.sub
i32.const 4
i32.mul
i32.const 12
i32.add
local.get $_tempSwap_f32
f32.store
)
)