make series$1
for frame in 134 192 211 248 250 255
do
  time ./run.sh $1 "$frame" $3 $4 $2 $5
done
