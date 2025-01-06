program longtime
    implicit none
    integer :: n, x, a, b

    n=0
    x=0
    do
        a = -2
        b = 6
        x = (b/a*b + 100*b) * (b/a*b + 100*b) + (b/a*b + 100*b) * (b/a*b + 100*b) + (b/a*b + 100*b) * (b/a*b + 100*b) / (b/a*b + 100*b)
 
        if (n>2147483646) then
            exit
        end if
        n = n+1
    end do
 
    
    

end program longtime