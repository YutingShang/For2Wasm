program partialredundant4

    implicit none
    integer :: z, p, q, a, y, i, terminate

    p = 7 
    q = 6

    terminate = 1

    do i = 1, 6
        a=0
        do 
            z = p * p * p * p * p * p * p / (q * q * q * q * q * q)
            if (a==1000000000) then
                exit   
            endif
            y = p * p * p * p * p * p * p / (q * q * q * q * q * q)
            
            if (terminate==1) then
                a = a + 1
            endif
        end do
    end do
    z = p * p * p * p * p * p * p / (q * q * q * q * q * q)

    print *, "7^7 / 6^6 = ", z, y

end program 