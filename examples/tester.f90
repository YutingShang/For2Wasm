! contains nested loops and if statements

program tester
    implicit none
    integer :: sum, a, i

    
    do i = 1, 10
        print *, i
        do 
            if (i == 5) then
                exit
            end if
        end do
    end do

end program tester