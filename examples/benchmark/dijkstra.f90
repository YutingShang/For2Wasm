program dijkstra_algorithm
    implicit none
    integer :: n, INT_MAX
    integer :: graph(10000, 10000), distances(10000), start
    logical :: visited(10000)
    integer :: i, j, min_dist, min_index, edge_weight, path_length
    logical :: has_edge

    n = 10000
    INT_MAX = 999999
    
    ! Initialize graph (for simplicity)
    do i = 1, n
        do j = 1, n
            if (i == j) then
                graph(i, j) = 0
            else if (i + j < 10000) then
                graph(i, j) = i + j
            else
                graph(i, j) = 0
            end if
        end do
    end do
    
    ! Set starting vertex
    start = 1
    
    ! Initialize distances and visited
    do i = 1, n
        distances(i) = INT_MAX
        visited(i) = .false.
    end do
    distances(start) = 0
    
    i=1
    do 
        
        ! Find minimum distance vertex (not visited)
        min_dist = INT_MAX
        min_index = -1
        
        do j = 1, n
            if (.not. visited(j) .and. distances(j) <= min_dist) then
                min_dist = distances(j)
                min_index = j
            end if
        end do
        
        if (min_index == -1 .or. i>=n) then     !each vertex is popped from the queue once
            exit
        end if
        
        ! Mark selected vertex as visited
        visited(min_index) = .TRUE.
        
        ! Update distances of adjacent vertices distances if going through min_index is shorter than current distance
        do j = 1, n
            edge_weight = graph(min_index, j)
            has_edge = edge_weight > 0
            
            if (.not. visited(j) .and. has_edge .and. &
                distances(min_index) /= INT_MAX .and. &
                distances(min_index) + edge_weight < distances(j)) then
                
                distances(j) = distances(min_index) + edge_weight
                
                ! Unused debug output (dead code)
                path_length = 0
                path_length = path_length + 1  
            end if
        end do
        i=i+1
    end do
    
    ! Output results
    print *, "Shortest distances from start vertex", start
    print *, "================"

    do i = 1, n
        print *, "To vertex", i, "is", distances(i)
        print *, "--------------"
    end do
end program dijkstra_algorithm