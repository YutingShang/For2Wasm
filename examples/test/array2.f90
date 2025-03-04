! using different array data types

program array2
    implicit none
    real :: x(2, 2, 3, 2)
    integer :: i, j, k, l

    do i = 1, 2
        do j = 1, 2
            do k = 1, 3
                do l = 1, 2
                    print *, i, "*", j, "*", k, "*", l
                    x(i, j, k, l) = i * j * k * l +1.00009
                    print *, x(i, j, k, l)
                end do
            end do
        end do
    end do

    print *, x
end program array2
