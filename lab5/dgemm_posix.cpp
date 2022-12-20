#include <chrono>
#include <fstream>
#include <iostream>
#include <pthread.h>

using namespace std;
struct arg_matrix {
  double *a;
  double *b;
  double *c;
  int n;
};

int rows;
int count = 0;
int num_th;
int help;

void *dgemm_posix(void *_matrix) {
  int i, j, k;
  int start;
  int end;
  int thread_num = count++;
  arg_matrix matrix = (*(arg_matrix *)_matrix);
  if (matrix.n % num_th > 0) {
    if (thread_num < num_th - 1) {
      rows = matrix.n / num_th;
      start = thread_num * rows;
      end = start + rows;
      help = end;
    }
    if (thread_num == num_th - 1) {
      rows = matrix.n / num_th;
      rows += matrix.n % num_th;
      start = help;
      end = matrix.n;
    }
  } else {
    rows = matrix.n / num_th;
    start = thread_num * rows;
    end = start + rows;
  }

  for (i = start; i < end; ++i) {
    for (k = 0; k < matrix.n; ++k) {
      for (j = 0; j < matrix.n; ++j) {
        *(matrix.c + i * matrix.n + j) +=
            *(matrix.a + i * matrix.n + k) * *(matrix.b + k * matrix.n + j);
      }
    }
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  int size = 1000;
  num_th = 10;

  size = std::stol(std::string(argv[1]));
  num_th = std::stol(std::string(argv[2]));

  pthread_t threads_id[num_th];
  arg_matrix matrix;
  matrix.a = new double[size * size];
  matrix.b = new double[size * size];
  matrix.c = new double[size * size];
  matrix.n = size;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      *(matrix.a + i * size + j) = rand() % 1000 + rand() / 1000;
      *(matrix.b + i * size + j) = rand() % 1000 + rand() / 1000;
      // *(matrix.a + i * size + j) = j;
      // if (i == j) *(matrix.b + i * size + j) = 1;
      // else *(matrix.b + i * size + j) = 0;
    }
  }
  auto r_tnow = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_th; i++) {
    pthread_create(&threads_id[i], NULL, dgemm_posix, &matrix);
  }
  for (int i = 0; i < num_th; i++) {
    pthread_join(threads_id[i], NULL);
  }
  auto r_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - r_tnow);
  /*
   for (int i = 0; i < size * size; i++) {

            cout<<matrix.a[i]<<" ";
            if ((i + 1) % size == 0)
            cout<<"\n";
    }
    cout<<"\n\n";
    for (int i = 0; i < size * size; i++) {

            cout<<matrix.b[i]<<" ";
            if ((i + 1) % size == 0)
            cout<<"\n";
    }
    cout<<"\n\n";
    for (int i = 0; i < size * size; i++) {

            cout<<matrix.c[i]<<" ";
            if ((i + 1) % size == 0)
            cout<<"\n";
    }
  */
  cout << "POSIX " << size << " " << num_th << " "
       << (double)r_time.count() * 1e-9 << "\n";

  ofstream out("posix.csv", ios_base::app);
  out << "POSIX" << size << ";" << num_th << ";"
      << (double)r_time.count() * 1e-9 << ";\n";
  out.close();
}