!Case 1 imperfectly nested loops
!tiling should only tile the outer loop, and also the bounds are expressions here

program matrix1
    implicit none
    integer(8) :: array(20), b(20), sum, i, j, k, N, temp, start
    data array /1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20/
    data b /-20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1/
    
    N = 20
    start = 8
    do i = start-7, N-4*1
        temp = i+4
        do j = 1, N
            do k = 1, 20
                array(i) = b(temp)
            end do
        end do
    end do

    sum = 0
    do i = 1, N
        sum = sum + array(i)+b(i)
    end do
    print *, sum
end program matrix1
