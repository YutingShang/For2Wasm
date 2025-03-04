!another test of arithmetic operations and especially casting and loading, which requires extra care and code

program arithmetic2

    integer :: i, j, k
    real :: a, b, c, array(3)
    logical :: l
    logical(2) :: b4
    logical(8) :: b8, b9
    data array /1.0, 2.0, 3.0/


    i = 1
    j = 2
    a = b + i + j    !this currently works
    a = b + (i+j)    !this does not work, but should
    c = (i+j) + (i+k)
    a = (i+j) + (i+k)     !i think this also would not work - but it does because it does the casting to the answer of big sum...
    a = (b+ i) + (i+j)    !this also works fine
    a = (1+2) + (4.5+7.1)
    a = (1+2) - (4.5+7.1)
    l = (1+2) == (4.5+7.1)
    l = (1+2) >= (4.5+7.1)

    !!problematic ones, - the operand at the bottom of the stack needs to convert data type (i.e. need to add code to it)
    ! should work now - I handle commutative and non-commutative operations differently
    l = (1.4+2) == (4.5+7.1)
    l = (1.4+2) /= (4 + 9)        !not equal to
    l = 5.42 == (4.1 + 10)
    l = 5 == (4.1 + 10)
    b4 = .false.
    b8 = .true.
    b9 = (b4 .and. b4) .and. (b8 .or. b8)     !!different logical datatypes i32 or i64
    b9 = b8 .and. (b4 .or. b4)
    print *, "a = ", a


    array(1+2) = array(5-4)
    print *, "array = ", array

    array(i) = 1.0
    array(i) = array(j) 
    array(i+2) = k
    array(i+2) = array(i)    !problematic, but should work


end program arithmetic2