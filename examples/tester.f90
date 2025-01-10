program simple
    implicit none
    integer :: a, b, x, y, z    

    a = 0
    b = 1

    if (a>3) then
        x = a .OR. b
    else 
        x = a .OR. b
    end if

    y = a .OR. b
    print *, "x =", x
   

end program simple
