! This program is an example of dead code elimination.
! first iteration will detect c as dead code
! second iteration will detect a,b as dead code
! third iteration will detect x,y as dead code

program dead1
    implicit none
    integer :: a, b, c, x, y, z
 
    x=1
    y=2
    z=3

    a = x+11
    b = y+12
    c = a*b
    print *, "z=", z

end program dead1