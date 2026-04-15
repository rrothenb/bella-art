make series$1
for frame in 5 17 68 123 194 223 232 252 301 302 314 319 331 353 405 539 
do
  time ./run.sh $1 "$frame" $3 $4 $2 $5
done
