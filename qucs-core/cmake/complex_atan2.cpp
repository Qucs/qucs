/*
 * Check whether you have the complex atan2 function.
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
  complex<double> x(1,1),y(1,1);
  atan2(x,double(1.0));
  atan2(x, y); atan2(double(1.0), x);
  return 0;
}
