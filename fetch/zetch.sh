#! /usr/bin/env bash 

# deps 
# - figlet
# - toilet ?
# - awk 
# - grep 
# - cut 

# setting ?
# You want settings ?
# Look no further, adjust the variables below lmao

CURRENT_DIR=$(dirname "${BASH_SOURCE[0]}")
source $CURRENT_DIR/../utils/formatting.sh
COLUMN=6

print_mod() {
  # format to be used 
  # print_mod(COLOR, ICON, NAME, VALUE)
  
  echo -ne "${BOLD}│ "

  # print the colored icon 
  echo -ne "${1}${2} ${WHITE}"

  printf " %-${COLUMN}s" $3

  echo -ne " │ ${1}"
  shift 3
  echo -e "${@}${RESET}"
}

dummy_print() {
  echo "dummy text"
}

show_top() {
  echo -n "╭"

  for (( i = 1; i <= ${COLUMN} + 5; i++))
  do 
    echo -n "─"
  done

  echo "╮"
}

show_bottom() {
  echo -n "╰"

  for (( i = 1; i <= ${COLUMN} + 5; i++))
  do 
    echo -n "─"
  done

  echo "╯"
}

show_user() {
  echo $USER
}

show_hname() {
  cat /etc/hostname
}

show_pretty_disro() {
    cat /etc/os-release | grep "PRETTY_NAME=" | cut -d "=" -f 2 | sed 's/"//g; s/(.*)//g'
}

show_kernel() {
  uname -r
}

show_shell() {
  echo $SHELL
}

show_packages() {
  if $(command -v apt 1>/dev/null); then 
    echo $(dpkg -–list | wc -l) "(deb)"
  elif $(command -v pacman 1>/dev/null); then 
    echo $(pacman -Q | wc -l) "(pacman)"
  elif $(command -v dnf 1>/dev/null); then 
    echo $(dnf list installed | wc -l) "(rpm)"
  fi
}

show_uptime() {
  uptime -p
}

show_memory() {
  echo $(free -h | awk '/Mem:/{print $3}') "|" $(free -h | awk '/Mem:/{print $2}')
}


load_async() {
  ZETCH_TMP=$(mktemp -d)

  show_user > $ZETCH_TMP/zetch_user.tmp &
  show_hname > $ZETCH_TMP/zetch_hname.tmp &
  show_pretty_disro > $ZETCH_TMP/zetch_pretty_disro.tmp &
  # show_kernel > $ZETCH_TMP/zetch_kernel.tmp &
  # show_shell > $ZETCH_TMP/zetch_shell.tmp &
  # show_packages > $ZETCH_TMP/zetch_packages.tmp &
  show_uptime > $ZETCH_TMP/zetch_uptime.tmp &
  show_memory > $ZETCH_TMP/zetch_memory.tmp &

  # wait for all background jobs to finish
  wait

  export ZETCH_USER=$(cat $ZETCH_TMP/zetch_user.tmp)
  export ZETCH_HNAME=$(cat $ZETCH_TMP/zetch_hname.tmp)
  export ZETCH_PRETTY_DISTR=$(cat $ZETCH_TMP/zetch_pretty_disro.tmp)
  # export ZETCH_KERNEL=$(cat $ZETCH_TMP/zetch_kernel.tmp)
  # export ZETCH_SHELL=$(cat $ZETCH_TMP/zetch_shell.tmp)
  # export ZETCH_PACKAGES=$(cat $ZETCH_TMP/zetch_packages.tmp)
  export ZETCH_UPTIME=$(cat $ZETCH_TMP/zetch_uptime.tmp)
  export ZETCH_MEMORY=$(cat $ZETCH_TMP/zetch_memory.tmp)
}


main() {
  # clear
  # echo ""
  # show_logo
  show_top
  load_async

  # define modules here
  print_mod $RED "" "user" $ZETCH_USER
  print_mod $YELLOW "󰍹" "hname" $ZETCH_HNAME
  print_mod $GREEN "󰌽" "distro" $ZETCH_PRETTY_DISTR
  # print_mod $CYAN "" "kernel" $ZETCH_KERNEL
  print_mod $BLUE "" "uptime" $ZETCH_UPTIME
  # print_mod $MAGENTA "" "shell" $ZETCH_SHELL
  # print_mod $RED "󰏔" "pkgs" $ZETCH_PACKAGES
  print_mod $YELLOW "󰍛" "memory" $ZETCH_MEMORY

  show_bottom
}

debug() {
    print_mod $RED "" "debug" "This is a debug message"
}

main
