# ifndef  _QF_CAUER
  # define  _QF_CAUER

# define	_QF_CAUER_DEBUG

const double	SN_ACC =  1e-5 ;  // Accuracy of sn (x) is SN_ACC²
const double	K_ERR1 =  1e-8 ; // Accuracy of K(k)
const double	K_ERR2 =  1e-20 ; // Accuracy of K(k)
const double	K_ERR3 =  1e-6 ; // Accuracy of K(k)

// Static functions 

static double	      K	      (double) ;	      // K(k)
static double	      Kp      (double) ;	      // K(1 - k)
static double	      sn      (double, double) ;      // sn (x, k)

static double	      FMAX    (double, double) ;      // max (x, y)
static double	      ASIND   (double) ;	      // Arcsin in °

static void  CC		      (void) ;		      // Calculate CC n Θ ρ

class
    qf_cauer : public qf_filter

{
  private :
		  // Standard paramaters

		  double    rho ;		// Reflection coeff.
		  double    th ;		// Modular angle

		  // Zeros of transmission

		  double      * a ;

  public :

		  qf_cauer    (unsigned, double, double) ;
		  qf_cauer    (double, double, double,
			       double, double, double, qft) ;
    virtual	  ~qf_cauer   (void) ;

						// Computes standard form
	    void  normalize   (double, double, double, qft) ;

	    void  xfer	      (void) ;		// Computes xfer fctn
            void  values      (void) ;		// Computes norm values
    virtual void  synth	      (qft) ;		// Standard -> Actual form
	    void  dump	      (void) ;		// Dumps to cout
} ;

# endif
