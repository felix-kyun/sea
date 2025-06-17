# Sea Launcher

By default sea launcher expects the window manager to open it in floating mode.
In niri, you can ensure this by adding the following to your niri config:

```kdl
window-rule {
    match app-id="^kitty$" title="^sea-shell*$"
    open-floating true
}

```

Most of the dependecies are pre installed in most \*nix.
Dependencies:

- fzf
- kitty
- find
- grep
- cut
- sed
- setsid
