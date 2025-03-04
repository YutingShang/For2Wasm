!Case 3: imperfectly nested loops
!tiling should only tile the outer loop

program matrix3
    implicit none
    double precision :: arr(100, 100, 100), ray(100, 100, 100), sum
    real :: temp
    integer :: i, j, k
    
    temp = -0.99
    do i = 1, 100
        do j = 1, 100        
            do k = 1, 50+50
                arr(i,j,k) = temp
                ray(i,j,k) = 2.2
            end do
        end do
        temp = temp + 0.067
    end do

    !sum the arrays
    sum = 0
    do i = 1, 100
        do j = 1, 100
            do k = 1, 100
                sum = sum + arr(i,j,k) + ray(i,j,k)
            end do
        end do
    end do

    print *, sum
end program matrix3
