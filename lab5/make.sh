g++ dgemm_singlethread.cpp -o singlethread.out
g++ dgemm_omp.cpp -o omp.out -fopenmp 
g++ dgemm_posix.cpp -o posix.out -pthread