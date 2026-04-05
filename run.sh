bin/Darwin/surface --frame:$1
/Applications/bella_cli.app/Contents/MacOS/bella_cli -i:surface_frame_$1.bsa &
PID=$!
sleep $2
kill -INT $PID
wait $PID
rm surface_frame_$1.bsa
rm surface_frame_$1.bsi
rm surface_env_$1.hdr