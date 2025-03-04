! This program is an example of dead code elimination.
! a is dead code - all nested loop can be removed
! NB. program runs for about 30 seconds without optimisation, 1 second with simplification optimisation

program dead3
    implicit none
    integer :: a, i, j, k
 
    do i = 1, 10000
        do j = 1, 1000
            do k = 1, 1000
                a = 3
            end do
        end do
    end do

    print *, "Never gonna give you up"

end program dead3

