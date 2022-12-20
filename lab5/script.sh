g++ dgemm_singlethread.cpp -o singlethread.out
echo "Type;MatrixSize;Time;" > ./singlethread.csv

g++ dgemm_omp.cpp -o omp.out -fopenmp 
echo "Type;MatrixSize;ThreadsCount;Time;" > ./omp.csv

g++ dgemm_posix.cpp -o posix.out -pthread
echo "Type;MatrixSize;ThreadsCount;Time;" > ./posix.csv

minsize=4000
maxsize=4000
stepsize=2

minthread=2
maxthread=16
stepthread=2

for (( i=$minsize; i<=$maxsize; i*=$stepsize ))
do
./singlethread.out $i
for (( j=$minthread; j<=$maxthread; j*=$stepthread ))
do
./posix.out $i $j
./omp.out $i $j
done
done