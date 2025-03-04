! triangular.f90
! Calculates triangular number of a given input

program triangular
    implicit none
    integer :: res, q

    print *, "What's the triangular number of?"
    read *, q
    res = 0
    do
        if (q == 0) then
            exit
        endif
        res = res + q
        q = q -1 
    enddo

    print *, "TADA here is triangular number of ", q, "=", res
end