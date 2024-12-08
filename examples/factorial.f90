! sum.f90
! Performs summations using in a loop using EXIT statement
! Saves input information and the summation in a data file

program summation
    implicit none
    integer :: sum, q

    print *, "Triangular number"
    q=5
    sum = 0
    do
        if (q == 0) then
            exit
        endif
        sum = sum + q
        q = q -1 
    enddo

    print *, "TADA here is sum", sum
end