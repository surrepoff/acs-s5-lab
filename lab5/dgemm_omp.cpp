#include <chrono>
#include <fstream>
#include <iostream>
#include <omp.h>

using namespace std;

void dgemm_omp(double *a, double *b, double *c, int size, int num_th) {
  // OMP
  int i, j, k;
#pragma omp parallel private(i, j, k) shared(a, b, c) num_threads(num_th)
  {
    int threadId = omp_get_thread_num(); // Amount of threads
    int itemsPerThread = size / num_th;
    int lb = threadId * itemsPerThread;
    int ub = (threadId == num_th - 1) ? (size) : (lb + itemsPerThread);
#pragma omp for schedule(static)
    for (i = 0; i < size; ++i) {
      for (k = 0; k < size; ++k) {
        for (j = 0; j < size; ++j) {
          *(c + i * size + j) += *(a + i * size + k) * *(b + k * size + j);
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  int size = 1000;
  int num_th = 10;

  size = std::stol(std::string(argv[1]));
  num_th = std::stol(std::string(argv[2]));

  double *a, *b, *c;
  a = new double[size * size];
  b = new double[size * size];
  c = new double[size * size];

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      *(a + i * size + j) = rand() % 1000 + rand() / 1000;
      *(b + i * size + j) = rand() % 1000 + rand() / 1000;
      // *(a + i * size + j) = j;
      // if (i == j) *(b + i * size + j) = 1;
      // else *(b + i * size + j) = 0;
    }
  }
  auto r_tnow = std::chrono::high_resolution_clock::now();
  dgemm_omp(a, b, c, size, num_th);
  auto r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - r_tnow);
  cout << "OMP " << size << " " << num_th << " "
       << (double)r_time.count() * 1e-9 << "\n";

  ofstream out("omp.csv", ios_base::app);
  out << "OMP" << size << ";" << num_th << ";" << (double)r_time.count() * 1e-9
      << ";\n";
  out.close();
  return 0;
}