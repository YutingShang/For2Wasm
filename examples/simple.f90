program simple
    implicit none
    integer :: x, i, j, y

    x = 3+4
    i = x+5
    j = x+i+x
    y = j*x
    y = 1 < 2
    x = 1 .AND. 0
    i = .NOT. y
    y = y+1

    do i=1, 5
        x = x+1
    end do

   






end program simple