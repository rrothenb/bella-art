make series3
for frame in $(seq 0 511)
do
  time ./run3.sh "$frame" 300s
done
