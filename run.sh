bin/Darwin/series$1 --frame:$2 --desiredtriangles:$4 --maxframes:$5
/Applications/bella_cli.app/Contents/MacOS/bella_cli -i:series$1_frame_$2.bsa -res:$6 &
PID=$!
sleep $3
kill -INT $PID
wait $PID
rm series$1_frame_$2.bsa
rm series$1_frame_$2.bsi
rm series$1_env_$2.hdr
rm -f series$1_blend_$2.hdr
convert series$1_frame_$2.png -auto-gamma series$1_frame_$2.jpg
rm series$1_frame_$2.png
mv series$1_mesh_$2.obj /tmp