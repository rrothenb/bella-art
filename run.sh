bin/Darwin/surface --frame:$1
/Applications/bella_cli.app/Contents/MacOS/bella_cli -i:surface_frame_$1.bsa &
PID=$!
sleep 300  # desired render time in seconds
kill -INT $PID
wait $PID
rm surface_frame_$1.bsa
rm surface_frame_$1.bsi
rm surface_env_$1.hdr