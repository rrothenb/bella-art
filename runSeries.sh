make series1
for frame in $(seq 0 512)
do
  time ./run.sh "$frame" 300s
done
