program controlFlow
    implicit none
    double precision :: a, b
    integer(8) :: j, i

    i = 0
    do 
        print *, "Enter a decimal number, or 0 to stop : "
        read *, a
        if (a == 0) then
            exit
        end if
        
        do j=1, 5
            print *, "Enter a smaller decimal number : "
            read *, b
            
            if (b < a) then
                exit
            else if (b > a) then
                print *, "Try AGAIN! "
            else if (b == a) then
                print *, "Not quite - you entered the same number! "
            end if
        end do
        print *, "You got it right! "

        i = i + 1
        if (i == 3) then
            exit
        end if
        print *, "Lets play again! "
        print *, "--------------------------------"
    end do


end program controlFlow