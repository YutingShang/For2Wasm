program simple
    implicit none
    integer :: a, b, x, y, z    

    a = 3
    b = 4
    !y=a+b

    y = (b/a*b + 100*b) * (b/a*b + 100*b) + (b/a*b + 100*b) * (b/a*b + 100*b) + (b/a*b + 100*b) * (b/a*b + 100*b) / (b/a*b + 100*b)

    if (a>3) then
        x = 1
    else 
        x = 2
    end if

    x = (b/a*b + 100*b) * (b/a*b + 100*b) + (b/a*b + 100*b) * (b/a*b + 100*b) + (b/a*b + 100*b) * (b/a*b + 100*b) / (b/a*b + 100*b)

    print *, "x =", x
   

end program simple
