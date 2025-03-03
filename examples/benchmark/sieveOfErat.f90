program prime_sieve
    implicit none
    integer :: i, j, count, limit, marked(500000000)  ! 1 means marked (not prime), 0 means unmarked (prime)
    
    ! Set the upper limit
    limit = 500000000
    
    ! Initialize all numbers as unmarked
    do i = 1, limit
        marked(i) = 0
    end do
    
    ! Mark 1 as it's not a prime by definition
    marked(1) = 1
    
    ! Apply the sieve - opportunities for loop optimization
    i = 2
    do 
        if (i * i > limit) then
            exit
        end if
        ! If i is unmarked, mark all its multiples
        if (marked(i) == 0) then
            j = i * i  ! Start from i*i as smaller multiples already marked
            do 
                if (j > limit) then
                    exit
                end if
                marked(j) = 1
                j = j + i
            end do
        end if
        
        ! Move to next number
        i = i + 1
    end do
    
    ! Count primes and calculate sum
    count = 0
    do i = 1, limit
        if (marked(i) == 0) then
            count = count + 1
        end if
    end do
    
    print *, "Number of primes up to", limit, "is", count
end program prime_sieve