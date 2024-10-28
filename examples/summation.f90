! sum.f90
! Performs summations using in a loop using EXIT statement
! Saves input information and the summation in a data file

program summation
    implicit none
    integer :: sum, a

    print *, "This program performs summations. Enter 0 to stop."
    sum = 0
    do
        print *, "Add:"
        read *, a
        if (a == 0) then
            exit
        else
            sum = sum + a
        end if
    end do

    print *, "Summation =", sum
end