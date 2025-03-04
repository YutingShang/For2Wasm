! This program is an example of dead code elimination.
! a is dead code - the entire inner loop can be removed
! NB. program runs for about 9 seconds without optimisation, 1 second with simplification optimisation

program dead2
    implicit none
    integer :: a, b, i, j
 
    do i = 1, 50000
        do j = 1, 50000
            a = 3
        end do
        b = 42
    end do

    print *, "The answer to the universe is:", b

end program dead2