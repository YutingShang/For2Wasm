! common sub expression elimination in a loop
! program runs slightly faster after common sub expression elimination
! just run the node.js program, not the compilation part

! also used to check CSE and PRE run correctly with other data types

program common4
    implicit none
    integer :: i
    real :: a, b, c, x, y, z, k

    x = -5.38
    y = 6.6892
    z = 10.000000000001
    k = 3.0

    do i = 1, 1000000000
        a = x*y/k * (y*z/x) + x*y/k * (y*z/x)
        b = x*y/k * (y*z/x) + x*y/k * (y*z/x)
        c = x*y/k * (y*z/x) + x*y/k * (y*z/x)
    end do

    print *, "a =", a
    print *, "b =", b
    print *, "c =", c

end program common4