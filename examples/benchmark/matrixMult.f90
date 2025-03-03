! 2000x2000 matrix, no tiling
! matrix all all 1.1's multiplied by matrix all all -2's
! answer should be -12560*2000^2 

program matrixMult
    implicit none
    integer :: i, j, k, repeat, m2(2000, 2000)
    real :: m1(2000, 2000), m3(2000, 2000), sum
    
    ! initialise matrix and matrix2 with 1's
    do i = 1, 2000
        do j = 1, 2000
            m1(i,j) = 3.14
            m2(i,j) = -2
            m3(i,j) = 0
        end do
    end do

    ! multiply matrix1 and matrix2 to get matrix3
    ! instead of i,j,k switch to j, k, i for column-major order
    do i = 1, 2000
        do j = 1, 2000
            do k = 1, 2000
                m3(i,j) = m3(i,j) + m1(i,k) * m2(k,j)
            end do
        end do
    end do


    ! sum the elements of matrix3 - should be equal to N^3
    sum = 0
    do i = 1, 2000
        do j = 1, 2000
            sum = sum + m3(i,j)
        end do
    end do

    print *, sum
    print *, m3(1,1), m3(1,2), m3(1,3)
 

end program matrixMult
