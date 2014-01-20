/*
 * Check whether you have the complex polar (complex, complex) function.
 */

#ifdef HAVE_TR1_COMPLEX
# include <tr1/complex>
  using namespace std;
  using namespace std::tr1;
#else
# include <complex>
# ifdef HAVE_NAMESPACES
  using namespace std;
#endif
#endif

int main()
{
  /* In order to get an error with g++ need to take real */
  complex<double> x,y;
  double d;
  d=real(polar(x,y));
  d=real(polar(x,2.0));
  d=real(polar(2.0,y));
  return 0;
}
