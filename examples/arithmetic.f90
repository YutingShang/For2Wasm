! some cases to test out the different data types and their arithmetic operations
! integer, integer(8), real, double precision

! correct results from gfortran:
! 2147483647 + 2147483647 =           -2
! 214748364700000 + 2147483647 =       214750512183647
! pi + e =    5.85987473    
! 42*10^42 + phi =    4.2000000000000005E+043
! pi - 2147483647 =   -2.14748365E+09
! 42*10^42 / 2147483647 =    1.9557774076032349E+034
! 2147483647 / pi  =             683565248
! 20-10*5/3/7 =    17.6190472    
! (20-10)*5/3/7 =    2.38095236    
! 20-(10*5)/3/7 =    17.6190472    
! (20-10)*(5/3/7) =    2.38095236    
! 20-10*(5/(3/7)) =   -96.6666718  

program arithmetic
    implicit none
    integer :: p, q, r, i
    integer(8) :: s, t
    real :: u, v, w
    real(8) :: x  !should be the same as double precision
    double precision :: y
    doubleprecision z
    real :: a, b, c, d, e, k

    !!!!test out some basic operations on different datatypes

    p = 2147483647
    q = 2147483647
    r = p + q
    print *, "2147483647 + 2147483647 = ", r

    s = 214748364700000_8  !tag it for 8 byte integer!
    t = s + q
    print *, "214748364700000 + 2147483647 = ", t

    u = 3.141592653589793238462643383279502884197
    v = 2.718281828459045235360287471352662497757
    w = u + v
    print *, "pi + e = ", w

    x = 42
    do i=1, 42
        x = x*10
    end do
    y = 1.618033988749894848204586834365638117720_8
    z = x + y
    print *, "42*10^42 + phi = ", z


    !!!!test out some other operations, including mixing types

    w = u - q     !real - integer
    print *, "pi - 2147483647 = ", w

    z = x / p     !double precision / integer
    print *, "42*10^42 / 2147483647 = ", z

    t = p / u      !integer / real -> truncated to integer(8)
    print *, "2147483647 / pi  = ", t

    
    !!!!test some ordered operations
    
    a = 20
    b = 10
    c = 5
    d = 3
    e = 7


    k = a-b*c/d/e
    print *, "20-10*5/3/7 = ", k

    k = (a-b)*c/d/e
    print *, "(20-10)*5/3/7 = ", k

    k = a-(b*c)/d/e
    print *, "20-(10*5)/3/7 = ", k

    k = (a-b)*(c/d/e)
    print *, "(20-10)*(5/3/7) = ", k

    k = a-b*(c/(d/e))
    print *, "20-10*(5/(3/7)) = ", k


    !!!!test some negative numbers

    p = -2147483647
    q = -2147483647
    r = p + q
    print *, "-2147483647 + -2147483647 = ", r

    x = -1.234567890_8
    y = - 100.001_8
    z = x * y / (x + y)
    print *, "(-1.234567890_8) * (-100.001_8) / ((-1.234567890_8) + (-100.001_8)) = ", z

    p = -3
    x = -(-3.14)
    q = 3 + 8 / ( 1 - 5 )
    print *, "-3 = ", p
    print *, "-(-3.14) = ", x
    print *, "3 + 8 / ( 1 - 5 ) = ", q

    print *, 1-(9.999),x, 3+p, 4/(-3), -(-3), q
    
    z = 12/ (16/(8-4.01))
    print *, "12/ (16/(8-4)) = ", z
    
    
end program arithmetic