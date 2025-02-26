(module
(import "console" "log" (func $log_f32 (param f32)))
(import "js" "mem" (memory 733))
(func (export "main")
(local $_tempSwap_f32 f32)
(local $_kmax i32)
(local $_kk i32)
(local $_jmax i32)
(local $_jj i32)
(local $_imax i32)
(local $_ii i32)
(local $i i32)
(local $j i32)
(local $k i32)
(local $repeat i32)
(local $sum f32)
(block $endloop7
i32.const 1
local.set $_ii
(loop $body7
local.get $_ii
i32.const 2000
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
i32.const 2000
i32.gt_s
(if
(then
i32.const 2000
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
i32.const 2000
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
i32.const 2000
i32.gt_s
(if
(then
i32.const 2000
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
local.get $j
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 0
i32.add
f32.const 1
f32.store
local.get $j
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 16000000
i32.add
f32.const 1
f32.store
local.get $j
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 32000000
i32.add
f32.const 0
f32.store
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
(block $endloop9
i32.const 1
local.set $_ii
(loop $body9
local.get $_ii
i32.const 2000
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
i32.const 2000
i32.gt_s
(if
(then
i32.const 2000
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
i32.const 2000
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
i32.const 2000
i32.gt_s
(if
(then
i32.const 2000
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
i32.const 2000
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
i32.const 2000
i32.gt_s
(if
(then
i32.const 2000
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
(block $endloop2
local.get $_ii
local.set $i
(loop $body2
local.get $i
local.get $_imax
i32.gt_s
(if 
(then 
 br $endloop2
)
)
(block $endloop3
local.get $_jj
local.set $j
(loop $body3
local.get $j
local.get $_jmax
i32.gt_s
(if 
(then 
 br $endloop3
)
)
(block $endloop4
local.get $_kk
local.set $k
(loop $body4
local.get $k
local.get $_kmax
i32.gt_s
(if 
(then 
 br $endloop4
)
)
local.get $j
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 32000000
i32.add
f32.load
local.get $k
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 0
i32.add
f32.load
local.get $j
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $k
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 16000000
i32.add
f32.load
f32.mul
f32.add
local.set $_tempSwap_f32
local.get $j
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 32000000
i32.add
local.get $_tempSwap_f32
f32.store
local.get $k
i32.const 1
i32.add
local.set $k
br $body4
)
)
local.get $j
i32.const 1
i32.add
local.set $j
br $body3
)
)
local.get $i
i32.const 1
i32.add
local.set $i
br $body2
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
f32.const 0
local.set $sum
(block $endloop12
i32.const 1
local.set $_ii
(loop $body12
local.get $_ii
i32.const 2000
i32.gt_s
(if 
(then 
 br $endloop12
)
)
local.get $_ii
i32.const 4
i32.add
i32.const 1
i32.sub
i32.const 2000
i32.gt_s
(if
(then
i32.const 2000
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
(block $endloop13
i32.const 1
local.set $_jj
(loop $body13
local.get $_jj
i32.const 2000
i32.gt_s
(if 
(then 
 br $endloop13
)
)
local.get $_jj
i32.const 4
i32.add
i32.const 1
i32.sub
i32.const 2000
i32.gt_s
(if
(then
i32.const 2000
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
(block $endloop5
local.get $_ii
local.set $i
(loop $body5
local.get $i
local.get $_imax
i32.gt_s
(if 
(then 
 br $endloop5
)
)
(block $endloop6
local.get $_jj
local.set $j
(loop $body6
local.get $j
local.get $_jmax
i32.gt_s
(if 
(then 
 br $endloop6
)
)
local.get $j
i32.const 1
i32.sub
i32.const 2000
i32.mul
local.get $i
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 32000000
i32.add
f32.load
local.get $sum
f32.add
local.set $sum
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
local.get $_jj
i32.const 4
i32.add
local.set $_jj
br $body13
)
)
local.get $_ii
i32.const 4
i32.add
local.set $_ii
br $body12
)
)
local.get $sum
call $log_f32
)
)