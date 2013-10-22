/*
 * Check whehter you have the complex polar (double, double) function.
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
  double d;
  d=real(polar(double(1.0),double(2.0)));
  return 0;
}
