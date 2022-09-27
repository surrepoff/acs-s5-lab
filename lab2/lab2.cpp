#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void getNameCPU(char cpuname[]) {
  FILE *fp;
  if ((fp = fopen("/proc/cpuinfo", "r")) == NULL) {
    printf("Can't open /proc/cpuinfo \n");
    return;
  }
  size_t m = 0;
  char *line = NULL;
  while (getline(&line, &m, fp) > 0) {
    if (strstr(line, "model name")) {
      strcpy(cpuname, &line[13]);
      break;
    }
  }
  for (int i = 0; i < 60; i++)
    if (cpuname[i] == '\n') {
      cpuname[i] = '\0';
      break;
    }
  fclose(fp);
}

void matrixMul(int **matrixA, int **matrixB, int **matrixC, int matrixSize) {
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      matrixC[i][j] += 0;
      for (int k = 0; k < matrixSize; k++)
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
    }
}

void matrixMul(float **matrixA, float **matrixB, float **matrixC,
               int matrixSize) {
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      matrixC[i][j] += 0;
      for (int k = 0; k < matrixSize; k++)
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
    }
}

void matrixMul(double **matrixA, double **matrixB, double **matrixC,
               int matrixSize) {
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      matrixC[i][j] += 0;
      for (int k = 0; k < matrixSize; k++)
        matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
    }
}

int outToCSV(char *nameCPU, char *type, int launchCount, double averageTime,
             double *time, double dispersion, int matrixSize) {
  FILE *fp;
  if (!(fp = fopen("output.csv", "a"))) {
    printf("Error: can't open/find output.csv\n");
    return 1;
  }
  fprintf(fp, "%s;%s;%s;%s;%lld;%lld;%s;%e;%e;%e;%e;\n", nameCPU,
          "matrixMultiplication", type, "O0", (long long)launchCount,
          (long long)launchCount * matrixSize * matrixSize, "clock()",
          averageTime, sqrt(abs(dispersion)), abs(dispersion) / averageTime,
          (launchCount * matrixSize * matrixSize) / averageTime);
  fclose(fp);
  return 0;
}

int main(int argc, char *argv[]) {
  srand(time(0));

  int matrixSize = 200;
  int launchCount = 200;

  char cpuname[60];
  getNameCPU(cpuname);

  clock_t start, stop;

  printf("matrixSize = %d\n", matrixSize);
  printf("launchCount = %d\n", launchCount);

  double timeSum = 0;
  double dispersion = 0;
  double summand1 = 0;
  double summand2 = 0;
  double time[launchCount];

  //////////////////////////

  timeSum = 0;
  summand1 = 0;
  summand2 = 0;

  int **mA_i = new int *[matrixSize];
  int **mB_i = new int *[matrixSize];
  int **mR_i = new int *[matrixSize];

  for (int i = 0; i < matrixSize; i++) {
    mA_i[i] = new int[matrixSize];
    mB_i[i] = new int[matrixSize];
    mR_i[i] = new int[matrixSize];
    for (int j = 0; j < matrixSize; j++) {
      mA_i[i][j] = rand() / 10000;
      mB_i[i][j] = rand() / 10000;
      mR_i[i][j] = 0;
    }
  }

  for (int i = 0; i < launchCount; i++) {
    start = clock();
    matrixMul(mA_i, mB_i, mR_i, matrixSize);
    stop = clock();
    time[i] = ((double)(stop - start)) / CLOCKS_PER_SEC;
    timeSum += time[i];
    summand1 += time[i] * time[i];
    summand2 += time[i];
  }

  dispersion = summand1 / launchCount - summand2 / launchCount;

  outToCSV(cpuname, (char *)"int", launchCount, timeSum / launchCount, time,
           dispersion, matrixSize);

  for (int i = 0; i < matrixSize; i++) {
    delete (mA_i[i]);
    delete (mB_i[i]);
    delete (mR_i[i]);
  }
  delete[](mA_i);
  delete[](mB_i);
  delete[](mR_i);

  //////////////////////////

  timeSum = 0;
  summand1 = 0;
  summand2 = 0;

  float **mA_f = new float *[matrixSize];
  float **mB_f = new float *[matrixSize];
  float **mR_f = new float *[matrixSize];

  for (int i = 0; i < matrixSize; i++) {
    mA_f[i] = new float[matrixSize];
    mB_f[i] = new float[matrixSize];
    mR_f[i] = new float[matrixSize];
    for (int j = 0; j < matrixSize; j++) {
      mA_f[i][j] = rand() / 10000 + (float)rand() / 10000;
      mB_f[i][j] = rand() / 10000 + (float)rand() / 10000;
      mR_f[i][j] = 0;
    }
  }

  for (int i = 0; i < launchCount; i++) {
    start = clock();
    matrixMul(mA_f, mB_f, mR_f, matrixSize);
    stop = clock();
    time[i] = ((double)(stop - start)) / CLOCKS_PER_SEC;
    timeSum += time[i];
    summand1 += time[i] * time[i];
    summand2 += time[i];
  }

  dispersion = summand1 / launchCount - summand2 / launchCount;

  outToCSV(cpuname, (char *)"float", launchCount, timeSum / launchCount, time,
           dispersion, matrixSize);

  for (int i = 0; i < matrixSize; i++) {
    delete (mA_f[i]);
    delete (mB_f[i]);
    delete (mR_f[i]);
  }
  delete[](mA_f);
  delete[](mB_f);
  delete[](mR_f);

  //////////////////////////

  timeSum = 0;
  summand1 = 0;
  summand2 = 0;

  double **mA_d = new double *[matrixSize];
  double **mB_d = new double *[matrixSize];
  double **mR_d = new double *[matrixSize];

  for (int i = 0; i < matrixSize; i++) {
    mA_d[i] = new double[matrixSize];
    mB_d[i] = new double[matrixSize];
    mR_d[i] = new double[matrixSize];
    for (int j = 0; j < matrixSize; j++) {
      mA_d[i][j] = rand() / 10000 + (double)rand() / 10000;
      mB_d[i][j] = rand() / 10000 + (double)rand() / 10000;
      mR_d[i][j] = 0;
    }
  }

  for (int i = 0; i < launchCount; i++) {
    start = clock();
    matrixMul(mA_d, mB_d, mR_d, matrixSize);
    stop = clock();
    time[i] = ((double)(stop - start)) / CLOCKS_PER_SEC;
    timeSum += time[i];
    summand1 += time[i] * time[i];
    summand2 += time[i];
  }

  dispersion = summand1 / launchCount - summand2 / launchCount;

  outToCSV(cpuname, (char *)"double", launchCount, timeSum / launchCount, time,
           dispersion, matrixSize);

  for (int i = 0; i < matrixSize; i++) {
    delete (mA_d[i]);
    delete (mB_d[i]);
    delete (mR_d[i]);
  }
  delete[](mA_d);
  delete[](mB_d);
  delete[](mR_d);

  return 0;
}
