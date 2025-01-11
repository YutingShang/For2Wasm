! This program is an example of common subexpression elimination.
! The expression y*z is recomputed 4 times in sequence

program common1
    implicit none
    integer :: a, b, c, d, y, z

    y = 20
    z = 25

    a = y * z
    b = y * z
    c = y * z
    d = y * z

    print *, "a = 20 * 25 =", a
    print *, "b = 20 * 25 =", b
    print *, "c = 20 * 25 =", c
    print *, "d = 20 * 25 =", d
   

end program common1