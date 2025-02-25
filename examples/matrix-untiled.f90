! 3000x3000 matrix, no tiling

program matrix3000
    implicit none
    integer :: i, j, k, repeat
    real :: m1(3000, 3000), m2(3000, 3000), m3(3000, 3000), sum
    
    ! initialise matrix and matrix2 with 1's
    do i = 1, 3000
        do j = 1, 3000
            m1(i,j) = 1
            m2(i,j) = 1
            m3(i,j) = 0
        end do
    end do

    ! multiply matrix1 and matrix2 to get matrix3
    ! instead of i,j,k switch to j, k, i for column-major order
    do i = 1, 3000
        do j = 1, 3000
            do k = 1, 3000
                m3(i,j) = m3(i,j) + m1(i,k) * m2(k,j)
            end do
        end do
    end do


    ! sum the elements of matrix3 - should be equal to N^3
    sum = 0
    do i = 1, 3000
        do j = 1, 3000
            sum = sum + m3(i,j)
        end do
    end do

    print *, sum
 

end program matrix3000
