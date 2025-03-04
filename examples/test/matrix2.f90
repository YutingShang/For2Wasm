!Case 2: imperfectly nested loops
!tiling should only tile the outer two loops

program matrix2
    implicit none
    double precision :: arr(100, 100, 100), ray(100, 100, 100), sum
    integer :: i, j, k
    
    do i = 1, 100
        do j = 1, 100
            do k = 1, 100
                arr(i,j,k) = 0
                ray(i,j,k) = 0
            end do
        
            do k = 1, 50+50
                arr(i,j,k) = -0.99
                ray(i,j,k) = 2.2
            end do
        end do
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
end program matrix2
