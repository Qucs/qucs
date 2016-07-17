#include <mat.h>
Mat::Mat(unsigned int r, unsigned int c)
{ 
  rows = r;
  cols = c;
  data = (r > 0 && c > 0) ? new complex<double>[r * c] : NULL;
}


void Mat::ones()
{
  for (unsigned int r = 0; r < rows; r++)
     for (unsigned int c = 0; c < cols; c++) data[r * cols + c] = 1;
}

void Mat::eye()
{
  for (unsigned int r = 0; r < rows; r++)
     for (unsigned int c = 0; c < cols; c++) (r==c) ? data[r * cols + c] = 1 : data[r * cols + c] = 0;
}

Mat operator * (complex<double> z, Mat a) {
  Mat A(a.rows, a.cols);
  for (unsigned int r = 0; r < a.rows; r++)
      for (unsigned int c = 0; c<a.cols; c++) A(r, c) = z*a(r, c);
  return A;
}

complex<double> ScalarProduct(vec U, vec V)
{
 if (V.size() != U.size()) return -1e30;
 complex<double> acc=0;
 for (unsigned int i = 0; i < U.size(); i++) acc += U[i]*V[i];
 return acc;
}


Mat operator * (Mat a, Mat b) {
//Check dimmensions
  if (a.cols != b.rows) return Mat(1,1);
  Mat Res(a.rows, b.cols);

  for (unsigned int r = 0; r < Res.rows; r++)
     for (unsigned int c = 0; c < Res.cols; c++) Res(r, c) = ScalarProduct(a.getRow(r), b.getCol(c));

  return Res;
}

vec Mat::getRow(unsigned int r)
{
   vec V(cols);
   for (unsigned int i = 0; i < cols; i++) V[i] = data[r * cols + i];
   return V;
}

vector<double> Mat::getRowReal(unsigned int r)
{
   vector<double> V(cols);
   for (unsigned int i = 0; i < cols; i++) V[i] = real(data[r * cols + i]);
   return V;
}
 
vector<double> Mat::getRowImag(unsigned int r)
{
   vector<double> V(cols);
   for (unsigned int i = 0; i < cols; i++) V[i] = imag(data[r * cols + i]);
   return V;
}

vec Mat::getCol(unsigned int c)
{
   vec V(rows);
   for (unsigned int i = 0; i < rows; i++) V[i] = data[i * cols + c];
   return V;
}

vector<double> Mat::getColReal(unsigned int c)
{
   vector<double> V(rows);
   for (unsigned int i = 0; i < rows; i++) V[i] = real(data[i * cols + c]);
   return V;
}

vector<double> Mat::getColImag(unsigned int c)
{
   vector<double> V(rows);
   for (unsigned int i = 0; i < rows; i++) V[i] = imag(data[i * cols + c]);
   return V;
}

void Mat::print ()
{
  for (unsigned int r = 0; r < rows; r++)
  {
     for (unsigned int c = 0; c < cols; c++)
     {
         cout << data[r * cols + c] << " ";
     }
     cout << endl;
  }
}

