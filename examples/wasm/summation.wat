(module
    (import "console" "logString" (func $logString (param i32 i32)))  ;; parameters for string_offest and string_length in memory
    (import "console" "log" (func $log (param i32)))
        (import "console" "promptSync" (func $read (result i32)))
    (import "js" "mem" (memory 1))     ;; 1 page memory = 64KB

    ;; data to write string to memory
    ;; only one memory in program, so no need to specify (data (memory 0)...
    ;; i32.const means starting at offset 0
    (data (i32.const 0) "This program performs summations. Enter 0 to stop.")   ;; 50 letters 
    (data (i32.const 50) "Add:") ;;4 letters
    (data (i32.const 54) "Summation =")   ;; 11 letters
    
    (func (export "main")
        (local $sum i32)
        (local $a i32)    ;; variable declarations

        i32.const 0
        i32.const 50
        call $logString       ;; prints This program performs ...
        
        (local.set $sum (i32.const 0))    ;; initialise sum=0

        ;; looping - do while
        (block $exit
        (loop $sumloop

            i32.const 50
            i32.const 4
            call $logString    ;; prints Add:

            (local.set $a (call $read))   ;; read *, a
            
            local.get $a
            i32.const 0
            i32.eq     ;; checks a==0
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

            br $sumloop   ;;loop,

        )  ;; end of loop
        );; exit

        i32.const 54
        i32.const 11
        call $logString   ;;print "Summation: "
        local.get $sum
        call $log
    )
)