# define  _QF_API

#include "qf_api.h"

#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"
#include "qf_tform.h"
#include "qf_bessel.h"
#include "qf_blinch.h"
#include "qf_butcheb.h"
#include "qf_cauer.h"
#include "qf_icheb.h"
#include "qf_legendre.h"
#include "qf_zigzag.h"

struct	qf_filter_api*	qf_filter_apis [] = {

	& qf_bessel_api,
	& qf_blinch_api,
	& qf_butt_api,
	& qf_cauer_api,
	& qf_cheb_api,
	& qf_icheb_api,
	& qf_lgndr_api,
	NULL
};

struct	qf_tform_api*	qf_tform_apis [] = {

	& qf_nonetf_api,
	& qf_lowpass_api,
	& qf_highpass_api,
	& qf_bandpass_api,
	& qf_bandstop_api,
	& qf_zigzag_api,
	NULL
};

# undef _QF_API
