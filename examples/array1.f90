! using data statements to initialize arrays, with / and /
! also printing arrays

program array1
    implicit none
    integer :: i, j, z(5), y(2, 3), k
    data z /1, 2, 3, 4, 5/
    data y /1, 0, 1, -4, 6, 9/

    print *, "Array z is"
    do i = 1, 5
        z(i) = i
        print *, z(i)
    end do

    print *, "Setting z(3) to 10"
    z(3) = 10

    print *, "Array z is now"
    print *, z

    print *, "Array y in row major order is"
    do i = 1, 2
        do j = 1, 3
            print *, y(i, j)
        end do
    end do

    print *, "The array y in column major order is ", y
    print *, "Enter a value to store in y(1, 2)"
    read *, k
    y(1, 2) = k
    print *, "The array y is now", y

end program array1
