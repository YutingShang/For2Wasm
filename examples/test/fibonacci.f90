program fibonacci
    implicit none
    integer :: n, ppnumber, pnumber, cnumber, i

    print *, "Enter a positive integer:"
    read *, n

    if (n==0) then 
        print *, "The fibonacci number of ", n, " is:", n
    else
        ppnumber = 0   !previous previous number
        pnumber = 0   !previous number
        cnumber = 1   !current number
        i=1
        do 
            ppnumber = pnumber
            pnumber = cnumber
            cnumber = ppnumber + pnumber

            if (i>=n) then
                exit
            endif
            i=i+1
        end do
        print *, "The fibonacci number of ", n, " is:", cnumber
    end if

    
  
end program fibonacci