!calculate the mandelbrot set for a 2000x2000 grid
!calculate z = z^2 + c for each point in the grid
!if z escapes to infinity (test for abs(z) > 2), the point is in the mandelbrot set
!otherwise, the point is not in the mandelbrot set

program mandelbrot
    implicit none
    double precision :: x, y, x_new, y_new, x_0, y_0
    double precision :: x_part, y_part, x_start, x_end, y_start, y_end
    integer :: iter, max_iter, x_0_index, y_0_index, width, height, i, j
    real :: in_mandelbrot(2000, 2000)   !holds color values for each point, closer to 0 means it diverged earlier, closer to 0.5 means it diverged later, 1 means it is (likely) in the mandelbrot set
    integer :: indices_to_test(5, 2)
    logical :: diverges, last_point_was_in_mandelbrot   ! a dead flag for dce
    !for indices (400, 1000), (400, 600), (1100, 900), (825, 1317), (825, 1310)
    data indices_to_test /400, 400, 1100, 825, 825, 1000, 600, 900, 1317, 1310/

    max_iter = 1000   !number of iterations before we stop and say it diverged
    diverges = .false.

    width = 2000
    height = 2000

    x_start = -2.0
    x_end = 2.0
    y_start = -2.0
    y_end = 2.0

    !initialize the array
    do i = 1, width
        do j = 1, height
            in_mandelbrot(i, j) = 0
        end do
    end do

    do i = 1, width
        do j = 1, height
            x_0 = x_start + (i-1)*(x_end-x_start)/width
            y_0 = y_start + (j-1)*(y_end-y_start)/height

            x = x_0
            y = y_0

            diverges = .false.

            do iter = 1, max_iter
                x_new = x*x - y*y + x_0
                y_new = 2*x*y + y_0

                if ((x_new*x_new + y_new*y_new)**0.5 > 2.0) then
                    diverges = .true. 
                    exit
                end if

                x = x_new
                y = y_new
            end do

            if (iter == max_iter+1) then
                in_mandelbrot(i, j) = 1   !point is in the mandelbrot set since it didn't diverge
            else if (iter < max_iter/10) then
                in_mandelbrot(i, j) = 0   ! diverged early
            else if (iter < max_iter/5) then
                in_mandelbrot(i, j) = 0.2
            else if (iter < max_iter/4) then
                in_mandelbrot(i, j) = 0.4
            else if (iter < max_iter/2) then
                in_mandelbrot(i, j) = 0.49
            else
                in_mandelbrot(i, j) = 0.5   ! diverged later
            end if

            ! some redundant loading values in_mandelbrot(i, j)
            ! some dead code - setting a random flag
            if (in_mandelbrot(i, j) == 1) then
                last_point_was_in_mandelbrot = .true.
            else if (in_mandelbrot(i, j) == 0) then
                last_point_was_in_mandelbrot = .false.
            else if (in_mandelbrot(i, j) == 0.2) then
                last_point_was_in_mandelbrot = .false.
            else if (in_mandelbrot(i, j) == 0.4) then
                last_point_was_in_mandelbrot = .false.
            else if (in_mandelbrot(i, j) == 0.49) then
                last_point_was_in_mandelbrot = .false.
            end if
        end do
    end do
    
    

    !print the array for some values

    do i = 1, 5
        x_0_index = indices_to_test(i, 1)
        y_0_index = indices_to_test(i, 2)

            x_part = x_start + (x_0_index-1)*(x_end-x_start)/width
            y_part = y_start + (y_0_index-1)*(y_end-y_start)/height

            print *, "x_part = ", x_part, "y_part = ", y_part
            if (in_mandelbrot(x_0_index, y_0_index) == 1) then
                print *, "The point is in the mandelbrot set"
            else if (in_mandelbrot(x_0_index, y_0_index) == 0) then
                print *, "The point is not in the mandelbrot set"
                print *, "The point diverged early"
            else
                print *, "The point is not in the mandelbrot set", "Diverged quite late"
                print *, "color value = ", in_mandelbrot(x_0_index, y_0_index)
            end if

            print *, "--------------------------------"
    end do
            

    





end program mandelbrot
