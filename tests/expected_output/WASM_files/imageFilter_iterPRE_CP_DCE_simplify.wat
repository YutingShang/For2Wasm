(module
(import "console" "log" (func $log_f32 (param f32)))
(import "console" "logString" (func $logString (param i32 i32)))
(import "math" "pow" (func $pow (param f64 f64) (result f64)))
(import "js" "mem" (memory 12208))
(data (i32.const 0) "Convolution completed. Sample values:")
(func (export "main")
(local $_tempSwap_f32 f32)
(local $_s8 i32)
(local $_s7 i32)
(local $_s6 i32)
(local $_s5 i32)
(local $_s4 i32)
(local $_s3 i32)
(local $_s2 i32)
(local $width i32)
(local $height i32)
(local $x i32)
(local $y i32)
(local $kx i32)
(local $ky i32)
(local $sum_x f32)
(local $sum_y f32)
(local $magnitude f32)
(local $pixel f32)
i32.const 10000
local.set $width
i32.const 10000
local.set $height
i32.const 3
i32.const 1
i32.sub
i32.const 3
i32.mul
i32.const 3
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 800000037
i32.add
f32.const 1.0
f32.store
i32.const 3
i32.const 1
i32.sub
i32.const 3
i32.mul
i32.const 3
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 800000073
i32.add
f32.const 1.0
f32.store
(block $endloop0
i32.const 1
local.set $y
(loop $body0
local.get $y
local.get $height
i32.gt_s
(if 
(then 
 br $endloop0
)
)
(block $endloop1
i32.const 1
local.set $x
(loop $body1
local.get $x
local.get $width
i32.gt_s
(if 
(then 
 br $endloop1
)
)
local.get $x
local.get $y
i32.add
f32.convert_i32_s
f32.const 100.0
f32.div
local.set $_tempSwap_f32
local.get $y
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $x
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 37
i32.add
local.get $_tempSwap_f32
f32.store
local.get $x
i32.const 1
i32.add
local.set $x
br $body1
)
)
local.get $y
i32.const 1
i32.add
local.set $y
br $body0
)
)
(block $endloop2
i32.const 2
local.set $y
local.get $height
i32.const 1
i32.sub
local.set $_s2
(loop $body2
local.get $y
local.get $_s2
i32.gt_s
(if 
(then 
 br $endloop2
)
)
(block $endloop3
i32.const 2
local.set $x
local.get $width
i32.const 1
i32.sub
local.set $_s3
(loop $body3
local.get $x
local.get $_s3
i32.gt_s
(if 
(then 
 br $endloop3
)
)
f32.const 0.0
local.set $sum_x
f32.const 0.0
local.set $sum_y
(block $endloop4
i32.const 0
i32.const 1
i32.sub
local.set $_s4
local.get $_s4
local.set $ky
(loop $body4
local.get $ky
i32.const 1
i32.gt_s
(if 
(then 
 br $endloop4
)
)
(block $endloop5
local.get $_s4
local.set $kx
(loop $body5
local.get $kx
i32.const 1
i32.gt_s
(if 
(then 
 br $endloop5
)
)
local.get $x
local.get $kx
i32.add
local.set $_s5
local.get $y
local.get $ky
i32.add
local.set $_s6
local.get $_s6
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $_s5
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 37
i32.add
f32.load
local.set $pixel
local.get $ky
i32.const 2
i32.add
local.set $_s7
local.get $kx
i32.const 2
i32.add
local.set $_s8
local.get $_s8
i32.const 1
i32.sub
i32.const 3
i32.mul
local.get $_s7
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 800000037
i32.add
f32.load
local.get $pixel
f32.mul
local.get $sum_x
f32.add
local.set $sum_x
local.get $_s6
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $_s5
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 37
i32.add
f32.load
local.get $_s8
i32.const 1
i32.sub
i32.const 3
i32.mul
local.get $_s7
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 800000073
i32.add
f32.load
f32.mul
local.get $sum_y
f32.add
local.set $sum_y
local.get $kx
i32.const 1
i32.add
local.set $kx
br $body5
)
)
local.get $ky
i32.const 1
i32.add
local.set $ky
br $body4
)
)
local.get $sum_x
local.get $sum_x
f32.mul
local.get $sum_y
local.get $sum_y
f32.mul
f32.add
local.set $magnitude
local.get $magnitude
f32.const 0.0
f32.gt
(if
(then
local.get $magnitude
f64.promote_f32
f64.const 0.5
call $pow
f32.demote_f64
local.set $magnitude
)
)
local.get $y
i32.const 1
i32.sub
i32.const 10000
i32.mul
local.get $x
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 400000037
i32.add
local.get $magnitude
f32.store
local.get $x
i32.const 1
i32.add
local.set $x
br $body3
)
)
local.get $y
i32.const 1
i32.add
local.set $y
br $body2
)
)
i32.const 0
i32.const 37
call $logString
i32.const 100
i32.const 1
i32.sub
i32.const 10000
i32.mul
i32.const 100
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 400000037
i32.add
f32.load
call $log_f32
i32.const 150
i32.const 1
i32.sub
i32.const 10000
i32.mul
i32.const 150
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 400000037
i32.add
f32.load
call $log_f32
i32.const 200
i32.const 1
i32.sub
i32.const 10000
i32.mul
i32.const 200
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 400000037
i32.add
f32.load
call $log_f32
i32.const 2000
i32.const 1
i32.sub
i32.const 10000
i32.mul
i32.const 2000
i32.const 1
i32.sub
i32.add
i32.const 4
i32.mul
i32.const 400000037
i32.add
f32.load
call $log_f32
)
)