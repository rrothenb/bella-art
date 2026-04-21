make series$1
for frame in $(seq 0 $(($2 - 1)))
do
  time ./run.sh $1 "$frame" $3 $4 $2 $5
done
