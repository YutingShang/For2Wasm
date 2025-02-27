(module
(import "console" "log" (func $log_i32 (param i32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "js" "mem" (memory 1))
(data (i32.const 0) "After")
(data (i32.const 5) "generations:")
(data (i32.const 17) "Living cells =")
(func (export "main")
(local $_tempSwap_i32 i32)
(local $_s4 i32)
(local $_s3 i32)
(local $_s2 i32)
(local $_s1 i32)
(local $_s0 i32)
(local $i i32)
(local $j i32)
(local $ni i32)
(local $nj i32)
(local $gen i32)
(local $neighbors i32)
(local $alive_cells i32)
(local $size_i i32)
(local $size_j i32)
(local $generations i32)
i32.const 50
local.set $size_i
i32.const 50
local.set $size_j
i32.const 1000000
local.set $generations
(block $endloop0
i32.const 1
local.set $i
(loop $body0
local.get $i
local.get $size_i
i32.gt_s
(if 
(then 
 br $endloop0
)
)
(block $endloop1
i32.const 1
local.set $j
(loop $body1
local.get $j
local.get $size_j
i32.gt_s
(if 
(then 
 br $endloop1
)
)
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 0
i32.store
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 10031
i32.add
i32.const 0
i32.store
local.get $j
i32.const 1
i32.add
local.set $j
br $body1
)
)
local.get $i
i32.const 1
i32.add
local.set $i
br $body0
)
)
i32.const 3
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 2
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 4
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 3
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 2
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 4
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 3
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 4
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 4
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 4
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 20
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 20
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 21
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 20
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 22
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 20
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 30
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 30
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 31
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 30
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 30
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 31
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 31
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 31
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 40
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 10
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 39
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 11
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 41
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 11
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 39
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 12
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 41
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 12
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
i32.const 40
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 13
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.const 1
i32.store
(block $endloop2
i32.const 1
local.set $gen
(loop $body2
local.get $gen
local.get $generations
i32.gt_s
(if 
(then 
 br $endloop2
)
)
(block $endloop3
i32.const 1
local.set $i
(loop $body3
local.get $i
local.get $size_i
i32.gt_s
(if 
(then 
 br $endloop3
)
)
(block $endloop4
i32.const 1
local.set $j
local.get $i
i32.const 1
i32.add
local.set $_s0
(loop $body4
local.get $j
local.get $size_j
i32.gt_s
(if 
(then 
 br $endloop4
)
)
i32.const 0
local.set $neighbors
local.get $i
i32.const 1
i32.gt_s
(if
(then
local.get $j
i32.const 1
i32.gt_s
local.set $_s1
local.get $_s1
(if
(then
local.get $i
i32.const 1
i32.sub
local.set $_s3
local.get $_s3
local.get $j
i32.const 1
i32.sub
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
)
(else
local.get $i
i32.const 1
i32.sub
local.set $_s3
)
)
local.get $_s3
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
local.get $j
local.get $size_j
i32.lt_s
local.set $_s2
local.get $_s2
(if
(then
local.get $_s3
local.get $j
i32.const 1
i32.add
local.set $_s4
local.get $_s4
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
)
(else
local.get $j
i32.const 1
i32.add
local.set $_s4
)
)
)
(else
local.get $j
i32.const 1
i32.add
local.set $_s4
local.get $j
local.get $size_j
i32.lt_s
local.set $_s2
local.get $j
i32.const 1
i32.gt_s
local.set $_s1
)
)
local.get $_s1
(if
(then
local.get $j
i32.const 1
i32.sub
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
)
)
local.get $_s2
(if
(then
local.get $_s4
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
)
)
local.get $i
local.get $size_i
i32.lt_s
(if
(then
local.get $_s1
(if
(then
local.get $_s0
local.get $j
i32.const 1
i32.sub
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
)
)
local.get $_s0
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
local.get $_s2
(if
(then
local.get $_s0
local.get $_s4
i32.const 1
i32.sub
i32.const 50
i32.mul
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $neighbors
i32.add
local.set $neighbors
)
)
)
)
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
i32.const 1
i32.eq
(if
(then
local.get $neighbors
i32.const 2
i32.lt_s
(if
(then
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 10031
i32.add
i32.const 0
i32.store
)
(else
local.get $neighbors
i32.const 3
i32.gt_s
(if
(then
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 10031
i32.add
i32.const 0
i32.store
)
(else
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 10031
i32.add
i32.const 1
i32.store
)
)
)
)
)
(else
local.get $neighbors
i32.const 3
i32.eq
(if
(then
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 10031
i32.add
i32.const 1
i32.store
)
(else
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 10031
i32.add
i32.const 0
i32.store
)
)
)
)
local.get $_s4
local.set $j
br $body4
)
)
local.get $_s0
local.set $i
br $body3
)
)
(block $endloop5
i32.const 1
local.set $i
(loop $body5
local.get $i
local.get $size_i
i32.gt_s
(if 
(then 
 br $endloop5
)
)
(block $endloop6
i32.const 1
local.set $j
(loop $body6
local.get $j
local.get $size_j
i32.gt_s
(if 
(then 
 br $endloop6
)
)
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 10031
i32.add
i32.load
local.set $_tempSwap_i32
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
local.get $_tempSwap_i32
i32.store
local.get $j
i32.const 1
i32.add
local.set $j
br $body6
)
)
local.get $i
i32.const 1
i32.add
local.set $i
br $body5
)
)
local.get $gen
i32.const 1
i32.add
local.set $gen
br $body2
)
)
i32.const 0
local.set $alive_cells
(block $endloop7
i32.const 1
local.set $i
(loop $body7
local.get $i
local.get $size_i
i32.gt_s
(if 
(then 
 br $endloop7
)
)
(block $endloop8
i32.const 1
local.set $j
(loop $body8
local.get $j
local.get $size_j
i32.gt_s
(if 
(then 
 br $endloop8
)
)
local.get $j
i32.const 1
i32.sub
i32.const 50
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 31
i32.add
i32.load
local.get $alive_cells
i32.add
local.set $alive_cells
local.get $j
i32.const 1
i32.add
local.set $j
br $body8
)
)
local.get $i
i32.const 1
i32.add
local.set $i
br $body7
)
)
i32.const 0
i32.const 5
call $logString
local.get $generations
call $log_i32
i32.const 5
i32.const 12
call $logString
i32.const 17
i32.const 14
call $logString
local.get $alive_cells
call $log_i32
)
)