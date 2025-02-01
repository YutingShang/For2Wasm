program preloop
    implicit none
    integer :: a, b, c, d, x, y, z, k

    x = 5
    y = 6
    z = 10
    k = 3

    do k = 1, 10
    enddo

    if (a==2) then
        a = x*y/k * (y*z/x)
     !   b = x*y/k * (y*z/x)
     !   c = x*y/k * (y*z/x)
    end if

    b = x*y/k * (y*z/x)

    print *, a, b

   
end program preloop