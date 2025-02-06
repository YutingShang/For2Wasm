!This program has partial redundancy between the end of one loop and the start of another
!PRE can bring out the first x+y outside of the loop
!can also perform CP and DCE 

program partialredundant3
    implicit none
    integer :: sum, a , b, x, y, i

    x = 2
    y = 3

    do i=1, 10
        a = x+y
        x =3
        b = x+y
    end do

    a = x+y

    print *, "2+3 = ", a, b


end program partialredundant3