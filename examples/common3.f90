! This program has 1 positive and 1 negative example of common subexpression elimination.
! The expression R = P .AND. Q is computed twice, once before the if statement and once after the if statement
! However one of the branches invalidates the expression by changing the value of P, so it is not available after the if statement

! Although the option==1 expression is eliminated by CSE - both branches do not affect the expression, so it should still be available after the if statement

program common3
    implicit none
    integer :: P, Q, R, option

    P = 3 > 5
    Q = 7 < 10
    R = P .AND. Q
    print *, " (3 > 5) AND (7 < 10) =", R

    print *, "Enter option 0 or 1 :"
    read *, option

    if (option == 1) then
        P = 3 > 2
    end if

    R = P .AND. Q
    if (option == 1) then
        print *, " (3 > 2) AND (7 < 10) =", R
    else
        print *, " (3 > 5) AND (7 < 10) =", R
    end if

end program common3