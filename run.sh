bin/Darwin/series1 --frame:$1
/Applications/bella_cli.app/Contents/MacOS/bella_cli -i:series1_frame_$1.bsa &
PID=$!
sleep $2
kill -INT $PID
wait $PID
rm series1_frame_$1.bsa
rm series1_frame_$1.bsi
rm series1_env_$1.hdr