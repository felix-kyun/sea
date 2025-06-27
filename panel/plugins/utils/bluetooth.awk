/Device .* Connected:/ {
    match($0, /Device .* Connected: ([a-z]+)/, status);
    match($0, /Device ([A-Z0-9:]+) Connected:/, device);

    if (status[1] == "yes") {
        printf "connected|%s\n", device[1];
    } else {
        printf "disconnected|%s\n", device[1];
    }

}
