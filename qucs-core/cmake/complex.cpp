/*
 * Check whether the compiler has complex<T>.
 */

#include <complex>
#ifdef HAVE_NAMESPACES
  using namespace std;
#endif
int main()
{
  complex<float> a;
  complex<double> b;
  abs(b);
  real(b);
  imag(b);
  arg(b);
  conj(b);
  return 0;
}
