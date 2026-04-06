make series2
for frame in $(seq 0 999)
do
  time ./run2.sh "$frame" 300s
done