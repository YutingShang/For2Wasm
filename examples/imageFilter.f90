program convolution_filter
    implicit none
    integer :: width, height
    real :: src(10000, 10000), dst(10000, 10000)
    real :: kernel_x(3, 3), kernel_y(3, 3)
    integer :: x, y, kx, ky
    real :: sum_x, sum_y, magnitude, angle, normalized
    real :: pixel, pi

    width = 10000    
    height = 10000
   
    
    ! Initialize kernel_x and kernel_y
    kernel_x(1, 1) = -1.0; kernel_x(1, 2) = 0.0; kernel_x(1, 3) = 1.0
    kernel_x(2, 1) = -2.0; kernel_x(2, 2) = 0.0; kernel_x(2, 3) = 2.0
    kernel_x(3, 1) = -1.0; kernel_x(3, 2) = 0.0; kernel_x(3, 3) = 1.0
    
    kernel_y(1, 1) = -1.0; kernel_y(1, 2) = -2.0; kernel_y(1, 3) = -1.0
    kernel_y(2, 1) = 0.0;  kernel_y(2, 2) = 0.0;  kernel_y(2, 3) = 0.0
    kernel_y(3, 1) = 1.0;  kernel_y(3, 2) = 2.0;  kernel_y(3, 3) = 1.0
    
    pi = 3.14159265359
    
    ! Read input image (simulated)
    do y = 1, height
        do x = 1, width
            src(x, y) = (x + y) / 100.0
        end do
    end do
    
    ! Apply convolution
    do y = 2, height-1
        do x = 2, width-1
            sum_x = 0.0
            sum_y = 0.0
            
            ! Common subexpressions in pixel neighborhood access
            do ky = -1, 1
                do kx = -1, 1
                    ! This calculation is repeated in multiple places
                    pixel = src(x+kx, y+ky)
                    
                    ! Same calculations computed multiple times
                    sum_x = sum_x + pixel * kernel_x(ky+2, kx+2)
                    sum_y = sum_y + src(x+kx, y+ky) * kernel_y(ky+2, kx+2)
                end do
            end do
            
            ! Redundant calculations of magnitude without sqrt function
            ! Using a simple approximation for magnitude
            magnitude = sum_x * sum_x + sum_y * sum_y
            if (magnitude > 0.0) then
                magnitude = magnitude ** 0.5  ! Approximation of sqrt
            end if
            
            ! Use the magnitude
            dst(x, y) = magnitude
            
            ! Dead code - debug outputs never used
            if (sum_x /= 0.0) then
                angle = sum_y / sum_x
                if (sum_x < 0.0) then
                    if (sum_y >= 0.0) then
                        angle = angle + pi
                    else
                        angle = angle - pi
                    end if
                end if
                angle = angle * 180.0 / pi
            else
                if (sum_y > 0.0) then
                    angle = 90.0
                else if (sum_y < 0.0) then
                    angle = -90.0
                else
                    angle = 0.0
                end if
            end if
            
            normalized = magnitude / 4.0
        end do
    end do
    
    ! Output result
    print *, "Convolution completed. Sample values:"
    print *, dst(100, 100), dst(150, 150), dst(200, 200), dst(2000, 2000)
end program convolution_filter