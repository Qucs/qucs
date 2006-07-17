// compile with:  gcc fourier.c -o fourier -Wall -lm

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <qdatetime.h>

#define NUM 4096
typedef double nr_double_t;

// Helper macro.
#define Swap(a,b) { nr_double_t t; t = a; a = b; b = t; }

/* The function performs a 1-dimensional fast fourier transformation.
   Each data item is meant to be defined in equidistant steps.  The
   number of data items needs to be of binary size, e.g. 64, 128. */
void _fft_1d (nr_double_t * data, int len, int isign) {

  // bit reversal method
  int i, j, m, n;
  n = 2 * len;
  j = 0;
  for (i = 0; i < n; i += 2) {
    if (j > i) {                   // was index already swapped ?
      Swap (data[j], data[i]);     // swap real part
      Swap (data[j+1], data[i+1]); // swap imaginary part
    }
    m = len;
    while (m >= 2 && j >= m) {     // calculate next swap index
      j -= m;
      m >>= 1;
    }
    j += m;
  }

  // Danielson-Lanzcos algorithm
  int mmax, istep;
  nr_double_t wt, th, wr, wi, wpr, wpi, ti, tr;
  mmax = 2;
  while (n > mmax) {
    istep = mmax << 1;
    th = isign * (2 * M_PI / mmax);
    wt = sin (0.5 * th);
    wpr = -2.0 * wt * wt;
    wpi = sin (th);
    wr = 1.0;
    wi = 0.0;
    for (m = 1; m < mmax; m += 2) {
      for (i = m; i <= n; i += istep) {
	j = i + mmax;
	tr = wr * data[j] - wi * data[j-1];
	ti = wr * data[j-1] + wi * data[j];
	data[j] = data[i] - tr;
	data[j-1] = data[i-1] - ti;
	data[i] += tr;
	data[i-1] += ti;
      }
      wr = (wt = wr) * wpr - wi * wpi + wr;
      wi = wi * wpr + wt * wpi + wi;
    }
    mmax = istep;
  }
}


void _fft_1d_neu (nr_double_t * data, int len, int isign) {

  // bit reversal method
  int i, j, m, n;
  n = 2 * len;
  j = 0;
  for (i = 2; i < len; i += 2) {
    m = len;
    while (m >= 2 && j >= m) {     // calculate next swap index
      j -= m;
      m >>= 1;
    }
    j += m;
    if (j > i) {                   // was index already swapped ?
      Swap (data[j], data[i]);     // swap real part
      Swap (data[j+1], data[i+1]); // swap imaginary part
      if (j < len) {
        Swap (data[n-j-2], data[n-i-2]); // swap real part
        Swap (data[n-j-1], data[n-i-1]); // swap imaginary part
      }
    }
  }

  // Danielson-Lanzcos algorithm
  int mmax, istep;
  nr_double_t wt, th, wr, wi, wpr, wpi, ti, tr;
  mmax = 2;
  while (n > mmax) {
    istep = mmax << 1;
    th = isign * (2.0 * M_PI / mmax);
    wpr = cos (th);
    wpi = sin (th);
    wr = 1.0;
    wi = 0.0;

    for (i = 1; i <= n; i += istep) {
      j = i + mmax;
      tr = data[j];
      ti = data[j-1];
      data[j] = data[i] - tr;
      data[j-1] = data[i-1] - ti;
      data[i] += tr;
      data[i-1] += ti;
    }

    for (m = 3; m < mmax; m += 2) {
      wt = wr;
      wr = wt * wpr - wi * wpi;
      wi = wi * wpr + wt * wpi;
      for (i = m; i <= n; i += istep) {
	j = i + mmax;
	tr = wr * data[j] - wi * data[j-1];
	ti = wr * data[j-1] + wi * data[j];
	data[j] = data[i] - tr;
	data[j-1] = data[i-1] - ti;
	data[i] += tr;
	data[i-1] += ti;
      }
    }
    mmax = istep;
  }
}


int main()
{
  nr_double_t Buffer[NUM], Vector[NUM], Test[NUM];
  srand(0x12345678);

  int i, z;
  for(z=0; z<NUM; z++)
    Buffer[z] = (nr_double_t)rand();
  for(z=0; z<NUM; z++)
    Vector[z] = Buffer[z];
  for(z=0; z<NUM; z++)
    Test[z] = Buffer[z];

/*  for(z=0; z<10; z++)
    printf("%g ", Buffer[z]);
  printf("\n");
  for(z=0; z<10; z++)
    printf("%g ", Vector[z]);
  printf("\n");
  for(z=0; z<10; z++)
    printf("%g ", Test[z]);
  printf("\n");
  printf("\n");*/

  _fft_1d (Vector, NUM/2, 1);
  _fft_1d_neu (Test, NUM/2, 1);

/*  for(z=0; z<10; z++)
    printf("%g ", Buffer[z]);
  printf("\n");
  for(z=0; z<10; z++)
    printf("%g ", Vector[z]);
  printf("\n");
  for(z=0; z<10; z++)
    printf("%g ", Test[z]);
  printf("\n");
  printf("\n");*/

  for(z=0; z<NUM; z++)
    if(fabs((Test[z]-Vector[z])/Vector[z]) > 1e-9) {
      printf("%g\n", fabs((Test[z]-Vector[z])/Vector[z]));
      printf("Ungleich: %g-%g = %g\n", Test[z], Vector[z], Test[z] - Vector[z]);
      break;
    }

QTime t1;
/*t1.start();
  for(z=0; z<100; z++)
    _fft_1d (Vector, NUM, 1);
qDebug("Original: %d ms", t1.elapsed());
*/
t1.start();
  i = 1;
  for(z=0; z<200; z++) {
    _fft_1d_neu (Test, NUM, i);
    i *= -1;
  }
qDebug("Dauer: %d ms", t1.elapsed());

  return 0;
}
