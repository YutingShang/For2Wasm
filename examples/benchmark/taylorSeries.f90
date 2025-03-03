program taylorSeries
    implicit none
    double precision :: x, term, e_x, factorial, sign, sin_x, cos_x, tan_x, ln_x, arctan_x
    integer :: numTerms, i, j, temp, repeat

    print *, "Calculating Taylor Series for a value of x around 0"
    x = 0.123456789

    repeat = 100000000

    do i = 1, repeat

        x = x + 0.000000001

        e_x = 1+x/1+(x*x)/(1*2)+(x*x*x)/(1*2*3)+(x*x*x*x)/(1*2*3*4)+(x*x*x*x*x)/(1*2*3*4*5)+(x*x*x*x*x*x)/(1*2*3*4*5*6)+(x*x*x*x*x*x*x)/(1*2*3*4*5*6*7)+(x*x*x*x*x*x*x*x)/(1*2*3*4*5*6*7*8)+(x*x*x*x*x*x*x*x*x)/(1*2*3*4*5*6*7*8*9)+(x*x*x*x*x*x*x*x*x*x)/(1*2*3*4*5*6*7*8*9*10)

        sin_x = x-(x*x*x)/(1*2*3)+(x*x*x*x*x)/(1*2*3*4*5)-(x*x*x*x*x*x*x)/(1*2*3*4*5*6*7)+(x*x*x*x*x*x*x*x*x)/(1*2*3*4*5*6*7*8*9)-(x*x*x*x*x*x*x*x*x*x*x)/(1*2*3*4*5*6*7*8*9*10)

        cos_x = 1-(x*x)/(1*2)+(x*x*x*x)/(1*2*3*4)-(x*x*x*x*x*x*x)/(1*2*3*4*5*6*7)+(x*x*x*x*x*x*x*x*x)/(1*2*3*4*5*6*7*8*9)-(x*x*x*x*x*x*x*x*x*x*x)/(1*2*3*4*5*6*7*8*9*10)

        ln_x = (x-1)-(x-1)*(x-1)/2+(x-1)*(x-1)*(x-1)/3-(x-1)*(x-1)*(x-1)*(x-1)/4+(x-1)*(x-1)*(x-1)*(x-1)*(x-1)/5-(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)/6+(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)/7-(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)/8+(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)/9-(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)*(x-1)/10

        arctan_x = x-(x*x*x)/3+(x*x*x*x*x)/5-(x*x*x*x*x*x*x)/7+(x*x*x*x*x*x*x*x*x)/9-(x*x*x*x*x*x*x*x*x*x*x)/11

    end do

    print *, "For x = ", x

    print *, "e^x = ", e_x

    print *, "sin(x) = ", sin_x

    print *, "cos(x) = ", cos_x

    print *, "ln(x) = ", ln_x

    print *, "arctan(x) = ", arctan_x

end program taylorSeries