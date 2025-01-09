! This program calculates the factorial of a given positive integer
! Prints message if the input is not a positive integer

program factorial
    implicit none
    integer :: num, i, factorial_res

    print *, "Enter a positive integer:"
    read *, num

    if (num < 0) then
        print *, "Factorial is not defined for negative numbers."
    else
        factorial_res = 1
        i = 1
        do
            factorial_res = factorial_res * i
            i = i + 1
            if (i > num) then
                exit
            endif
        end do
        print *, "The factorial of ", num, " is: ", factorial_res
    end if
end program factorial