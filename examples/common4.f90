! common sub expression elimination in a loop
! program runs slightly faster after common sub expression elimination
! just run the node.js program, not the compilation part

program common4
    implicit none
    integer :: a, b, c, x, y, z, k, i

    x = 5
    y = 6
    z = 10
    k = 3

    do i = 1, 1000000000
        a = x*y/k * (y*z/x)
        b = x*y/k * (y*z/x)
        c = x*y/k * (y*z/x)
    end do

    print *, "a =", a
    print *, "b =", b
    print *, "c =", c

end program common4