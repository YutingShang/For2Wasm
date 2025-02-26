(module
(import "console" "log" (func $log_f64 (param f64)))
(import "js" "mem" (memory 245))
(func (export "main")
(local $_tempSwap_i32 i32)
(local $_tempSwap_f32 f32)
(local $_kmax i32)
(local $_kk i32)
(local $_jmax i32)
(local $_jj i32)
(local $_imax i32)
(local $_ii i32)
(local $sum f64)
(local $i i32)
(local $j i32)
(local $k i32)
(block $endloop7
i32.const 1
local.set $_ii
(loop $body7
local.get $_ii
i32.const 100
i32.gt_s
(if 
(then 
 br $endloop7
)
)
local.get $_ii
i32.const 4
i32.add
i32.const 1
i32.sub
i32.const 100
i32.gt_s
(if
(then
i32.const 100
local.set $_imax
)
(else
local.get $_ii
i32.const 4
i32.add
i32.const 1
i32.sub
local.set $_imax
)
)
(block $endloop8
i32.const 1
local.set $_jj
(loop $body8
local.get $_jj
i32.const 100
i32.gt_s
(if 
(then 
 br $endloop8
)
)
local.get $_jj
i32.const 4
i32.add
i32.const 1
i32.sub
i32.const 100
i32.gt_s
(if
(then
i32.const 100
local.set $_jmax
)
(else
local.get $_jj
i32.const 4
i32.add
i32.const 1
i32.sub
local.set $_jmax
)
)
(block $endloop0
local.get $_ii
local.set $i
(loop $body0
local.get $i
local.get $_imax
i32.gt_s
(if 
(then 
 br $endloop0
)
)
(block $endloop1
local.get $_jj
local.set $j
(loop $body1
local.get $j
local.get $_jmax
i32.gt_s
(if 
(then 
 br $endloop1
)
)
(block $endloop2
i32.const 1
local.set $k
(loop $body2
local.get $k
i32.const 100
i32.gt_s
(if 
(then 
 br $endloop2
)
)
local.get $k
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $j
i32.const 1
i32.sub
i32.const 100
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 8
i32.mul
i32.const 0
i32.add
f64.const 0
f64.store
local.get $k
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $j
i32.const 1
i32.sub
i32.const 100
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 8
i32.mul
i32.const 8000000
i32.add
f64.const 0
f64.store
local.get $k
i32.const 1
i32.add
local.set $k
br $body2
)
)
(block $endloop3
i32.const 1
local.set $k
(loop $body3
i32.const 50
i32.const 50
i32.add
local.set $_tempSwap_i32
local.get $k
local.get $_tempSwap_i32
i32.gt_s
(if 
(then 
 br $endloop3
)
)
f32.const 0
f32.const 0.99
f32.sub
local.set $_tempSwap_f32
local.get $k
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $j
i32.const 1
i32.sub
i32.const 100
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 8
i32.mul
i32.const 0
i32.add
local.get $_tempSwap_f32
f64.promote_f32
f64.store
local.get $k
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $j
i32.const 1
i32.sub
i32.const 100
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 8
i32.mul
i32.const 8000000
i32.add
f64.const 2.2
f64.store
local.get $k
i32.const 1
i32.add
local.set $k
br $body3
)
)
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
local.get $_jj
i32.const 4
i32.add
local.set $_jj
br $body8
)
)
local.get $_ii
i32.const 4
i32.add
local.set $_ii
br $body7
)
)
f64.const 0
local.set $sum
(block $endloop9
i32.const 1
local.set $_ii
(loop $body9
local.get $_ii
i32.const 100
i32.gt_s
(if 
(then 
 br $endloop9
)
)
local.get $_ii
i32.const 4
i32.add
i32.const 1
i32.sub
i32.const 100
i32.gt_s
(if
(then
i32.const 100
local.set $_imax
)
(else
local.get $_ii
i32.const 4
i32.add
i32.const 1
i32.sub
local.set $_imax
)
)
(block $endloop10
i32.const 1
local.set $_jj
(loop $body10
local.get $_jj
i32.const 100
i32.gt_s
(if 
(then 
 br $endloop10
)
)
local.get $_jj
i32.const 4
i32.add
i32.const 1
i32.sub
i32.const 100
i32.gt_s
(if
(then
i32.const 100
local.set $_jmax
)
(else
local.get $_jj
i32.const 4
i32.add
i32.const 1
i32.sub
local.set $_jmax
)
)
(block $endloop11
i32.const 1
local.set $_kk
(loop $body11
local.get $_kk
i32.const 100
i32.gt_s
(if 
(then 
 br $endloop11
)
)
local.get $_kk
i32.const 4
i32.add
i32.const 1
i32.sub
i32.const 100
i32.gt_s
(if
(then
i32.const 100
local.set $_kmax
)
(else
local.get $_kk
i32.const 4
i32.add
i32.const 1
i32.sub
local.set $_kmax
)
)
(block $endloop4
local.get $_ii
local.set $i
(loop $body4
local.get $i
local.get $_imax
i32.gt_s
(if 
(then 
 br $endloop4
)
)
(block $endloop5
local.get $_jj
local.set $j
(loop $body5
local.get $j
local.get $_jmax
i32.gt_s
(if 
(then 
 br $endloop5
)
)
(block $endloop6
local.get $_kk
local.set $k
(loop $body6
local.get $k
local.get $_kmax
i32.gt_s
(if 
(then 
 br $endloop6
)
)
local.get $k
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $j
i32.const 1
i32.sub
i32.const 100
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 8
i32.mul
i32.const 0
i32.add
f64.load
local.get $sum
f64.add
local.get $k
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $j
i32.const 1
i32.sub
i32.const 100
i32.mul
i32.add
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 8
i32.mul
i32.const 8000000
i32.add
f64.load
f64.add
local.set $sum
local.get $k
i32.const 1
i32.add
local.set $k
br $body6
)
)
local.get $j
i32.const 1
i32.add
local.set $j
br $body5
)
)
local.get $i
i32.const 1
i32.add
local.set $i
br $body4
)
)
local.get $_kk
i32.const 4
i32.add
local.set $_kk
br $body11
)
)
local.get $_jj
i32.const 4
i32.add
local.set $_jj
br $body10
)
)
local.get $_ii
i32.const 4
i32.add
local.set $_ii
br $body9
)
)
local.get $sum
call $log_f64
)
)