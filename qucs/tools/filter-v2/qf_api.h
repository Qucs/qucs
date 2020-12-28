# ifndef  _QF_API_H
# define  _QF_API_H

// Identify each filter by a unique bit

enum {
  QF_BESL   = 1,
  QF_BLNCH  = 2 * QF_BESL,
  QF_BUTT   = 2 * QF_BLNCH,
  QF_CAUER  = 2 * QF_BUTT,
  QF_CHEB   = 2 * QF_CAUER,
  QF_ICHEB  = 2 * QF_CHEB,
  QF_LGNDR  = 2 * QF_ICHEB
};

// Id for transforms

enum {
  QF_NONETF   = 1,

  QF_LOWPASS  = 2 * QF_NONETF,
  QF_HIGHPASS = 2 * QF_LOWPASS,
  QF_BANDPASS = 2 * QF_HIGHPASS,
  QF_BANDSTOP = 2 * QF_BANDPASS,

  QF_ZIGZAG   = 2 * QF_BANDSTOP
};

# endif	  // QF_API_H
