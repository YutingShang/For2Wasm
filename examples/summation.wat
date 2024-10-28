(module
    (import "console" "logString" (func $logString (param i32 i32)))  ;; parameters for string_offest and string_length in memory
    (import "console" "log" (func $log (param i32)))
  ;;  (import "console" "readlines" (func $readlines))   ;; maybe can't do async readline from wasm
    (import "js" "mem" (memory 1))     ;; 1 page memory = 64KB
    
    ;; data to write string to memory
    ;; only one memory in program, so no need to specify (data (memory 0)...
    ;; i32.const means starting at offset 0
    (data (i32.const 0) "This program performs summations. Enter 0 to stop.")   ;; 50 letters 
    (data (i32.const 50) "Summation =")   ;; 11 letters
     
    (func (export "intro")
        i32.const 0
        i32.const 50
        call $logString       ;;prints This program performs ...
    )

    
    (func (export "main")
        (param $base i32)    ;;base index of the array, each int will be 4 bytes
        (param $length i32)
       ;; (result i32)       ;;the sum - or just log it
        (local $sum i32)
        (local $count i32)   ;; keep track of the loop index
        (local $a i32)

        ;; looping - do while
        (local.set $count (i32.const 0))
        (local.set $sum (i32.const 0))    ;; initialise sum=0

        (block $exit
        (loop $sumloop
            ;; get the first integer, store in $a
            ;; a = base + offset * 4,  count keeps track of offset
            ;; nested/structured code
            (local.set $a (i32.load (i32.add (local.get $base) (i32.mul (local.get $count) (i32.const 4)))))    
            
            local.get $a
            i32.const 0
            i32.eq     ;; check a==0
            (if 
                (then 
                    br $exit    ;;branch OUT of the exit block
                )
                (else 
                    local.get $sum
                    local.get $a
                    i32.add
                    local.set $sum  ;; set sum = sum + a (plain - non-structured code)
                )
            )

            ;; increment count ++
            ;; if it is still the case that count < length (of array)
            ;; then next loop 
            (local.set $count (i32.add (local.get $count) (i32.const 1)))
            local.get $count
            local.get $length
            i32.lt_s   ;; count < length

            (if 
                (then 
                    br $sumloop   ;;loop, otherwise terminate loop
                )
            )

        )  ;; end of loop
        );; exit

        
        i32.const 50
        i32.const 61
        call $logString   ;;print "Summation: "
        local.get $sum
        call $log
        
    )
)