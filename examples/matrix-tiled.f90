! with loop tiling - 3000x3000 matrix, tile size can be changed

program matrix3000tiled
    implicit none
    integer :: i, j, k, ii, jj, kk, repeat, tile, iimax, jjmax, kkmax
    real :: m1(3000, 3000), m2(3000, 3000), m3(3000, 3000), sum

tile = 512

    ! Initialize matrix and matrix2 with 1's and matrix3 with 0's
    do i = 1, 3000
        do j = 1, 3000
            m1(i,j) = 1
            m2(i,j) = 1
            m3(i,j) = 0
        end do
    end do

    ! Multiply matrix1 and matrix2 to get matrix3 using loop tiling
    
    do ii = 1, 3000, tile
        if (ii+tile-1 > 3000) then
            iimax = 3000
        else
            iimax = ii+tile-1
        end if
        do jj = 1, 3000, tile
            if (jj+tile-1 > 3000) then
                jjmax = 3000
            else
                jjmax = jj+tile-1
            end if
            do kk = 1, 3000, tile
                if (kk+tile-1 > 3000) then
                    kkmax = 3000
                else
                    kkmax = kk+tile-1
                end if
                do i = ii, iimax
                    do j = jj, jjmax
                        do k = kk, kkmax
                            m3(i,j) = m3(i,j) + m1(i,k) * m2(k,j)
                        end do
                    end do
                end do
            end do
        end do
    end do
   

    ! Sum the elements of matrix3
    sum = 0
    do i = 1, 3000
        do j = 1, 3000
            sum = sum + m3(i,j)
        end do
    end do

    print *, sum


end program matrix3000tiled
