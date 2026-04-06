bin/Darwin/series3 --frame:$1
/Applications/bella_cli.app/Contents/MacOS/bella_cli -i:series3_frame_$1.bsa &
PID=$!
sleep $2
kill -INT $PID
wait $PID
rm series3_frame_$1.bsa
rm series3_frame_$1.bsi
rm series3_env_$1.hdr
rm -f series3_blend_$1.hdr
