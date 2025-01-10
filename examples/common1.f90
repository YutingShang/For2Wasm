! This program is an example of common subexpression elimination.
! The expression a = x+11 can be replaced with a = 11+x

program common1
    implicit none
    integer :: a, b, c, d, y, z

    y = 20
    z = 25

    a = y * z
    b = y * z
    c = y * z
    d = y * z

    print *, "20 * 25 =", d
   

end program common1