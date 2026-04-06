bin/Darwin/series2 --frame:$1
/Applications/bella_cli.app/Contents/MacOS/bella_cli -i:series2_frame_$1.bsa &
PID=$!
sleep $2
kill -INT $PID
wait $PID
rm series2_frame_$1.bsa
rm series2_frame_$1.bsi
rm series2_env_$1.hdr
rm -f series2_blend_$1.hdr