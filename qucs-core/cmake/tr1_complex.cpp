/*
 * Check whether the compiler has TR1 compliant complex<T>.
 */

#include <tr1/complex>
using namespace std;
using namespace std::tr1;

int main()
{
  complex<float> a;
  complex<double> b;
  abs(b);
  real(b);
  imag(b);
  arg(b);
  return 0;
}
