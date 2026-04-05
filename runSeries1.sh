make series1
for frame in 20 93 143 166 223 250 310 311 312 313 334 345 421 444 485 506
do
  time ./run.sh "$frame" 4h
done
