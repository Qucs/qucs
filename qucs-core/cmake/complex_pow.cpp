/*
 * Check whether you have the complex pow function.
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
  complex<double> x,y;
#ifdef HAVE_TR1_COMPLEX
  std::tr1::pow(x,double(2.0));
  std::tr1::pow(x, y); std::tr1::pow(double(2.0), x);
#else
  std::pow(x,double(2.0));
  std::pow(x, y); std::pow(double(2.0), x);
#endif
  return 0;
}
