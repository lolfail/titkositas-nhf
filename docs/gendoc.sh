#!/bin/bash

# start ydotool socket
ydtsock="$HOME/.ydotool_socket" 
sudo -b ydotoold --socket-path=$ydtsock --socket-own="$(id -u):$(id -g)" >/dev/null

echo Generating $output_file
make doc
echo Generated $output_file

echo Sending command to refresh Firefox
U=0
D=1

META_L=125
KEY_L=38
NAVRIGHT="$META_L:$D $KEY_L:$D $KEY_L:$U $META_L:$U"
YDOTOOL_SOCKET=$ydtsock ydotool key $NAVRIGHT >/dev/null;

sleep 0.1;
KEY_F5=63
REFRESH="$KEY_F5:$D $KEY_F5:$U"
YDOTOOL_SOCKET=$ydtsock ydotool key $REFRESH >/dev/null;
echo Sent

# kill ydotool socket
sudo killall ydotoold
sudo rm $HOME/.ydotool_socket
