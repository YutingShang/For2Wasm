! contains nested loops and if statements

program tester
    implicit none
    integer :: sum, a, i

    
    do
        read *, a
        if (a == 0) then
            exit
        else
            if (a == 1) then
                print *, "a is 1"
            else
                print *, "a is not 1"
            end if
        end if

        do 
            read *, i
            if (i == 0) then
                exit
            else
                print *, "i is not 0"
            end if
        end do
    end do

end program tester