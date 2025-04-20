program copyconst

    integer :: a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
    integer :: count, count2

    a = 1
    b = 2
    c = 3


    do 
        g = a
        h = b
        i = c
        j = a
        k = b
        l = c

        if (count == 1000000) then
            exit
        end if
        count = count + 1
        
    end do

    do count2 = 1, 1000
        do count = 1, 1000000
            m = g
            n = h
            o = i
            p = j
            q = k
            r = l
        end do
    end do

    print *, m, n, o, p, q, r
end program copyconst