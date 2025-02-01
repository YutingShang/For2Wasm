program simple
    implicit none
    integer :: a, b, c, d, e, i

    a = 2
    b = 3
    c = 4

    if (a==2) then
        c = 2
    else
        a = b+c
    end if

    d = b+c

    if (a==2) then
        do i=1, 10
            e = b+c
        end do
    end if

    print *, a, b, c, d, e

   
end program simple
