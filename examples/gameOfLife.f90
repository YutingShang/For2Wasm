! running PRE makes it take longer
! regression test for fixed error - ifElseNode was not being added to the parent at the correct position
! another regression test for constant propagation - LoadElt and StoreElt did not have replaceReferencedVariable implemented

program game_of_life
    implicit none
    integer :: i, j, ni, nj, gen, neighbors, alive_cells, grid(50, 50), new_grid(50, 50)
    integer :: size_i, size_j, generations
    
    ! Set simulation parameters
    size_i = 50
    size_j = 50
    generations = 1000000
    
    ! Declare grid arrays - current and next generation
    !integer :: grid(50, 50), new_grid(50, 50)
    
    ! Initialize all cells to dead (0)
    do i = 1, size_i
        do j = 1, size_j
            grid(i, j) = 0
            new_grid(i, j) = 0
        end do
    end do
    
    ! Set up a glider pattern
    grid(2, 3) = 1
    grid(3, 4) = 1
    grid(4, 2) = 1
    grid(4, 3) = 1
    grid(4, 4) = 1
    
    ! Add some additional patterns
    ! Blinker
    grid(20, 20) = 1
    grid(20, 21) = 1
    grid(20, 22) = 1
    
    ! Block
    grid(30, 30) = 1
    grid(30, 31) = 1
    grid(31, 30) = 1
    grid(31, 31) = 1
    
    ! Beehive
    grid(10, 40) = 1
    grid(11, 39) = 1
    grid(11, 41) = 1
    grid(12, 39) = 1
    grid(12, 41) = 1
    grid(13, 40) = 1
    
    ! Main simulation loop
    do gen = 1, generations
        ! Compute next generation
        do i = 1, size_i
            do j = 1, size_j
                ! Count living neighbors
                neighbors = 0
                
                ! Top row
                if (i > 1) then
                    if (j > 1) then
                        neighbors = neighbors + grid(i-1, j-1)
                    end if
                    
                    neighbors = neighbors + grid(i-1, j)
                    
                    if (j < size_j) then
                        neighbors = neighbors + grid(i-1, j+1)
                    end if
                end if
                
                ! Middle row (left and right)
                if (j > 1) then
                    neighbors = neighbors + grid(i, j-1)
                end if
                
                if (j < size_j) then
                    neighbors = neighbors + grid(i, j+1)
                end if
                
                ! Bottom row
                if (i < size_i) then
                    if (j > 1) then
                        neighbors = neighbors + grid(i+1, j-1)
                    end if
                    
                    neighbors = neighbors + grid(i+1, j)
                    
                    if (j < size_j) then
                        neighbors = neighbors + grid(i+1, j+1)
                    end if
                end if
                
                ! Apply game of life rules
                ! Rule 1: Any live cell with fewer than two live neighbors dies (underpopulation)
                ! Rule 2: Any live cell with two or three live neighbors lives
                ! Rule 3: Any live cell with more than three live neighbors dies (overpopulation)
                ! Rule 4: Any dead cell with exactly three live neighbors becomes alive (reproduction)
                
                if (grid(i, j) == 1) then
                    ! Cell is alive
                    if (neighbors < 2) then
                        new_grid(i, j) = 0  ! Dies from underpopulation
                    else 
                        if (neighbors > 3) then
                            new_grid(i, j) = 0  ! Dies from overpopulation
                        else
                            new_grid(i, j) = 1  ! Survives
                        end if
                    end if
                else
                    ! Cell is dead
                    if (neighbors == 3) then
                        new_grid(i, j) = 1  ! Becomes alive from reproduction
                    else
                        new_grid(i, j) = 0  ! Stays dead
                    end if
                end if
            end do
        end do
        
        ! Update grid for next generation
        do i = 1, size_i
            do j = 1, size_j
                grid(i, j) = new_grid(i, j)
            end do
        end do
    end do
    
    ! Count alive cells at the end
    alive_cells = 0
    do i = 1, size_i
        do j = 1, size_j
            alive_cells = alive_cells + grid(i, j)
        end do
    end do
    
    ! Print results
    print *, "After", generations, "generations:"
    print *, "Living cells =", alive_cells
    
end program game_of_life
