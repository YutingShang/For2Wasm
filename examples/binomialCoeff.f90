program binomialCoeff
    implicit none
    integer :: n, k, i, trials, j
    integer(8) :: res

    ! Set parameters
    n = 20
    k = 10
    trials = 1000
    
    print *, "Starting binomial coefficient calculation"
    print *, "n =", n, "k =", k
    
    ! Perform multiple trials to make timing measurable
    do j = 1, trials
        ! Calculate binomial coefficient with minimal computations
        
        ! Initialize res
        res = 1
        
        ! Calculate binomial coefficient with efficient method
        do i = 1, k
            res = res * (n - i + 1) / i
        end do
    end do
    
    ! Output results
    print *, "C(", n, ",", k, ") =", res
    print *, "Calculation complete"
    
end program binomialCoeff
