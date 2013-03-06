
/* ------- code automatically created by ./mkelements.pl -------------- */


#include "adms.h"

#undef win32_interface
#if defined(WIN32)
#  if defined(insideadmstpathYacc)
#    define win32_interface __declspec(dllexport)
#  else
#    define win32_interface __declspec(dllimport)
#  endif
#else
#  define win32_interface extern
#endif

win32_interface p_slist globalfileoutputlist;
win32_interface int break_found;

win32_interface FILE *OUT(void);
win32_interface int is_admst (const char* name);
win32_interface int apath_main (int argc,char **argv);
win32_interface void dbxx (p_transform mytransform);
win32_interface void postxx (p_transform mytransform);
win32_interface void sanityxx (p_transform mytransform);
win32_interface p_transform lookfortemplates(p_transform mytransform,const char*match);
win32_interface p_path pparse (p_transform mytransform,const char* aname,const char* avalue);
win32_interface p_text tparse (p_ktransform mytransform,const char* aname,const char* avalue);
win32_interface p_admst adms_pull_admst (p_transform mytransform);
win32_interface p_admstvariable lookup_dollar (const char* myid);
win32_interface void tprintf (p_kadmst myadmst, p_ktext mytext);
win32_interface char* tsprintf (p_kadmst myadmst, p_ktext mytext);
win32_interface void free_ptraverse (p_ptraverse p);
win32_interface p_ptraverse bar (p_admst dot,p_path mypath0);
win32_interface void Xassert (p_transform mytransform,p_admst dot,p_admst dotdot);
win32_interface void xtraverse (p_slist mytransforms,p_admst dot,p_admst dotdot);
