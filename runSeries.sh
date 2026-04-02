for frame in $(seq 0 512)
do
  time ./run.sh "$frame"
done
