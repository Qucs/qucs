/*
 * Check whether you have the complex fmod function.
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
  fmod(x,double(2.0));
  fmod(x, y);
  fmod(double(2.0), x);
  return 0;
}
