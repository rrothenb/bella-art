bin/Darwin/series$1 --frame:$2 --desiredtriangles:$4 --maxframes:$5
/Applications/bella_cli.app/Contents/MacOS/bella_cli -i:series3_frame_$2.bsa -res:$6 &
PID=$!
sleep $3
kill -INT $PID
wait $PID
rm series$1_frame_$2.bsa
rm series$1_frame_$2.bsi
rm series$1_env_$2.hdr
rm -f series$1_blend_$2.hdr
convert series3_frame_$2.png -auto-gamma series3_frame_$2.jpg
rm series3_frame_$2.png
rm series3_mesh_$2.obj