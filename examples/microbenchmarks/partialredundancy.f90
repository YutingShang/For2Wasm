! can reduce runtime from 25 seconds to 11 second
! maybe try to make this a bit more useful??

program partialredundant4

    implicit none
    integer :: res1, res2, seven, six, iter

    seven = 7 
    six = 6
    
    iter = 2000000000
    do 
        res1 = seven * seven * seven * seven * seven * seven * seven / (six * six * six * six * six * six)
        if (iter==0) then
            exit   
        endif
        res2 = seven * seven * seven * seven * seven * seven * seven / (six * six * six * six * six * six)
        
        iter = iter - 1
    end do

    res1 = seven * seven * seven * seven * seven * seven * seven / (six * six * six * six * six * six)

    print *, "Calculated with multiplications and divisions: 7^7 / 6^6 = ", res2

    res1 = 7**7 / 6**6      !all previous res1 code is dead code
    print *, "Calculated with exponentiation: 7^7 / 6^6 = ", res1


end program 