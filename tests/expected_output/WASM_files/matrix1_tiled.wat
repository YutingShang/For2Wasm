(module
(import "console" "log_i64" (func $log_i64 (param i64)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "\01\00\00\00\00\00\00\00\02\00\00\00\00\00\00\00\03\00\00\00\00\00\00\00\04\00\00\00\00\00\00\00\05\00\00\00\00\00\00\00\06\00\00\00\00\00\00\00\07\00\00\00\00\00\00\00\08\00\00\00\00\00\00\00\09\00\00\00\00\00\00\00\0a\00\00\00\00\00\00\00\0b\00\00\00\00\00\00\00\0c\00\00\00\00\00\00\00\0d\00\00\00\00\00\00\00\0e\00\00\00\00\00\00\00\0f\00\00\00\00\00\00\00\10\00\00\00\00\00\00\00\11\00\00\00\00\00\00\00\12\00\00\00\00\00\00\00\13\00\00\00\00\00\00\00\14\00\00\00\00\00\00\00")
(data (i32.const 160) "\ec\ff\ff\ff\ff\ff\ff\ff\ed\ff\ff\ff\ff\ff\ff\ff\ee\ff\ff\ff\ff\ff\ff\ff\ef\ff\ff\ff\ff\ff\ff\ff\f0\ff\ff\ff\ff\ff\ff\ff\f1\ff\ff\ff\ff\ff\ff\ff\f2\ff\ff\ff\ff\ff\ff\ff\f3\ff\ff\ff\ff\ff\ff\ff\f4\ff\ff\ff\ff\ff\ff\ff\f5\ff\ff\ff\ff\ff\ff\ff\f6\ff\ff\ff\ff\ff\ff\ff\f7\ff\ff\ff\ff\ff\ff\ff\f8\ff\ff\ff\ff\ff\ff\ff\f9\ff\ff\ff\ff\ff\ff\ff\fa\ff\ff\ff\ff\ff\ff\ff\fb\ff\ff\ff\ff\ff\ff\ff\fc\ff\ff\ff\ff\ff\ff\ff\fd\ff\ff\ff\ff\ff\ff\ff\fe\ff\ff\ff\ff\ff\ff\ff\ff\ff\ff\ff\ff\ff\ff\ff")
(func (export "main")
(local $_tempSwap_i32 i32)
(local $_tempSwap_i64 i64)
(local $_imax i64)
(local $_ii i64)
(local $sum i64)
(local $i i64)
(local $j i64)
(local $k i64)
(local $N i64)
(local $temp i64)
(local $start i64)
i64.const 20
local.set $N
i64.const 8
local.set $start
(block $endloop4
local.get $start
i64.const 7
i64.sub
local.set $_ii
(loop $body4
i32.const 4
i32.const 1
i32.mul
local.set $_tempSwap_i32
local.get $N
local.get $_tempSwap_i32
i64.extend_i32_s
i64.sub
local.set $_tempSwap_i64
local.get $_ii
local.get $_tempSwap_i64
i64.gt_s
(if 
(then 
 br $endloop4
)
)
local.get $_ii
i64.const 4
i64.add
i64.const 1
i64.sub
i32.const 4
i32.const 1
i32.mul
local.set $_tempSwap_i32
local.get $N
local.get $_tempSwap_i32
i64.extend_i32_s
i64.sub
i64.gt_s
(if
(then
i32.const 4
i32.const 1
i32.mul
local.set $_tempSwap_i32
local.get $N
local.get $_tempSwap_i32
i64.extend_i32_s
i64.sub
local.set $_imax
)
(else
local.get $_ii
i64.const 4
i64.add
i64.const 1
i64.sub
local.set $_imax
)
)
(block $endloop0
local.get $_ii
local.set $i
(loop $body0
local.get $i
local.get $_imax
i64.gt_s
(if 
(then 
 br $endloop0
)
)
local.get $i
i64.const 4
i64.add
local.set $temp
(block $endloop1
i64.const 1
local.set $j
(loop $body1
local.get $j
local.get $N
i64.gt_s
(if 
(then 
 br $endloop1
)
)
(block $endloop2
i64.const 1
local.set $k
(loop $body2
local.get $k
i64.const 20
i64.gt_s
(if 
(then 
 br $endloop2
)
)
local.get $temp
i32.wrap_i64
i32.const 1
i32.sub
i32.const 8
i32.mul
i32.const 160
i32.add
i64.load
local.set $_tempSwap_i64
local.get $i
i32.wrap_i64
i32.const 1
i32.sub
i32.const 8
i32.mul
i32.const 0
i32.add
local.get $_tempSwap_i64
i64.store
local.get $k
i64.const 1
i64.add
local.set $k
br $body2
)
)
local.get $j
i64.const 1
i64.add
local.set $j
br $body1
)
)
local.get $i
i64.const 1
i64.add
local.set $i
br $body0
)
)
local.get $_ii
i64.const 4
i64.add
local.set $_ii
br $body4
)
)
i64.const 0
local.set $sum
(block $endloop5
i64.const 1
local.set $_ii
(loop $body5
local.get $_ii
local.get $N
i64.gt_s
(if 
(then 
 br $endloop5
)
)
local.get $_ii
i64.const 4
i64.add
i64.const 1
i64.sub
local.get $N
i64.gt_s
(if
(then
local.get $N
local.set $_imax
)
(else
local.get $_ii
i64.const 4
i64.add
i64.const 1
i64.sub
local.set $_imax
)
)
(block $endloop3
local.get $_ii
local.set $i
(loop $body3
local.get $i
local.get $_imax
i64.gt_s
(if 
(then 
 br $endloop3
)
)
local.get $i
i32.wrap_i64
i32.const 1
i32.sub
i32.const 8
i32.mul
i32.const 0
i32.add
i64.load
local.get $sum
i64.add
local.get $i
i32.wrap_i64
i32.const 1
i32.sub
i32.const 8
i32.mul
i32.const 160
i32.add
i64.load
i64.add
local.set $sum
local.get $i
i64.const 1
i64.add
local.set $i
br $body3
)
)
local.get $_ii
i64.const 4
i64.add
local.set $_ii
br $body5
)
)
local.get $sum
call $log_i64
)
)