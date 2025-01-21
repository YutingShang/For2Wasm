! This program is an example of common subexpression elimination.
! the expression (1+2+3+4+5+6+7+8+variable) * (1+2+3+4+5+6+7+8+variable) is computed in all 3 branches
! so it is available after the if statement where it is computed again
! no dead code until you do CSE and CP, followed by DCE

program common2
    implicit none
    integer :: flag, y, z, variable, temp

    print *, "Enter 1 or 2 to determine which branch is executed"
    read *, flag

    variable = 100

    ! both branches do not affect the expression
    if (flag == 1) then
        variable = 0
        y = (1+2+3+4+5+6+7+8+variable) * (1+2+3+4+5+6+7+8+variable)
        temp = y
        print *, "(1+2+3+4+5+6+7+8) ^ 2 ="
    else 
        if (flag == 2) then
            variable = 9
            y = (1+2+3+4+5+6+7+8+variable) * (1+2+3+4+5+6+7+8+variable)
            temp = y
            print *, "(1+2+3+4+5+6+7+8+9) ^ 2 ="
        else
            z = (1+2+3+4+5+6+7+8+variable) * (1+2+3+4+5+6+7+8+variable)
            temp = z
            print *, "Invalid input"
            variable = 10
        end if
        
    end if

    print *, temp
    z = (1+2+3+4+5+6+7+8+variable) * (1+2+3+4+5+6+7+8+variable)
    print *, z
   

end program common2