program factorial
    implicit none
    integer :: num, i, resultThing
    ! Get user input
    print *, "Enter a positive integer:"
    read *, num
    ! Check if the input is valid (positive integer)
    if (num < 0) then
        print *, "Factorial is not defined for negative numbers."
    else
        ! Initialize resultThing
        resultThing = 1
        i = 1
        ! Calculate factorial using do (true) and exit
        do
            resultThing = resultThing * i
            i = i + 1
            if (i > num) then
                exit
            endif
        end do
        ! Display the resultThing
        print *, "The factorial of ", num, " is: ", resultThing
    end if
end program factorial