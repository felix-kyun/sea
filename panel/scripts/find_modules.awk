#!/usr/bin/awk -f

/example/ {
    next
}

{
    if (NR > 1) 
        print "    " prev ","
    prev=$0
} 

END {
    print "    " prev 
}
