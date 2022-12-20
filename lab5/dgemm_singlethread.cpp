#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

void dgemm_singlethread(double *a, double *b, double *c, int size) {
  for (int i = 0; i < size; ++i) {
    for (int k = 0; k < size; ++k) {
      for (int j = 0; j < size; ++j) {
        *(c + i * size + j) += *(a + i * size + k) * *(b + k * size + j);
      }
    }
  }
}

int main(int argc, char **argv) {
  int size = 1000;

  size = std::stol(std::string(argv[1]));

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
  dgemm_singlethread(a, b, c, size);
  auto r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - r_tnow);
  cout << "SingleThread " << size << " " << (double)r_time.count() * 1e-9
       << "\n";

  ofstream out("singlethread.csv", ios_base::app);
  out << "SingleThread" << size << ";" << (double)r_time.count() * 1e-9
      << ";\n";
  out.close();
  return 0;
}