!This program has partial redundancy in the THEN branch, requiring an else branch to be added
!The redundant expression is more complex than binary, so requires multiple iterations of PRE + CP

program partialredundant2
    implicit none
    integer :: a, b, x, y, z, k

    x = 5
    y = 6
    z = 10
    k = 3

    if (a==2) then
        a = x*y/k * (y*z/x)
    end if

    b = x*y/k * (y*z/x)

    print *, a, b

   
end program partialredundant2