#!perl -W

use warnings;
use XML::LibXML;

my $top_srcdir=shift @ARGV;
$top_srcdir=".." if not defined $top_srcdir;

sub checkargument {
  my$arg1=shift;
  if(not defined $arg1)
  {
    my($package1,$filename1,$line1,$subroutine1,$hasargs1,$wantarray1,$evaltext1,$is_require1,$hints1,$bitmask1)=caller(1);
    die "$subroutine1:$line1: failed";
  }
}

my$admsxml=XML::LibXML->new->parse_file("$top_srcdir/adms.xml")->getDocumentElement;
map{eval("sub x$_ {\&checkargument(\@_); shift->getAttribute(\"$_\");}");} qw(name path datatypefamily datatypename default info element attribute format);
my$admstpathxml=XML::LibXML->new->parse_file("$top_srcdir/admstpath.xml")->getDocumentElement;
map{eval("sub x$_ {\&checkargument(\@_); shift->getAttribute(\"$_\");}");} qw(id label value ret linkto eparent regexp);
my$admstxml=XML::LibXML->new->parse_file("$top_srcdir/admst.xml")->getDocumentElement;
map{eval("sub x$_ {\&checkargument(\@_); shift->getAttribute(\"$_\");}");} qw(haschild type);

my$BUILDER="\n/* ------- code automatically created by $0 -------------- */\n\n";
# build accessors for attribute
sub eparent
{
  my$element=shift;
  return $element->parentNode if($element->parentNode->nodeName eq "element");
}
sub xsize
{
  my$node=shift;
  return "list" if($node->nodeName eq "list");
  return "scalar";
}
sub uid  {my$e=shift;return if not $e;return $e->findnodes("uid/scalar|uid/list");}
sub attr {my$e=shift;return if not $e;return $e->findnodes("(uid/scalar)|(uid/list)|scalar|list");}
#svn
my$cygpath_top_srcdir=$top_srcdir;
if($cygpath_top_srcdir=`cygpath -ad $top_srcdir 2>/dev/null`)
{
  chomp $cygpath_top_srcdir;
  $cygpath_top_srcdir="\"$cygpath_top_srcdir\"";
}
my$GIT;
$GIT=`git log --pretty=format:'%h' -n 1` or $GIT="unknown";
print "git version: $GIT\n";
#parse adms.xml
my $EA;
my $EH;
foreach($admsxml->findnodes("//element"))
{
  my$e=$_;
  push @$EA,$_;
  $EH->{&xname($_)}=$_;
}
#c-type
sub ctype
{
  my$xdatatypename=&xdatatypename($_);
  if(0) {}
  elsif($xdatatypename eq "basicenumeration") {return "admse"}
  elsif($xdatatypename eq "basicinteger")     {return "int"}
  elsif($xdatatypename eq "basicreal")        {return "double"}
  elsif($xdatatypename eq "basicstring")      {return "const char*"}
  elsif($xdatatypename eq "basickstring")      {return "const char*"}
  elsif($xdatatypename eq "basicfunction")    {return "void*"}
  elsif($xdatatypename eq "basiclist")    {return "p_slist"}
  else{return "p_$xdatatypename"}
}
sub ccontainer
{
  my$attribute=shift;
  return (&xsize($attribute) eq "list")?"p_slist":ctype($attribute);
}
sub hdece
{
  return &uid($_)?(join",",map{&ccontainer($_)." my".&xname($_)}&uid($_)):"void";
}
sub hdec
{
  return &uid($EH->{xdatatypename($_)})?(join",",map{&ctype($_)." my".&xname($_)}&uid($EH->{xdatatypename($_)})):"void";
}

my$adms_h;
$adms_h.=$BUILDER;
$adms_h.="#ifndef adms_h\n";
$adms_h.="#define adms_h\n";
$adms_h.="\n";
$adms_h.="#define GIT \"$GIT\" /* git release version */\n";
$adms_h.=q@
/*headers -- depend on compiler, OS, ...*/
#  if defined(_MSC_VER)
#    define PACKAGE_NAME "adms"
#    if ! defined(PACKAGE_VERSION)
#      pragma message(__FILE__ ": warning: #warning undefined macro 'PACKAGE_VERSION' - defaulted to 'unknown'")
#      define PACKAGE_VERSION "unknown"
#    endif
#    define PACKAGE_STRING PACKAGE_NAME " " PACKAGE_VERSION
#    define PACKAGE_TARNAME PACKAGE_NAME
#    define PACKAGE_BUGREPORT "r29173\@users.sourceforge.net"
#    include <direct.h>
#    define isnan _isnan
#    define ADMS_PATH_SEPARATOR "\\\\"
#    define ADMS_F_OK 0
#    ifndef inline
#      define inline
#    endif
#    define HAVE_FLOAT_H 1
#    define HAVE_STDLIB_H 1
#    define HAVE_PUTENV 1
#    define HAVE_LOCALE 1
#    define HAVE_STRING_H 1
#    define HAVE_SYS_STAT_H 1
#    include <io.h>
#  else
#    include <config.h>
#    define ADMS_PATH_SEPARATOR "/"
#    include <unistd.h>
#    define ADMS_F_OK F_OK
#  endif
#  include <math.h>
#  if defined(HAVE_SYS_STAT_H)
#    include <sys/stat.h>
#  endif
#  if defined(HAVE_LOCALE_H)
#    include <locale.h>
#  endif
#  if defined(HAVE_FLOAT_H)
#    include <float.h>
#  endif
#  if defined(HAVE_UNISTD_H)
#    include <unistd.h>
#  endif
#  if defined(HAVE_STDLIB_H)
#    include <stdlib.h>
#  endif
#  if defined(HAVE_STRING_H)
#    include <string.h>
#  endif
#  include <assert.h>
#  include <signal.h>
#  include <stdarg.h>
#  include <stdio.h>
#  include <ctype.h>
#  include <errno.h>
#  include <time.h>
/* in case not Posix */
#  if defined(_S_IFDIR)
#    define ADMS_S_IFDIR _S_IFDIR
#  else
#    define ADMS_S_IFDIR S_IFDIR
#  endif
/* check OS */
#  if defined(__CYGWIN__)
#    define ADMS_OS_MS
#    define ADMS_OS_MSCYGWIN
#    define ADMS_OS "MSCYGWIN"
#  elif defined(__MSDOS__)
#    define ADMS_OS_MS
#    define ADMS_OS_MSDOS
#    define ADMS_OS "MSDOS"
#  elif defined(_WIN64)
#    define ADMS_OS_MS
#    define ADMS_OS_MSWIN64
#    define ADMS_OS "MSWIN64"
#  elif defined(_WIN32)
#    define ADMS_OS_MS
#    define ADMS_OS_MSWIN32
#    define ADMS_OS "MSWIN32"
#  elif defined(__APPLE__)
#    define ADMS_OS_DARWIN
#    define ADMS_OS "DARWIN"
#  elif defined(__linux)
#    define ADMS_OS_LINUX
#    define ADMS_OS "LINUX"
#  else
#    define ADMS_OS_UNKNOWN
#    define ADMS_OS "UNKNOWN"
#  endif
/* check compiler */
#  if defined(__MINGW32__)
#    ifndef WIN32
#      define WIN32
#    endif
#    define ADMS_COMPILER_MINGW32
#    define ADMS_COMPILER "MINGW32"
#  elif defined(__CYGWIN__)
#    ifndef WIN32
#      define WIN32
#    endif
#    define ADMS_COMPILER_GCC
#    define ADMS_COMPILER "GCC"
#  elif defined(_MSC_VER)
#    ifndef WIN32
#      define WIN32
#    endif
#    define ADMS_COMPILER_MSVC
#    define ADMS_COMPILER "MSVC"
#  elif defined(__APPLE__)
#    if defined(__clang__)
#      define ADMS_COMPILER_CLANG
#      define ADMS_COMPILER "CLANG"
#    elif defined(__GNUC__)
#      define ADMS_COMPILER_GCC
#      define ADMS_COMPILER "GCC"
#    endif
#  elif defined(__linux)
#    if defined(__GNUC__)
#      define ADMS_COMPILER_GCC
#      define ADMS_COMPILER "GCC"
#    endif
#  else
#    define ADMS_COMPILER_CC
#    define ADMS_COMPILER "CC"
#  endif
@;
#c macros
$adms_h.="#define E\\\n";
map{$adms_h.="  EE(".xname($_).")\\\n";} @$EA;
$adms_h.="/*fin*/\n\n";
$adms_h.="#define A\\\n";
map{my$e=xname($_);map{$adms_h.="  AA(".xname($_).")\\\n";} &attr($_);} @$EA;
$adms_h.="/*fin*/\n\n";
my%ALIAS;
foreach(@$EA)
{
  my$e=&xname($_);
  foreach($_->findnodes("alias"))
  {
    push @{$ALIAS{&xname($_)}},[$e,&xelement($_)];
  }
}
my%ALIASBIS;
foreach(@$EA)
{
  my$e=&xname($_);
  foreach($_->findnodes("aliasbis"))
  {
    push @{$ALIASBIS{&xname($_)}},[$e,&xpath($_)];
  }
}

#va elements
my@VA;
map{push @VA,xname($_) if vla(xname($_));} @$EA;

#math elements
my@MATHELEMENTS;
foreach($admsxml->findnodes("//element[\@name='math']//element"))
{
   push @MATHELEMENTS,[xname($_),"math"];
}

#elements + attributes
$adms_h.="#define EA\\\n";
map{my$e=xname($_);map{$adms_h.="  EEA(".$e.",".xname($_).")\\\n";} &attr($_);} @$EA;
$adms_h.="/*fin*/\n\n";

$adms_h.="#define TK\\\n";
#add tokens for pparse
#look for //[@name=pparse]/scalar/attribute[@name=tkid]
my@token;
#element
map{push @token,xname($_);} @$EA;
#attribute
map{map{push @token,xname($_);} $_->findnodes("uid/scalar|uid/list|scalar|list");} @$EA;
#enum
map{map{push @token,xname($_);} $_->findnodes("uid/scalar/evalue|scalar/evalue");} @$EA;
push @token,qw(past_name datatypefamily absolute basicenumeration basicinteger basicreal basicstring basiclist empty);
my@tokenname;
my%seentokenname;
map{$adms_h.="  TTK($_)\\\n" if not defined $seentokenname{$_};$seentokenname{$_}=1;} @token;
$adms_h.="/*fin*/\n\n";

#lexer
my$tk;
map{$tk.="  tktk_".xid($_).",\n";} $admstpathxml->findnodes("//constant");
map{map{$tk.="  tktk_".$_.",\n";}split " ", xid($_);} $admstpathxml->findnodes("//regexp");
$adms_h.=qq@
#undef TTK
#define TTK(tk) admse_##tk,
typedef enum {
  admse__zero=0,
  admse__any,
  admse__constant,
  admse__p, /*%p*/
  admse__s, /*%s*/
  admse__path,
  admse__arguments,
  admse__croix,
  admse__croixcroix,
  admse__ladms,
  admse__ladmst,
  TK
  admse__
} admse;
@;
$adms_h.="\n";
foreach(@$EA)
{
  my$ename=xname($_);
  $adms_h.="typedef struct s_$ename  t_$ename;\n";
  $adms_h.="typedef t_$ename*        p_$ename;\n";
  $adms_h.="typedef const p_$ename   p_k$ename;\n";
}

#miscH
$adms_h.="\n/*miscH*/\n";
$adms_h.="typedef struct s_slist t_slist;\n";
$adms_h.="typedef t_slist* p_slist;\n";
$adms_h.="FILE *stdadmstdbgimpl;\n";
$adms_h.="FILE *stdadmstdbgimpl;\n";
$adms_h.="#define stdadmstdbg (stdadmstdbgimpl?stdadmstdbgimpl:stderr)\n";
$adms_h.="int admsmain(const char*xcode,const char*xflag,const char*vacode);\n";
$adms_h.="int adms_global_nbadmstnew(void);\n";
$adms_h.="int adms_global_nbadmstdestroy(void);\n";
$adms_h.="void adms_global_increment_nbadmstnew(void);\n";
$adms_h.="void adms_global_increment_nbadmstdestroy(void);\n";
$adms_h.="/*-- admst/new prototypes --*/\n";
foreach (qw[bd be bi br bs ns ks pa pn pc pd la ln pp])
{
my$cast=$_ eq"bd"?"const admse":
        $_ eq"be"?"const admse":
        $_ eq"bi"?"const int":
        $_ eq"br"?"const double":
        $_ eq"bs"?"char*":
        $_ eq"ns"?"char*":
        $_ eq"ks"?"char*":
        $_ eq"pd"?"p_admstvariable":
        $_ eq"pn"?"p_admst":
        $_ eq"pc"?"p_attribute":
        $_ eq"la"?"p_slist":
        $_ eq"ln"?"p_slist":
        $_ eq"pp"?"admse":
        "const p_adms";
my$inputs0=$_ eq"ks"?"$cast item":
         "p_admst myprevious,p_admst mypreviouspath,$cast item";
$adms_h.="p_admst adms_admst_new$_ ($inputs0);\n";
}
$adms_h.="\n/*-- Miscellaneous routines --*/\n";
$adms_h.=q@
const double adms_dzero;
double adms_NAN;
p_admst aread (p_admst myadmst);
void deref(p_admst mystack);
union admsu
{
  int i;
  double r;
  char* s;
  p_adms p;
};
const int minusone;

int adms_strtol (p_ktransform mytransform,const char* mynumber);
double adms_strtod (p_ktransform mytransform,const char* myreal);
void bp(void);
char*adms_integertostring(int value);
char*adms_doubletostring(double value);
int adms_setenv(const char* myname,const char* myvalue);
int adms_file_isregular(const char* myfilename);
int adms_file_isdirectory(const char* myfilename);
p_slist adms_slist_new (p_adms d);
p_slist adms_slist_copy (p_slist l);
void adms_slist_concat (p_slist* l1,p_slist l2);
p_slist adms_slist_find (p_slist l,p_kadms data);
void adms_slist_free (p_slist l);
int adms_slist_index (p_slist l, p_kadms data);
p_slist adms_slist_last (p_slist l);
unsigned int adms_slist_length (p_slist l);
p_slist adms_slist_nth (p_slist l,unsigned int  n);
p_adms adms_slist_nth_data (p_slist l,unsigned int n);
void adms_slist_print(const char* message,p_slist l);
p_adms adms_slist_pull(p_slist* l);
void adms_slist_push(p_slist* l,p_adms data);
p_slist adms_slist_reverse (p_slist l);
void adms_slist_inreverse (p_slist* l);
struct s_slist
{
  p_adms data;
  p_slist next;
};
@;
$adms_h.=q[

p_admsmain root(void);
void rootnew(p_admsmain myglobaladmsmain);

typedef p_admst (t_new) (p_ktransform mytransform,p_admst dot,p_slist arguments[],const int size);
typedef p_admst (*p_new) (p_ktransform mytransform,p_admst dot,p_slist arguments[],const int size);
#define _t_new(function) p_admst (function) (p_ktransform mytransform,p_admst dot,p_slist arguments[],const int size)

typedef int (t_cmp) (p_adms myadms, p_adms myadmsref);
typedef int (*p_cmp) (p_adms myadms, p_adms myadmsref);
#define _t_cmp(function) int (function) (p_adms myadms, p_adms myadmsref)

/*-- Messaging --*/
typedef void (t_message) (const char*format, ...);
typedef void (*p_message) (const char*format, ...);
#define _t_message(function)  void (function) (const char*format, ...)

void adms_toupper(char*m);
void adms_tolower(char*m);
char*adms_kclone(const char* m);
char*adms_knclone(const char* m,const int l);
char*adms_m2nclone(const char* m,const char* n);
void adms_strconcat(char **s1,char *s2);
void adms_k2strconcat(char **s1,const char* s2);
FILE *adms_file_open_read (const char* fileName);
];
my@message=(
  ["info","stdout",0],
  ["usage","stdout",0],
  ["verbose","stdout",0],
  ["debug","stdout",0],
  ["dbg_vla","stdout",0],
  ["dbg_xml","stdout",0],
  ["dbg_mem","stdout",0],
  ["hint","stderr",0],
  ["warning","stderr",0],
  ["obsolete","stderr",0],
  ["error","stderr",2],
  ["fatal","stderr",1],
  ["admstdbg","stdadmstdbg",-1]
);
foreach my$mess(@message)
{
  my$name=$mess->[0];
  $adms_h.="t_message adms_message_${name}_impl;\n";
  $adms_h.="t_message adms_message_${name}_continue_impl;\n";
  $adms_h.="#define adms_message_$name(VAARGS)\\\n";
  $adms_h.="{\\\n";
  $adms_h.="  if((root())\\\n";
  $adms_h.="    &&(root()->_$name==admse_yes))\\\n";
  $adms_h.="    adms_message_${name}_impl VAARGS;\\\n";
  $adms_h.="}\n";
  $adms_h.="#define adms_message_${name}_continue(VAARGS)\\\n";
  $adms_h.="{\\\n";
  $adms_h.="  if((root())\\\n";
  $adms_h.="    &&(root()->_$name==admse_yes))\\\n";
  $adms_h.="    adms_message_${name}_continue_impl VAARGS;\\\n";
  $adms_h.="}\n";
}
$adms_h.="\n";
$adms_h.="char* aprintf (p_ktransform mytransform,p_kadmst myadmst);\n";
sub vla {my$e=shift; return !(($e eq "return")||($e eq "adms")||($e eq "yaccval")||($e eq "admst")||($e eq "transform")
||($e eq "itransform")||($e eq "admstvariable")||($e eq "pparse")||($e eq "ptraverse")||($e eq "text")||($e eq "path"));}
foreach(@$EA)
{
  my$element=$_;
  my$ename=$element->getAttribute("name");
  $adms_h.="\n/* ------- $ename -------------- */\n";
  my$parent=&eparent($element);
  $adms_h.="#define adms_$ename(member) ((p_$ename)(member))\n";
  #mkstruct
  $adms_h.="struct s_$ename {\n";
  if($parent)
  {
    my$parentName=$parent->getAttribute ("name");
    $adms_h.="  t_$parentName _$parentName;\n";
  }
  foreach(&attr($element))
  {
    my$xname=&xname($_);
    if(($ename eq "admst")&&($xname eq "item"))
    {
      $adms_h.="  union admsu _item;\n";
    }
    elsif(($xname eq "datatypename")||($xname eq "pseudo"))
    {
      $adms_h.="  admse _${xname};\n";
    }
    else
    {
      my$ctype=ccontainer($_);
      $ctype=~s/const // if(&xdatatypename($_) eq "basicstring");
      $adms_h.="  $ctype _$xname;\n";
      $adms_h.="  int _${xname}_isdefault : 2;\n" if vla($ename);
    }
  }
  $adms_h.="};\n";
  #uid
  $adms_h.="char*adms_${ename}_uid (p_$ename my$ename);\n";
  if($ename ne "adms")
  {
    $adms_h.="p_$ename adms_${ename}_new (".&hdece($_).");\n";
    $adms_h.="void adms_${ename}_free(p_$ename my$ename);\n";
  }
  foreach(&attr($element))
  {
    #valueto
    my$xname=&xname($_);
    my$xdatatypename=&xdatatypename($_);
    $adms_h.="void adms_${ename}_valueto_$xname (p_$ename my$ename,".ccontainer($_)." _${xname});\n";
    next if xsize($_) eq "scalar";
    #prepend
    if(($ename eq "module")&&($xname =~ "branch|node|probe|source"))
    {
      $adms_h.="".ctype($_)." adms_${ename}_list_${xname}_prepend_by_id_once_or_ignore (p_$ename mymy${ename},".&hdec($_).");\n";
    }
    if(
      (($ename =~ "admsmain")&&($xname =~ "module|nature"))
      ||
      (($ename =~ "module")&&($xname =~ "branchalias|instance|node|nodealias|range"))
    )
    {
      $adms_h.="".ctype($_)." adms_${ename}_list_${xname}_prepend_by_id_once_or_abort (p_$ename mymy${ename},".&hdec($_).");\n";
    }
    if(
      (($ename =~ "admsmain")&&($xname =~ "discipline"))
      ||
      (($ename =~ "analogfunction|block|module")&&($xname =~ "variable"))
      ||
      (($ename =~ "instance")&&($xname =~ "parameterset|terminal"))
      ||
      (($ename =~ "module")&&($xname =~ "analogfunction"))
      ||
      (($ename =~ "variable")&&($xname =~ "alias"))
    )
    {
      $adms_h.="void adms_${ename}_list_${xname}_prepend_once_or_abort (p_$ename mymy${ename},".ctype($_)." my${xname});\n";
    }
    next if $xdatatypename eq "adms";
    my@cmp;
    foreach(&uid($EH->{$xdatatypename}))
    {
      my$cur="ref$xdatatypename->_".&xname($_);
      my$ref="my".&xname($_);
      if(xdatatypename($_) eq "basicstring")
      {
        push @cmp,"!strcmp($cur,$ref)";
      }
      elsif(&xdatatypename($_) eq "lexval")
      {
        push @cmp,"!strcmp($cur->_string,$ref->_string)";
      }
      elsif(&xdatatypename($_) eq "quark")
      {
        push @cmp,"!strcmp($cur->_value,$ref->_value)";
      }
      else
      {
        push @cmp,"($cur==$ref)";
      }
    }
    #lookup
    if(
      (($ename =~ "admsmain")&&($xname =~ "discipline"))
      ||
      (($ename =~ "admsmain")&&($xname =~ "module"))
      ||
      (($ename =~ "admsmain")&&($xname =~ "nature"))
      ||
      (($ename =~ "analogfunction")&&($xname =~ "variable"))
      ||
      (($ename =~ "module")&&($xname =~ "branchalias"))
      ||
      (($ename =~ "module")&&($xname =~ "node"))
      ||
      (($ename =~ "module")&&($xname =~ "variable"))
    )
    {
      $adms_h.="".ctype($_)." adms_${ename}_list_${xname}_lookup_by_id (p_$ename mymy$ename,".&hdec($_).");\n";
    }
  }
}
$adms_h.="
admse ns_strtoe (const char* tk);
char* ns_etostr (admse e);
p_new adms_admsxml_new (p_ktransform mytransform,const char* ename);
p_cmp adms_admsxml_cmp (p_kadmst myadmst);
#endif /* adms_h */\n";

open FILE,">$top_srcdir/admsXml/adms.h"; print FILE $adms_h; close FILE;
print "$top_srcdir/admsXml/adms.h: created\n";

sub pnew
{
  my$leaf=shift;
  my$leafpath=shift;
  my$element=shift;
  my$parent=shift;
  my$pname=$parent->getAttribute("name");
  my$pelement=$EH->{$pname};
  my$supersuperElement=&eparent($pelement);
  my$code;
  if($supersuperElement)
  {
    $code.=&pnew($leaf,"${leafpath}_$pname.",$pelement,$supersuperElement);
  }
  foreach(&attr($pelement))
  {
    my$xname=&xname($_);
    my$xdatatypename=&xdatatypename($_);
    my$xdefault=&xdefault($_);
    next if not $xdefault;
    if($xdefault=~/^@/)
    {
      $xdefault=~s/@//;
      $code="  ${leafpath}_$pname._$xname=admse_".$leaf->getAttribute($xdefault).";\n";
    }
    elsif($xdatatypename eq "basicreal")
    {
      $code.="  ${leafpath}_$pname._$xname=$xdefault;\n";
    }
    elsif($xdatatypename eq "basicenumeration")
    {
      $code.="  ${leafpath}_$pname._$xname=admse_$xdefault;\n";
    }
    else {die;}
  }
  return $code;
}

sub mkfree ()
{
  my$element=$_;
  my$ename=$element->getAttribute("name");
  my$code;
  $code.="void adms_${ename}_free (p_$ename my$ename)\n";
  $code.="{\n";
  $code.="  if(!my$ename)\n";
  $code.="    return;\n";
  my$parent;
  foreach(&attr($element))
  {
    if(xsize($_) eq "list" && $ename ne "admst")
    {
      $code.="  adms_slist_free(my$ename->_".&xname($_).");\n";
    }
    elsif(xdatatypename($_) eq "basicstring")
    {
      $code.="  free(my$ename->_".xname($_).");\n";
    }
  }
  $code.="  adms_global_increment_nbadmstdestroy();\n" if $ename eq "admst";
  $code.="  free(my$ename);\n";
  $code.="}\n";
  return $code;
}

sub mkuid ()
{
  my$element=$_;
  my$ename=$element->getAttribute("name");
  my$code;
  my$format=$element->findnodes("uid")->get_node(0)->getAttribute ("format");
  $code.="char*adms_${ename}_uid (p_$ename mynew$ename)\n";
  $code.="{\n";
  $code.="  char*myuid=NULL;\n";
  my@Attr=&attr($element);
  $code.="  adms_k2strconcat(&myuid,\"\");\n" if not $format;
  while($format=~s/^([^\$]*)\$(\d)//)
  {
    $code.="  adms_k2strconcat(&myuid,\"$1\");\n" if $1;
    my$attr=$Attr[$2-1];
    die "$ename: $format" if not defined $attr;
    my$xname=&xname($attr);
    my$xfamily=&xdatatypefamily($attr);
    my$xdatatypename=&xdatatypename($attr);
    my$xsize=&xsize($attr);
    my$xformat=&xformat($attr);
    if($xsize eq "list")
    {
      my$subformat=$xformat;
      $code.="  {\n";
      $code.="    p_slist list=mynew$ename->_$xname;\n";
      $code.="    while(list)\n";
      $code.="    {\n";
      $code.="      adms_strconcat(&myuid,adms_admsxml_uid(list->data));\n";
      $code.="      if(list->next) adms_k2strconcat(&myuid,\"$subformat\");\n";
      $code.="      list=list->next;\n";
      $code.="    }\n";
      $code.="  }\n";
    }
    elsif(defined($xformat))
    {
      $code.="  if(mynew$ename->_$xname)\n";
      $code.="  {\n";
      while($xformat=~s/([^\$]*)\$(\d+)//)
      {
        my$v=&xname($Attr[$2-1]);
        $code.="    adms_k2strconcat(&myuid,\"$1\");\n";
        $code.="    adms_strconcat(&myuid,adms_admsxml_uid(mynew$ename->_$v));\n";
      }
      $code.="    adms_k2strconcat(&myuid,\"$xformat\");\n" if $xformat;
      $code.="  }\n";
    }
    elsif($xdatatypename eq "basicenumeration")
    {
      foreach(@{$attr->findnodes("evalue")})
      {
        my$enode=$_;
        $code.="  if(mynew$ename->_$xname==admse_".&xname($enode).")\n";
        $code.="  {\n";
        my$enuminfo=&xinfo($enode);
        while($enuminfo=~s/([^\$]*)\$(\d+)//)
        {
          my$v=&xname($Attr[$2-1]);
          $code.="    adms_k2strconcat(&myuid,\"$1\");\n";
          $code.="    adms_strconcat(&myuid,adms_admsxml_uid(mynew$ename->_$v));\n";
        }
        $code.="    adms_k2strconcat(&myuid,\"$enuminfo\");\n" if $enuminfo;
        $code.="  }\n";
      }
    }
    elsif(($xdatatypename eq "basicinteger")&&($xname eq "pseudo"))
    {
      $code.="  adms_strconcat(&myuid,ns_etostr(mynew$ename->_$xname));\n";
    }
    elsif($xdatatypename eq "basicinteger")
    {
      $code.="  adms_strconcat(&myuid,adms_integertostring(mynew$ename->_$xname));\n";
    }
    elsif($xdatatypename eq "basicreal")
    {
      $code.="  adms_strconcat(&myuid,adms_doubletostring(mynew$ename->_$xname));\n";
    }
    elsif($xdatatypename eq "basicstring")
    {
      $code.="  adms_k2strconcat(&myuid,mynew$ename->_$xname);\n";
    }
    elsif($xdatatypename eq "adms")
    {
      $code.="  adms_strconcat(&myuid,adms_admsxml_uid(mynew$ename->_$xname));\n";
    }
    elsif($xfamily eq "reference")
    {
      $code.="  adms_strconcat(&myuid,adms_${xdatatypename}_uid(mynew$ename->_$xname));\n";
    }
  }
  $code.="  adms_k2strconcat(&myuid,\"$format\");\n" if $format;
  $code.="  return myuid;\n";
  $code.="}\n";
  return $code;
}

my$adms_c;
$adms_c.=$BUILDER;
$adms_c.="#include \"adms.h\"\n";
$adms_c.="const double adms_dzero=0.0;\n";
$adms_c.="double adms_NAN;\n";
#enumeration to string
$adms_c.="FILE *stdadmstdbgimpl=NULL;\n";
$adms_c.="#undef TTK\n";
$adms_c.="#define TTK(tk) if(e==admse_##tk) return #tk;\n";
$adms_c.="char* ns_etostr (admse e)\n";
$adms_c.="{\n";
$adms_c.="  TK\n";
$adms_c.="  if(e==admse__zero) return \"_zero\";\n";
$adms_c.="  if(e==admse__any) return \"_any\";\n";
$adms_c.="  if(e==admse__constant) return \"_constant\";\n";
$adms_c.="  if(e==admse__p) return \"_p\";\n";
$adms_c.="  if(e==admse__s) return \"_s\";\n";
$adms_c.="  if(e==admse__path) return \"_path\";\n";
$adms_c.="  if(e==admse__arguments) return \"_arguments\";\n";
$adms_c.="  if(e==admse__croix) return \"_croix\";\n";
$adms_c.="  if(e==admse__croixcroix) return \"_croixcroix\";\n";
$adms_c.="  if(e==admse__ladms) return \"_ladms\";\n";
$adms_c.="  if(e==admse__ladmst) return \"_ladmst\";\n";
$adms_c.="  return NULL;\n";
$adms_c.="}\n";
#string to enumeration
$adms_c.="#undef TTK\n";
$adms_c.="#define TTK(tk) if(!strcmp(mytk,#tk)) return admse_##tk;\n";
$adms_c.="admse ns_strtoe (const char* mytk)\n";
$adms_c.="{\n";
$adms_c.="  TK\n";
$adms_c.="  return admse__any;\n";
$adms_c.="}\n";
$adms_c.="static char* adms_admsxml_uid (p_kadms myitem)\n";
$adms_c.="{\n";
$adms_c.="  if(!myitem)\n";
$adms_c.="    return adms_kclone(\"\");\n";
foreach(@$EA)
{
  my$ename=xname($_);
  next if $ename eq "adms";
  $adms_c.="  if(myitem->_datatypename==admse_$ename)\n";
  $adms_c.="    return adms_${ename}_uid((p_$ename)myitem);\n";
}
$adms_c.="  adms_message_fatal_continue((\"%s:%i: internal error - should not be reached\\n\",__FILE__,__LINE__))\n";
$adms_c.="  adms_message_fatal((\"%s:%i: please report to r29173\@users.sourceforge.net\\n\",__FILE__,__LINE__))\n";
$adms_c.="  return NULL;\n";
$adms_c.="}\n";
$adms_c.="char* aprintf (p_ktransform mytransform,p_kadmst myadmst)\n";
$adms_c.="{\n";
$adms_c.="  if(myadmst->_pseudo==admse__ladms)\n";
$adms_c.="  {\n";
$adms_c.="    adms_message_fatal_continue((\"list not supported\\n\"))\n";
$adms_c.="    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mytransform)))\n";
$adms_c.="  }\n";
$adms_c.="  if(myadmst->_pseudo==admse_empty)\n";
$adms_c.="    return adms_kclone(\"\");\n";
$adms_c.="  if(myadmst->_pseudo==admse_datatypename)\n";
$adms_c.="    return adms_kclone(ns_etostr((admse)myadmst->_item.i));\n";
$adms_c.="  if(myadmst->_pseudo==admse_basicenumeration)\n";
$adms_c.="    return adms_kclone(ns_etostr((admse)myadmst->_item.i));\n";
$adms_c.="  if(myadmst->_pseudo==admse_basicinteger)\n";
$adms_c.="    return adms_integertostring(myadmst->_item.i);\n";
$adms_c.="  if(myadmst->_pseudo==admse_basicreal)\n";
$adms_c.="    return adms_doubletostring(myadmst->_item.r);\n";
$adms_c.="  if(myadmst->_pseudo==admse_basicstring)\n";
$adms_c.="    return adms_kclone((char*)myadmst->_item.s);\n";
$adms_c.="  if(myadmst->_pseudo==admse__croix)\n";
$adms_c.="  {\n";
$adms_c.="    if(((p_attribute)myadmst->_item.p)->_value)\n";
$adms_c.="      return aprintf(mytransform,(p_admst)((p_attribute)myadmst->_item.p)->_value);\n";
$adms_c.="    else\n";
$adms_c.="      return adms_kclone(\"\");\n";
$adms_c.="  }\n";
foreach(@$EA)
{
  my$ename=xname($_);
  next if $ename eq "adms";
  $adms_c.="  if(myadmst->_pseudo==admse_$ename)\n";
  $adms_c.="    return adms_${ename}_uid((p_$ename)myadmst->_item.p);\n";
}
$adms_c.="  adms_message_fatal_continue((\"%s:%i: internal error - should not be reached\\n\",__FILE__,__LINE__))\n";
$adms_c.="  adms_message_fatal_continue((\"%s:%i: please report to r29173\@users.sourceforge.net\\n\",__FILE__,__LINE__))\n";
$adms_c.="  adms_message_fatal((\"see %s\\n\",adms_transform_uid(mytransform)))\n";
$adms_c.="  return NULL;\n";
$adms_c.="}\n";

$adms_c.=q@
const int minusone=-1;
void deref (p_admst myadmst)
{
  myadmst->_refd--;
  if(myadmst->_refd)
    return;
  if(myadmst->_original)
  {
    if(myadmst->_hasnewbasicstring==1)
    {
      myadmst->_original->_refd--;
      if(myadmst->_original->_refd==0)
      {
        free(myadmst->_original->_item.s);
        adms_admst_free(myadmst->_original);
      }
    }
    else if(myadmst->_hasnewbasicstring==2)
    {
      myadmst->_original->_refd--;
      if(myadmst->_original->_refd==0)
      {
        adms_return_free((p_return)myadmst->_original->_item.p);
        adms_admst_free(myadmst->_original);
      }
    else
      adms_message_fatal(("internal error"))
    }
  }
  if(myadmst->_hasnewbasicstring==1)
    free(myadmst->_item.s);
  else if(myadmst->_hasnewbasicstring==2)
    adms_return_free((p_return)myadmst->_item.p);
  adms_admst_free(myadmst);
}
#define p_basicstring char*
#define adms_basicstring_uid(e) ((char*)e)
static p_admst adms_basicenumeration_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicenumeration) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicenumeration) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbi(dot,dot,adms_strtol(mytransform,s));
    free(s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static p_admst adms_basicinteger_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicinteger) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicinteger) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbi(dot,dot,adms_strtol(mytransform,s));
    free(s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static p_admst adms_basicreal_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicreal) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicreal) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbr(dot,dot,adms_strtod(mytransform,s));
    free(s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static p_admst adms_basicstring_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)
{
  if(size!=1)
  {
    adms_message_fatal_continue(("admst:new(basicstring) bad number of inputs (found %i, expected 1)\n",size))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else if(adms_slist_length(myinputs[0])!=1)
  {
    adms_message_fatal_continue(("admst:new(basicstring) bad number of element in inputs[0] (found %i, expected 1)\n",adms_slist_length(myinputs[0])))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
    return NULL;
  }
  else
  {
    char*s=aprintf(mytransform,(p_admst)(myinputs[0]->data));
    /*xnew*/p_admst myadmst=adms_admst_newbs(dot,dot,s);
    deref((p_admst)myinputs[0]->data);
    adms_slist_free(myinputs[0]);
    return myadmst;
  }
}
static int adms_basicenumeration_cmp (const char* mybasicenumeration,const char* refbasicenumeration)
{
  return (mybasicenumeration!=refbasicenumeration);
}
static int adms_basicinteger_cmp (const char* mybasicinteger,const char* refbasicinteger)
{
  return (mybasicinteger!=refbasicinteger);
}
static int adms_basicreal_cmp (const char* mybasicreal,const char* refbasicreal)
{
  return ((mybasicreal-refbasicreal)<0)||((mybasicreal-refbasicreal)>0);
}
static int adms_basicstring_cmp (const char* mybasicstring,const char* refbasicstring)
{
  return (strcmp(mybasicstring,refbasicstring));
}
@;
foreach(@$EA)
{
  my$ename=xname($_);
  next if not vla($ename);
  #cmp
  $adms_c.="static int adms_${ename}_cmp (p_$ename my$ename,p_$ename ref$ename)\n";
  $adms_c.="{\n";
  $adms_c.="  return\n";
  $adms_c.="  (\n";
  my$isfirst="    ";
  foreach(&uid($_))
  {
    my$xname=&xname($_);
    my$xdatatypename=&xdatatypename($_);
    if($xdatatypename eq "basicstring")
    {
      $adms_c.="${isfirst}strcmp(my$ename->_".&xname($_).",ref$ename->_".&xname($_).")";
      $isfirst="\n    ||\n    ";
    }
    elsif($xdatatypename eq "basicstring")
    {
      $adms_c.="${isfirst}strcmp(my$ename->_".&xname($_)."->_string,ref$ename->_".&xname($_)."->_string)";
      $isfirst="\n    ||\n    ";
    }
    elsif($xdatatypename eq "quark")
    {
      $adms_c.="${isfirst}strcmp(my$ename->_".&xname($_)."->_value,ref$ename->_".&xname($_)."->_value)";
      $isfirst="\n    ||\n    ";
    }
    elsif($xdatatypename eq "lexval")
    {
      $adms_c.="${isfirst}strcmp(my$ename->_".&xname($_)."->_string,ref$ename->_".&xname($_)."->_string)";
      $isfirst="\n    ||\n    ";
    }
    else
    {
      $adms_c.="$isfirst(my$ename->_".&xname($_)." != ref$ename->_".&xname($_).")";
      $isfirst="\n    ||\n    ";
    }
  }
  $adms_c.=$isfirst."1" if not &uid($_);
  $adms_c.="\n  );\n";
  $adms_c.="}\n";
}
foreach(@$EA)
{
  my$ename=xname($_);
  $adms_c.="\n/* ------- $ename -------------- */\n";
  $adms_c.=&mkfree($_);
  foreach(&attr($_))
  {
    #valueto
    my$xname=&xname($_);
    my$xdatatypename=&xdatatypename($_);
    $adms_c.="\nvoid adms_${ename}_valueto_$xname (p_$ename my$ename,".ccontainer($_)." _${xname})\n{\n";
#fixme: other basic?
    if(($xdatatypename eq "basicstring")&&(xsize($_) eq "scalar"))
    {
      $adms_c.="  my$ename->_$xname=adms_kclone(_$xname);\n";
    }
    elsif(($ename eq "admst")&&($xname eq "item"))
    {
      $adms_c.="  /*fixme: my$ename->_$xname=_$xname;*/\n";
    }
    else
    {
      $adms_c.="  my$ename->_$xname=_$xname;\n";
    }
    $adms_c.="  my$ename->_${xname}_isdefault=0;\n" if vla($ename);
    $adms_c.="}\n";
    next if xsize($_) eq "scalar";
    #prepend
    my$cdec=join",",map{"my".&xname($_)}&uid($EH->{xdatatypename($_)});
    if(($ename eq "module")&&($xname =~ "branch|node|probe|source"))
    {
      $adms_c.=ctype($_)." adms_${ename}_list_${xname}_prepend_by_id_once_or_ignore (p_$ename mymy${ename},".&hdec($_).")\n";
      $adms_c.="{\n";
      $adms_c.="  p_slist list=mymy$ename->_$xname;\n";
      $adms_c.="  ".ctype($_)." ref${xname}=adms_${xdatatypename}_new($cdec);\n";
      $adms_c.="  while(list)\n";
      $adms_c.="  {\n";
      $adms_c.="    if(!adms_${xdatatypename}_cmp((p_$xdatatypename)list->data,ref${xname}))\n";
      $adms_c.="    {\n";
      $adms_c.="      adms_${xdatatypename}_free (ref$xname);\n";
      $adms_c.="      return (p_$xdatatypename)list->data;\n";
      $adms_c.="    }\n";
      $adms_c.="    list=list->next;\n";
      $adms_c.="  }\n";
      $adms_c.="  adms_slist_push(&mymy$ename->_$xname,(p_adms)ref$xname);\n";
      $adms_c.="  return ref$xname;\n";
      $adms_c.="}\n";
    }
    if(
      (($ename =~ "admsmain")&&($xname =~ "module|nature"))
      ||
      (($ename =~ "module")&&($xname =~ "branchalias|instance|node|nodealias|range"))
    )
    {
      $adms_c.=ctype($_)." adms_${ename}_list_${xname}_prepend_by_id_once_or_abort (p_$ename mymy${ename},".&hdec($_).")\n";
      $adms_c.="{\n";
      $adms_c.="  p_slist list=mymy${ename}->_${xname};\n";
      $adms_c.="  ".ctype($_)." ref${xname}=adms_${xdatatypename}_new($cdec);\n";
      $adms_c.="  while(list)\n";
      $adms_c.="  {\n";
      $adms_c.="    if(!adms_${xdatatypename}_cmp((p_$xdatatypename)list->data,ref${xname}))\n";
      $adms_c.="    {\n";
      $adms_c.="      adms_message_fatal((\"${ename}=[%s] ${xname}=[%s] already defined\\n\",\n";
      $adms_c.="        adms_${ename}_uid(mymy${ename}),adms_${xdatatypename}_uid(ref${xname})))\n";
      $adms_c.="      adms_${xdatatypename}_free (ref${xname});\n";
      $adms_c.="    }\n";
      $adms_c.="    list=list->next;\n";
      $adms_c.="  }\n";
      $adms_c.="  adms_slist_push(&mymy${ename}->_${xname},(p_adms)ref$xname);\n";
      $adms_c.="  return ref${xname};\n";
      $adms_c.="}\n";
    }
    if(
      (($ename =~ "admsmain")&&($xname =~ "discipline"))
      ||
      (($ename =~ "analogfunction|block|module")&&($xname =~ "variable"))
      ||
      (($ename =~ "instance")&&($xname =~ "parameterset|terminal"))
      ||
      (($ename =~ "module")&&($xname =~ "analogfunction"))
      ||
      (($ename =~ "variable")&&($xname =~ "alias"))
    )
    {
      $adms_c.="void adms_${ename}_list_${xname}_prepend_once_or_abort (p_$ename mymy${ename},".ctype($_)." my${xname})\n";
      $adms_c.="{\n";
      $adms_c.="  p_slist list=mymy${ename}->_${xname};\n";
      $adms_c.="  while(list)\n";
      $adms_c.="  {\n";
      $adms_c.="    if(!adms_${xdatatypename}_cmp((p_$xdatatypename)list->data,my${xname}))\n";
      $adms_c.="    {\n";
      $adms_c.="      adms_message_fatal((\"${ename}=[%s] ${xname}=[%s] already defined\\n\",\n";
      $adms_c.="        adms_${ename}_uid(mymy${ename}),adms_${xdatatypename}_uid(my${xname})))\n";
      $adms_c.="      return;\n";
      $adms_c.="    }\n";
      $adms_c.="    list=list->next;\n";
      $adms_c.="  }\n";
      $adms_c.="  adms_slist_push(&mymy${ename}->_${xname},(p_adms)my$xname);\n";
      $adms_c.="}\n";
    }
    #lookup
    if(
      (($ename =~ "admsmain")&&($xname =~ "discipline"))
      ||
      (($ename =~ "admsmain")&&($xname =~ "module"))
      ||
      (($ename =~ "admsmain")&&($xname =~ "nature"))
      ||
      (($ename =~ "analogfunction")&&($xname =~ "variable"))
      ||
      (($ename =~ "module")&&($xname =~ "branchalias"))
      ||
      (($ename =~ "module")&&($xname =~ "node"))
      ||
      (($ename =~ "module")&&($xname =~ "variable"))
    )
    {
      $adms_c.=ctype($_)." adms_${ename}_list_${xname}_lookup_by_id (p_$ename mymy$ename,".&hdec($_).")\n";
      $adms_c.="{\n";
      $adms_c.="  p_slist list=mymy$ename->_$xname;\n";
      $adms_c.="  while(list) {\n";
      $adms_c.="    p_$xdatatypename ref$xdatatypename=(p_$xdatatypename)list->data;\n";
      $adms_c.="    if(";
      my@cmp;
      foreach(&uid($EH->{$xdatatypename}))
      {
        my$cur="ref$xdatatypename->_".&xname($_);
        my$ref="my".&xname($_);
        if(xdatatypename($_) eq "basicstring")
        {push @cmp,"!strcmp($cur,$ref)";}
        elsif(&xdatatypename($_) eq "lexval")
        {push @cmp,"!strcmp($cur->_string,$ref->_string)";}
        elsif(&xdatatypename($_) eq "quark")
        {push @cmp,"!strcmp($cur->_value,$ref->_value)";}
        elsif(&xdatatypename($_) eq "lexval")
        {push @cmp,"!strcmp($cur->_string,$ref->_string)";}
        else
        {push @cmp,"($cur==$ref)";}
      }
      $adms_c.=join "&&\n       ",@cmp;
      $adms_c.=")\n";
      $adms_c.="      return ref$xdatatypename;\n";
      $adms_c.="    list=list->next;\n";
      $adms_c.="  }\n";
      $adms_c.="  return NULL;\n";
      $adms_c.="}\n";
    }
  }
  next if $ename eq "adms";
  #new
  $adms_c.="p_$ename adms_${ename}_new (".&hdece($_).")\n";
  $adms_c.="{\n";
  $adms_c.="  p_$ename mynew$ename=(p_$ename) calloc(1,(size_t)sizeof(t_$ename));\n";
  $adms_c.="  adms_global_increment_nb${ename}new();\n" if($ename eq "admst");
  my$parent=&eparent($_);
  if($parent)
  {
    $adms_c.=&pnew($_,"mynew$ename->",$_,$parent);
  }
  foreach(&attr($_))
  {
    my$xname=&xname($_);
    my$xdatatypename=&xdatatypename($_);
    my$xdefault=&xdefault($_);
    if($_->parentNode->nodeName eq "uid")
    {
      if($xdatatypename eq "basicstring") {$adms_c.="  mynew$ename->_$xname=adms_kclone(my$xname);\n";}
      else                                {$adms_c.="  mynew$ename->_$xname=my$xname;\n";}
      $adms_c.="  mynew$ename->_${xname}_isdefault=1;\n" if vla($ename);
    }
    elsif(defined $xdefault)
    {
         if($xdatatypename eq "basicenumeration")     {$adms_c.="  mynew$ename->_$xname=admse_$xdefault;\n";}
      elsif($xdatatypename eq "basicinteger")         {$adms_c.="  mynew$ename->_$xname=$xdefault;\n"}
      elsif($xdatatypename eq "basicreal")            {$adms_c.="  mynew$ename->_$xname=$xdefault;\n"}
      elsif($xdatatypename eq "basicstring")          {$adms_c.="  mynew$ename->_$xname=adms_kclone(\"$xdefault\");\n";}
      elsif($xdatatypename eq "quark")                {$adms_c.="  mynew$ename->_$xname=adms_quark_new(\"$xdefault\");\n";}
      elsif($xdefault eq "")                          {$adms_c.="  mynew$ename->_$xname=NULL;\n";}
      $adms_c.="  mynew$ename->_${xname}_isdefault=1;\n" if vla($ename);
    }
    else
    {
      $adms_c.="  mynew$ename->_${xname}_isdefault=-1;\n" if vla($ename);
    }
  }
  $adms_c.="  return mynew$ename;\n";
  $adms_c.="}\n";
  $adms_c.=&mkuid($_);
  next if not vla($ename);
  #xnew
  my@inputs;
  my$i=0;
  my$check;
  my$free;
  my$nbinputs=scalar(@{uid($_)});
  $adms_c.="static p_admst adms_${ename}_xnew (p_ktransform mytransform,p_admst dot,p_slist myinputs[],const int size)\n{\n";
  #fixme: special prototype for variable
  if(${ename} eq "variable")
  {
    $adms_c.="  if(size==3)\n";
    $adms_c.="    return adms_variableprototype_xnew (mytransform,dot,myinputs,size);\n";
    $adms_c.="  else";
  }
  $adms_c.="  if(size!=$nbinputs)\n";
  $adms_c.="  {\n";
  $adms_c.="    adms_message_fatal_continue((\"admst:new($ename) bad number of inputs (found %i, expected $nbinputs)\\n\",size))\n";
  $adms_c.="    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mytransform)))\n";
  $adms_c.="    return NULL;\n";
  $adms_c.="  }\n";
  $adms_c.="  else\n";
  $adms_c.="  {\n";
  foreach(&uid($_))
  {
    my$xname=&xname($_);
    my$xdatatypename=&xdatatypename($_);
    if(0) {}
    elsif($xdatatypename eq "basicinteger")
    {
      $adms_c.="    char* my${xname}s=aprintf(mytransform,(p_admst)myinputs[$i]->data);\n";
      $adms_c.="    ".ctype($_)." my$xname=adms_strtol(mytransform,my${xname}s);\n";
      $free.="    free(my${xname}s);\n";
    }
    elsif($xdatatypename eq "basicreal")
    {
      $adms_c.="    char* my${xname}s=aprintf(mytransform,(p_admst)myinputs[$i]->data);\n";
      $adms_c.="    ".ctype($_)." my$xname=adms_strtod(mytransform,my${xname}s);\n";
      $free.="    free(my${xname}s);\n";
    }
    elsif($xdatatypename eq "basicstring")
    {
      $adms_c.="    char* my${xname}=aprintf(mytransform,(p_admst)myinputs[$i]->data);\n";
      $free.="    free(my${xname});\n";
    }
    elsif($xdatatypename eq "basicenumeration")
    {
      $adms_c.="    char* my${xname}s=aprintf(mytransform,(p_admst)myinputs[$i]->data);\n";
      $adms_c.="    admse my${xname}e=ns_strtoe(my${xname}s);\n";
      $adms_c.="    ".ctype($_)." my$xname=my${xname}e;\n";
      $check.="    if(!my${xname}e)\n";
      $check.="    {\n";
      $check.="      adms_message_fatal_continue((\"cannot convert '%s' to a valid value\\n\",my${xname}s))\n";
      $check.="      adms_message_fatal((\"see %s\\n\",adms_transform_uid(mytransform)))\n";
      $check.="    }\n";
      $free.="    free(my${xname}s);\n";
    }
    elsif(xsize($_) eq "list")
    {
      $adms_c.="    ".ccontainer($_)." my$xname=myinputs[$i];\n";
    }
    elsif($xdatatypename eq "lexval")
    {
      $adms_c.="    char* my${xname}s=aprintf(mytransform,(p_admst)myinputs[$i]->data);\n";
      $adms_c.="    ".ctype($_)." my$xname=adms_lexval_new(my${xname}s,adms_transform_uid(mytransform),0,0);\n";
      $free.="    free(my${xname}s);\n";
    }
    else
    {
      $adms_c.="    ".ccontainer($_)." my$xname=(p_$xdatatypename)((p_admst)myinputs[$i]->data)->_item.p;\n";
    }
    push @inputs,"my$xname";
    $i++;
  }
  $adms_c.="    /*xnew*/p_admst myadmst=adms_admst_newpa(dot,dot,(p_adms)adms_${ename}_new(".(join",",@inputs)."));\n";
  $adms_c.=$check if $check;
  $adms_c.=$free if $free;
  $adms_c.="    return myadmst;\n";
  $adms_c.="  }\n";
  $adms_c.="}\n";
}

my$VA;
foreach(@VA) {$VA.="  else if(!strcmp(ename,\"$_\")) return (p_new) adms_".$_."_xnew;\n";}
$adms_c.=qq[
p_new adms_admsxml_new (p_ktransform mytransform,const char* ename)
{
  if(0) {}
$VA  else if(!strcmp(ename,"basicenumeration")) return (p_new) adms_basicenumeration_xnew;
  else if(!strcmp(ename,"basicinteger")) return (p_new) adms_basicinteger_xnew;
  else if(!strcmp(ename,"basicreal")) return (p_new) adms_basicreal_xnew;
  else if(!strcmp(ename,"basicstring")) return (p_new) adms_basicstring_xnew;
  adms_message_fatal_continue(("adms element '%s' cannot be created using admst:new\\n",ename))
  adms_message_fatal(("see %s\\n",adms_transform_uid(mytransform)))
  return NULL;
}\n];
$VA="";
foreach(@VA) {$VA.="  if(myadmstread->_pseudo==admse_$_) return (p_cmp)adms_".$_."_cmp;\n";}
$adms_c.=qq[
p_cmp adms_admsxml_cmp (p_kadmst myadmst)
{
  p_admst myadmstread=aread(myadmst);
  if(myadmstread->_pseudo==admse_basicenumeration) return (p_cmp)adms_basicenumeration_cmp;
  if(myadmstread->_pseudo==admse_basicinteger) return (p_cmp)adms_basicinteger_cmp;
  if(myadmstread->_pseudo==admse_basicreal) return (p_cmp)adms_basicreal_cmp;
  if(myadmstread->_pseudo==admse_basicstring) return (p_cmp)adms_basicstring_cmp;
$VA  adms_message_fatal_continue(("%s:%i: internal error - should not be reached\\n",__FILE__,__LINE__))
  adms_message_fatal(("%s:%i: please report to r29173\@users.sourceforge.net\\n",__FILE__,__LINE__))
  return NULL;
}\n];

#miscC
$adms_c.="/*-- admst/new prototypes --*/\n";
foreach (qw[bd be bi br bs ks ns pa pn pc pd la ln pp])
{
my$field=$_ eq"bd"?"i":
         $_ eq"be"?"i":
         $_ eq"bi"?"i":
         $_ eq"br"?"r":
         $_ eq"bs"?"s":
         $_ eq"ns"?"s":
         $_ eq"ks"?"s":
         "p";
my$cast=$_ eq"bd"?"const admse":
        $_ eq"be"?"const admse":
        $_ eq"bi"?"const int":
        $_ eq"br"?"const double":
        $_ eq"bs"?"char*":
        $_ eq"ns"?"char*":
        $_ eq"ks"?"char*":
        $_ eq"pd"?"p_admstvariable":
        $_ eq"pn"?"p_admst":
        $_ eq"pc"?"p_attribute":
        $_ eq"la"?"p_slist":
        $_ eq"ln"?"p_slist":
        $_ eq"pp"?"admse":
        "const p_adms";
my$item=$_ eq"pn"?"item->_item.p":
        $_ eq"pd"?"(p_adms)item":
        $_ eq"pc"?"(p_adms)item":
        $_ eq"la"?"(p_adms)item":
        $_ eq"ln"?"(p_adms)item":
        $_ eq"pp"?"myprevious->_item.p":
        "item";
my$pseudocode=$_ eq"bd"?"admse_datatypename":
              $_ eq"be"?"admse_basicenumeration":
              $_ eq"bi"?"admse_basicinteger":
              $_ eq"br"?"admse_basicreal":
              $_ eq"bs"?"admse_basicstring":
              $_ eq"ns"?"admse_basicstring":
              $_ eq"ks"?"admse_basicstring":
              $_ eq"pa"?"item?item->_datatypename:admse_empty":
              $_ eq"pn"?"item->_pseudo":
              $_ eq"pc"?"admse__croix":
              $_ eq"pd"?"admse_basiclist":
              $_ eq"la"?"admse__ladms":
              $_ eq"ln"?"admse__ladmst":
              $_ eq"pp"?"item":
              "mypseudo";
my$extra=$_ eq"ns"?"myadmst->_hasnewbasicstring=1;\n  ":
         $_ eq"ks"?"myadmst->_refd=0;\n  ":
         "";
my$inputs0=$_ eq"ks"?"$cast item":
         "p_admst myprevious,p_admst mypreviouspath,$cast item";
my$inputs1=$_ eq"ks"?"NULL,NULL,$pseudocode":
         "myprevious,mypreviouspath,$pseudocode";
$adms_c.="p_admst adms_admst_new$_ ($inputs0)
{
  p_admst myadmst=adms_admst_new($inputs1);
  ".(($_ ne"pn")?"myadmst->_item.$field=$item;":"if(item->_hasnewbasicstring==1)
  {
    myadmst->_item.s=item->_item.s;
    myadmst->_original=item;
    item->_refd++;
  }
  else
    myadmst->_item.p=item->_item.p;").(($_ ne"bs")?"":"\n  if(myprevious->_hasnewbasicstring==2)
  {
    myadmst->_original=myprevious;
    myprevious->_refd++;
  }")."
  ${extra}return myadmst;
}\n";
}
$adms_c.=q[
/*-- Miscellaneous routines --*/
p_admst aread (p_admst myadmst)
{
  p_admst myadmstread=(myadmst&&myadmst->_pseudo==admse__croix)?(p_admst)((p_attribute)myadmst->_item.p)->_value:myadmst;
  return myadmstread;
}
int adms_strtol (p_ktransform mytransform,const char* mynumber)
{
  int nb;
  char* tail;
  errno=0;
  nb=strtol(mynumber,&tail,10);
  if(*tail||errno)
  {
    adms_message_fatal_continue(("%s: cannot convert value to valid integer value\n",mynumber))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  return nb;
}
double adms_strtod (p_ktransform mytransform,const char* myreal)
{
  double nb;
  char* tail;
  errno=0;
  nb=strtod(myreal,&tail);
  if(*tail||errno)
    return adms_NAN;
  if(*tail||errno)
  {
    adms_message_fatal_continue(("%s: cannot convert value to valid real value\n",myreal))
    adms_message_fatal(("see %s\n",adms_transform_uid(mytransform)))
  }
  return nb;
}
void bp (void) {}
char*adms_integertostring (int value)
{
   char*string=malloc(sizeof(char)*50);
   sprintf(string,"%i",value);
   return string;
}
char*adms_doubletostring (double value)
{
   if(isnan(value))
   {
     char*string=malloc(sizeof(char)*4);
     sprintf(string,"nan");
     return string;
   }
   else
   {
     char*string=malloc(sizeof(char)*50);
     sprintf(string,"%e",value);
     return string;
   }
}
int adms_file_isregular (const char* myfilename)
{
  return((access(myfilename,ADMS_F_OK)==0));
}
int adms_file_isdirectory (const char* myfilename)
{
  struct stat s;
  return((stat(myfilename,&s)==0)&&(s.st_mode&ADMS_S_IFDIR));
}
int adms_setenv (const char* myname,const char* myvalue)
{
  int myres;
#if defined(HAVE_PUTENV)
  char*myassign=NULL;
  adms_k2strconcat(&myassign,myname);
  adms_k2strconcat(&myassign,"=");
  adms_k2strconcat(&myassign,myvalue);
  myres=putenv(myassign);
  free(myassign);
#else
  myres=setenv(myname,myvalue,1);
#endif
  return (myres==0);
}
p_slist adms_slist_last (p_slist l)
{
  if(l)
  {
    while(l->next)
      l=l->next;
  }
  return l;
}
p_slist adms_slist_new (p_adms d)
{
  p_slist newl=NULL;
  adms_slist_push(&newl,d);
  return newl;
}
p_slist adms_slist_copy (p_slist l)
{
  p_slist copiedl=NULL;
  while(l)
  {
    adms_slist_push(&copiedl,l->data);
    l=l->next;
  }
  return adms_slist_reverse(copiedl);
}
void adms_slist_push (p_slist* l,p_adms data)
{
  p_slist n=malloc(sizeof(t_slist));
  n->next=*l;
  n->data=data;
  *l=n;
}
p_adms adms_slist_pull (p_slist* l)
{
  if(*l)
  {
    p_slist n=*l;
    p_adms data=n->data;
    *l=(*l)->next;
    free(n);
    return data;
  }
  return NULL;
}
void adms_slist_concat (p_slist* l1,p_slist l2)
{
  if(l2)
  {
    if(*l1)
      adms_slist_last(*l1)->next=l2;
    else
      *l1=l2;
  }
}
unsigned int adms_slist_length (p_slist l)
{
  unsigned int length=0;
  while(l)
  {
    length++;
    l=l->next;
  }
  return length;
}
p_slist adms_slist_nth (p_slist l,unsigned int  n)
{
  while (n-->0 && l)
    l=l->next;
  return l;
}
p_adms adms_slist_nth_data (p_slist l,unsigned int n)
{
  while (n-->0 && l)
    l=l->next;
  return l ? l->data : ((p_adms)0);
}
p_slist adms_slist_find (p_slist l,p_kadms data)
{
  while(l)
  {
    if(l->data==data)
      break;
    l=l->next;
  }
  return l;
}
int adms_slist_index (p_slist l, p_kadms data)
{
  int i=0;
  while(l)
  {
    if(l->data==data)
      return i;
    i++;
    l=l->next;
  }
  return -1;
}
p_slist adms_slist_reverse (p_slist l)
{
  p_slist p=NULL;
  while(l)
  {
    p_slist n=l->next;
    l->next=p;
    p=l;
    l=n;
  }
  return p;
}
void adms_slist_inreverse (p_slist* l)
{
  *l=adms_slist_reverse(*l);
}
void adms_slist_free (p_slist l)
{
  while(l)
  {
    p_slist freed=l;
    l=l->next;
    free(freed);
  }
}
int globalnbadmstnew=0, globalnbadmstdestroy=0;
int adms_global_nbadmstnew (void) {return globalnbadmstnew;}
int adms_global_nbadmstdestroy (void) {return globalnbadmstdestroy;}
void adms_global_increment_nbadmstnew (void) {globalnbadmstnew++;}
void adms_global_increment_nbadmstdestroy (void) {globalnbadmstdestroy++;}
p_admsmain globaladmsmain;
p_admsmain root (void) {return globaladmsmain;}
void rootnew (p_admsmain myglobaladmsmain) {globaladmsmain=myglobaladmsmain;}

/*-- Messaging --*/
void adms_toupper (char*m)
{
  for(;*m;m++)
    *m=toupper(*m);
}
void adms_tolower (char*m)
{
  for(;*m;m++)
    *m=tolower(*m);
}
char*adms_kclone (const char* m)
{
  if(m)
  {
    int l=strlen(m);
    char*mycpy=(char*)malloc((l+1)*sizeof(char));
    memcpy(mycpy,m,l);
    mycpy[l]='\0';
    return mycpy;
  }
  else
    return NULL;
}
char*adms_knclone (const char* m,const int l)
{
  char*mycpy=(char*)malloc((l+1)*sizeof(char));
  memcpy(mycpy,m,l);
  mycpy[l]='\0';
  return mycpy;
}
/*clone array (m,n(: "abcdefghijkl\0" returns as "cdef\0"*/
/*                      m   n                            */
/* same as adms_knclone(m,n-m)                           */
char*adms_m2nclone (const char* m,const char* n)
{
  int l=(char*)n-(char*)m;
  char*mycpy=(char*)malloc((l+1)*sizeof(char));
  memcpy(mycpy,m,l);
  mycpy[l]='\0';
  return mycpy;
}
/*s1: realloced s2: constant ret: s1=s1s2*/
void adms_k2strconcat (char **s1,const char* s2)
{
  if(!s2)
    return;
  if(*s1)
  {
    int l1=strlen(*s1);
    int l2=strlen(s2);
    *s1=(char*)realloc(*s1,(l1+l2+1)*sizeof(char));
    memcpy(*s1+l1,s2,l2+1);
  }
  else
    *s1=strdup(s2);
}
/*s1: realloced s2: freed ret: s1=s1s2*/
void adms_strconcat (char **s1,char *s2)
{
  adms_k2strconcat(s1,s2);
  free(s2);
}
FILE* adms_file_open_read (const char* myfilename)
{
  FILE* fh=fopen(myfilename,"r");
  if(!fh)
    adms_message_fatal(("%s: failed to open file [read mode]\n",myfilename))
  return fh;
}

];

foreach my$mess(@message)
{
my$name=$mess->[0];
my$ofh=$mess->[1];
my$isfatal=($mess->[2]==1);
my$iserror=($mess->[2]==2);
my$prefix=sprintf("\"[%-7s]",$name);
$prefix=~s/ /\./g;
$prefix.=" \"";
$adms_c.="_t_message (adms_message_${name}_impl)\n";
$adms_c.="{\n";
$adms_c.="  va_list ap;\n";
$adms_c.="  int insideformat=0;\n";
$adms_c.="  int i;\n";
$adms_c.="  char* s;\n";
$adms_c.="  char c;\n";
$adms_c.="  double d;\n";
$adms_c.="  void* p;\n";
$adms_c.="  fputs($prefix,$ofh);\n" if($mess->[2]>=0);
$adms_c.="  va_start(ap, format);\n";
$adms_c.="  for(;*format;format++)\n";
$adms_c.="  {\n";
$adms_c.="    if(insideformat)\n";
$adms_c.="    {\n";
$adms_c.="      insideformat=0;\n";
$adms_c.="      switch(*format) \n";
$adms_c.="      {\n";
$adms_c.="        case 's':\n";
$adms_c.="          s=va_arg (ap,char*);\n";
$adms_c.="          if(s) fputs(s,$ofh); else fputs(\"NULL\",$ofh);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'e':\n";
$adms_c.="          d=va_arg (ap,double);\n";
$adms_c.="          fprintf($ofh,\"%e\",d);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'c':\n";
$adms_c.="          c=va_arg (ap,int);\n";
$adms_c.="          fprintf($ofh,\"%c\",c);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'g':\n";
$adms_c.="          d=va_arg (ap,double);\n";
$adms_c.="          fprintf($ofh,\"%g\",d);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'f':\n";
$adms_c.="          d=va_arg (ap,double);\n";
$adms_c.="          fprintf($ofh,\"%f\",d);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'i':\n";
$adms_c.="          i=va_arg (ap,int);\n";
$adms_c.="          fprintf($ofh,\"%i\",i);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'p':\n";
$adms_c.="          p=va_arg (ap,void*);\n";
$adms_c.="          fprintf($ofh,\"%p\",p);\n";
$adms_c.="          break;\n";
$adms_c.="        default:\n";
$adms_c.="          fputc(*format,$ofh);\n";
$adms_c.="      }\n";
$adms_c.="    }\n";
$adms_c.="    else\n";
$adms_c.="    {\n";
$adms_c.="      switch(*format) \n";
$adms_c.="      {\n";
$adms_c.="        case '%':\n";
$adms_c.="          insideformat=1;\n";
$adms_c.="          break;\n";
$adms_c.="        default:\n";
$adms_c.="          fputc(*format,$ofh);\n";
$adms_c.="      }\n";
$adms_c.="    }\n";
$adms_c.="  }\n";
$adms_c.="  va_end (ap);\n";
$adms_c.="  fflush($ofh);\n";
$adms_c.="  bp();\n" if($iserror);
$adms_c.="  bp(),exit(1);\n" if($isfatal);
$adms_c.="}\n";
$adms_c.="_t_message (adms_message_${name}_continue_impl)\n";
$adms_c.="{\n";
$adms_c.="  va_list ap;\n";
$adms_c.="  int insideformat=0;\n";
$adms_c.="  int i;\n";
$adms_c.="  char* s;\n";
$adms_c.="  char c;\n";
$adms_c.="  double d;\n";
$adms_c.="  void* p;\n";
$adms_c.="  fputs($prefix,$ofh);\n" if($isfatal || $iserror);
$adms_c.="  va_start(ap, format);\n";
$adms_c.="  for(;*format;format++)\n";
$adms_c.="  {\n";
$adms_c.="    if(insideformat)\n";
$adms_c.="    {\n";
$adms_c.="      insideformat=0;\n";
$adms_c.="      switch(*format) \n";
$adms_c.="      {\n";
$adms_c.="        case 's':\n";
$adms_c.="          s=va_arg (ap,char*);\n";
$adms_c.="          if(s) fputs(s,$ofh); else fputs(\"NULL\",$ofh);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'c':\n";
$adms_c.="          c=va_arg (ap,int);\n";
$adms_c.="          fputc(c,$ofh);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'e':\n";
$adms_c.="          d=va_arg (ap,double);\n";
$adms_c.="          fprintf($ofh,\"%e\",d);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'g':\n";
$adms_c.="          d=va_arg (ap,double);\n";
$adms_c.="          fprintf($ofh,\"%g\",d);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'f':\n";
$adms_c.="          d=va_arg (ap,double);\n";
$adms_c.="          fprintf($ofh,\"%f\",d);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'i':\n";
$adms_c.="          i=va_arg (ap,int);\n";
$adms_c.="          fprintf($ofh,\"%i\",i);\n";
$adms_c.="          break;\n";
$adms_c.="        case 'p':\n";
$adms_c.="          p=va_arg (ap,void*);\n";
$adms_c.="          fprintf($ofh,\"%p\",p);\n";
$adms_c.="          break;\n";
$adms_c.="        default:\n";
$adms_c.="          fputc(*format,$ofh);\n";
$adms_c.="      }\n";
$adms_c.="    }\n";
$adms_c.="    else\n";
$adms_c.="    {\n";
$adms_c.="      switch(*format) \n";
$adms_c.="      {\n";
$adms_c.="        case '%':\n";
$adms_c.="          insideformat=1;\n";
$adms_c.="          break;\n";
$adms_c.="        default:\n";
$adms_c.="          fputc(*format,$ofh);\n";
$adms_c.="      }\n";
$adms_c.="    }\n";
$adms_c.="  }\n";
$adms_c.="  va_end (ap);\n";
$adms_c.="  fflush($ofh);\n";
$adms_c.="}\n";
}

open FILE,">$top_srcdir/admsXml/adms.c"; print FILE $adms_c; close FILE;
print "$top_srcdir/admsXml/adms.c: created\n";

#apath language

map{my$e=$_;my$ename=&xname($e);
map{push @{$A{xname($_)}},[$e,$_] if (&xname($_) ne "datatypename");}
&attr($_) if vla($ename)||$ename eq "return"||$ename eq "math"||$ename eq "subexpression";} @$EA;

my@Location03;
foreach(@$EA)
{
  my$ename=&xname($_);
  next if not $ename eq "adms"||$ename eq "math"||$ename eq "subexpression";
  push @Location03,"  IFIDENT($ename)\n";
  foreach($_->findnodes(".//element"))
  {
    next if not vla(xname($_));
    push @Location03,"    MKscalar(".xname($_).",sub)\n";
  }
  push @Location03,"  ENDIFIDENT\n";
}
foreach(keys(%A))
{
  my($aname,$ee)=($_,$A{$_});
  push @Location03,"  IFIDENT($aname)\n";
  foreach(@$ee)
  {
    my($e,$a)=@$_;
    if(xsize($a) eq "list")
    {
      push @Location03,"    MKlist(".xname($e).",".xdatatypename($a).")\n";
    }
    else
    {
      push @Location03,"    MKscalar(".xname($e).",".xdatatypename($a).")\n";
    }
  }
  push @Location03,"  ENDIFIDENT\n";
}


#dtd
sub LN
{
  my($type,$rhs)=(shift,shift);
  return "p_admst d=adms_admst_new$type(ai,ai,$rhs);".&ALLLASTPOS("d");
}
sub LLNN
{
  my($type,$rhs)=(shift,shift);
  return "p_admst d=adms_admst_new$type(dot,dot,$rhs);".&ALLLASTPOS("d");
}
open admstpath_dtd,">$top_srcdir/admsXml/admstpath.dtd";
my$dtd=q[
<?xml version="1.0" encoding="UTF-8"?>
<!-- Spec interne d'un chemin 'admstpath'. Validation: xmllint dbgp.xml -noout -postvalid -noblanks -->
<!--
Fichier dbgp.xml
<?xml version="1.0"?>
<!--?xml-stylesheet type="text/xsl" href="admsstyle.xsl"?-->
<!DOCTYPE admstpath SYSTEM "admstpath.dtd">
<!-- Exemple de donnees 'admstpath' -->
<admstpath>
<!-- inserer ici les elements <p/> -->
</admstpath>
-->
<!ELEMENT admstpath (p+) >
];

print admstpath_dtd $dtd;
close admstpath_dtd;
print "$top_srcdir/admsXml/admstpath.dtd: created\n";

open admstpathYacc_h,">$top_srcdir/admsXml/admstpathYacc.h";
print admstpathYacc_h "$BUILDER
#include \"adms.h\"

p_slist globalfileoutputlist;
int break_found;

FILE *OUT(void);
int is_admst (const char* name);
int apath_main (int argc,char **argv);
void dbxx (p_transform mytransform);
void postxx (p_transform mytransform);
void sanityxx (p_transform mytransform);
p_transform lookfortemplates(p_transform mytransform,const char*match);
p_path pparse (p_transform mytransform,const char* aname,const char* avalue);
p_text tparse (p_ktransform mytransform,const char* aname,const char* avalue);
p_admst adms_pull_admst (p_transform mytransform);
p_admstvariable lookup_dollar (const char* myid);
void tprintf (p_kadmst myadmst, p_ktext mytext);
char* tsprintf (p_kadmst myadmst, p_ktext mytext);
void free_ptraverse (p_ptraverse p);
p_ptraverse bar (p_admst dot,p_path mypath0);
void Xassert (p_transform mytransform,p_admst dot,p_admst dotdot);
void xtraverse (p_slist mytransforms,p_admst dot,p_admst dotdot);
";
close admstpathYacc_h;
print "$top_srcdir/admsXml/admstpathYacc.h: created\n";


open admstpathYacc_y,">$top_srcdir/admsXml/admstpathYacc.y";
print admstpathYacc_y "$BUILDER
%define api.prefix \"admstpath\"
%lex-param {p_pparse mypparse}
%parse-param {p_pparse mypparse}
\%{
#include \"admstpathYacc.h\"
";

sub REQUIRECHILD
{
  return "    if(!mytransform->_children) adms_message_fatal((\"transform requires children - see %s\\n\",adms_transform_uid(mytransform)))\n";
}
sub EXCLUDECHILD
{
  return "    if(mytransform->_children) adms_message_fatal((\"transform does not support children - see %s\\n\",adms_transform_uid(mytransform)))\n";
}
sub EXCLUDE
{
 my($type,$item)=@_;
 return "    if(mytransform->_$type$item) adms_message_fatal((\"transform does not support attribute '$item' - see %s\\n\",adms_transform_uid(mytransform)))\n";
}
sub REQUIRE
{
 my($type,$item)=@_;
 return "    if(!mytransform->_$type$item) adms_message_fatal((\"transform requires attribute '$item' - see %s\\n\",adms_transform_uid(mytransform)))\n";
}
sub OBSOLETE
{
 my($obsolete,$new)=@_;
 return "    if(!strcmp(mytransform->_name,\"$obsolete\")) adms_message_obsolete((\"%s: please use '$new' instead\\n\",adms_transform_uid(mytransform)))\n";
}
print admstpathYacc_y '
static int admstpathlex (p_pparse mypparse);
static void dbpath (p_path mypath);
static void dbtext (p_text mytext);
static void postpath (p_path mypath);
static void posttext (p_text mytext)
{
  p_slist li;
  for(li=mytext->_token;li;li=li->next)
  {
    p_adms mytoken=li->data;
    if(mytoken->_datatypename==admse_path)
    {
      ((p_path)mytoken)->_aname=adms_kclone("//");
      adms_k2strconcat(&((p_path)mytoken)->_aname,mytext->_aname);
      postpath((p_path)mytoken);
    }
    else if(mytoken->_datatypename==admse_text)
    {
      ((p_text)mytoken)->_aname=adms_kclone("//");
      adms_k2strconcat(&((p_text)mytoken)->_aname,mytext->_aname);
      posttext((p_text)mytoken);
    }
  }
}
void dbxx (const p_transform mytransform)
{
  if(!is_admst(mytransform->_name))
  {
    p_slist l=mytransform->_attribute;
    for(;l;l=l->next)
      dbtext((p_text)((p_attribute)l->data)->_value);
  }
';
foreach($admstxml->findnodes("//transform"))
{
  my$tname=xname($_);
  foreach($_->findnodes(".//attribute|.//attribute-implied|.//attribute-warning"))
  {
    my$type=xtype($_);
    my$aname=$type.xname($_);
    print admstpathYacc_y "  if(!strcmp(mytransform->_name,\"$tname\")&&mytransform->_$aname) db$type(mytransform->_$aname);\n";
  }
  foreach($_->findnodes(".//attribute-list"))
  {
    my$type=xtype($_);
    my$aname=$type.xname($_);
    print admstpathYacc_y "  if(!strcmp(mytransform->_name,\"$tname\")&&mytransform->_$aname)\n";
    print admstpathYacc_y "  {\n";
    print admstpathYacc_y "    p_slist l=mytransform->_$aname;\n";
    print admstpathYacc_y "    for(;l;l=l->next)\n";
    print admstpathYacc_y "      db$type((p_$type)l->data);\n";
    print admstpathYacc_y "  }\n";
  }
}
print admstpathYacc_y "}\n";

print admstpathYacc_y "void postxx (p_transform mytransform)\n";
print admstpathYacc_y "{\n";
print admstpathYacc_y "  if(!is_admst(mytransform->_name))
  {
    p_slist l=mytransform->_attribute;
    for(;l;l=l->next)
      posttext((p_text)((p_attribute)l->data)->_value);
  }
";
foreach($admstxml->findnodes("//transform"))
{
  my$tname=xname($_);
  foreach($_->findnodes(".//attribute|.//attribute-implied|.//attribute-warning"))
  {
    my$type=xtype($_);
    my$aname=$type.xname($_);
    print admstpathYacc_y "  if(!strcmp(mytransform->_name,\"$tname\")&&mytransform->_$aname) post$type(mytransform->_$aname);\n";
  }
  foreach($_->findnodes(".//attribute-list"))
  {
    my$type=xtype($_);
    my$aname=$type.xname($_);
    print admstpathYacc_y "  if(!strcmp(mytransform->_name,\"$tname\")&&mytransform->_$aname)\n";
    print admstpathYacc_y "  {\n";
    print admstpathYacc_y "    p_slist l=mytransform->_$aname;\n";
    print admstpathYacc_y "    for(;l;l=l->next)\n";
    print admstpathYacc_y "      post$type((p_$type)l->data);\n";
    print admstpathYacc_y "  }\n";
  }
}
print admstpathYacc_y "}\n";

print admstpathYacc_y 'void sanityxx (p_transform mytransform)'."\n";
print admstpathYacc_y '{'."\n";
print admstpathYacc_y '  if(!is_admst(mytransform->_name)) { }'."\n";
foreach($admstxml->findnodes("//transform"))
{
  my$transform=$_;
  my@ids;
  foreach($transform->findnodes("obsolete"))
  {
    push @ids,"!strcmp(mytransform->_name,\"".xname($_)."\")";
  }
  push @ids,"!strcmp(mytransform->_name,\"".xname($transform)."\")";
  print admstpathYacc_y "  else if(".(join "||",@ids).")\n  {\n";
  print admstpathYacc_y &REQUIRECHILD if(defined xhaschild($transform) && (xhaschild($transform) eq "yes"));
  print admstpathYacc_y &EXCLUDECHILD if(defined xhaschild($transform) && (xhaschild($transform) eq "no"));
  foreach($transform->findnodes("obsolete|exclude|attribute"))
  {
    if($_->nodeName eq "attribute")
    {
      print admstpathYacc_y &REQUIRE(xtype($_),xname($_));
    }
    elsif($_->nodeName eq "exclude")
    {
      print admstpathYacc_y &EXCLUDE(xtype($_),xname($_));
    }
    elsif($_->nodeName eq "obsolete")
    {
      print admstpathYacc_y &OBSOLETE(xname($_),xname($transform));
    }
  }
  print admstpathYacc_y "  }\n";
}
print admstpathYacc_y '  else adms_message_fatal(("%s: unknown transform\n",adms_transform_uid(mytransform)))'."\n";
print admstpathYacc_y '}'."\n";
sub ALIASADMST {my$d=shift;return " adms_slist_push(&p->_aliasadmst,(p_adms)$d); ";}
sub arrachage {my$ai=shift;return "if($ai->_preva) $ai->_preva->_nexta=$ai->_nexta,$ai->_preva=NULL; if($ai->_nexta) $ai->_nexta->_preva=$ai->_preva,$ai->_nexta=NULL;";}
sub insertion {my$ai=shift;return "if(p->_a1==NULL) p->_a1=p->_an=$ai; else p->_an->_nexta=$ai,$ai->_preva=p->_an,p->_an=$ai;";}
sub fixme {my$ai=shift;return &arrachage($ai).&insertion($ai);}
sub ALLLASTPOS {my$ai=shift;return &fixme($ai)."$ai->_position=++p->_position;";}
foreach (@Location03)
{
  if(/MK\w+\((\w+),(\w+)\)/) {$e=$1;$d=$2;}
  if(/IFIDENT\((\w+)\)/)
  {
    $a=$1;
    print admstpathYacc_y "static void location030$a\(p_ptraverse p,p_path mypath,p_admst dot)\n";
    print admstpathYacc_y "{\n";
    print admstpathYacc_y "  p_admst ai=aread(dot);\n";
    print admstpathYacc_y "  admse e;\n";
    print admstpathYacc_y "  if(ai)\n";
    print admstpathYacc_y "    e=ai->_pseudo;\n";
    print admstpathYacc_y "  else\n";
    print admstpathYacc_y "  {\n";
    print admstpathYacc_y "    p_admst d=adms_admst_newpa(ai,ai,NULL);".&ALLLASTPOS("d")." return;\n";
    print admstpathYacc_y "  }\n";
    print admstpathYacc_y "  if(0) {}\n";
    if($a eq "dependency")
    {
      foreach(@MATHELEMENTS)
      {
        my($ealias,$e)=@{$_};
        print admstpathYacc_y "  else if(e==admse_$ealias)\n";
        print admstpathYacc_y "    e=admse_$e;\n";
      }
    }
    foreach(@{$ALIAS{$a}})
    {
      my($ealias,$e)=@{$_};
      print admstpathYacc_y "  else if(e==admse_$ealias)\n";
      print admstpathYacc_y "    e=admse_$e;\n";
    }
    print admstpathYacc_y "  if(0) {}\n";
    foreach(@{$ALIASBIS{$a}})
    {
      my($elt,$palias)=@{$_};
      my@ealias=split "/",$palias;
      print admstpathYacc_y "  else if(e==admse_$elt)\n";
      print admstpathYacc_y "  {\n";
      my$idx=0;
      my$xdatatypename;
      foreach(@ealias)
      {
        my$path=$_;
        my$islast=scalar(@ealias)==$idx+1;
        my$prevpath=$xdatatypename?$xdatatypename:$elt;
        my$prevai=$xdatatypename?"d".($idx-1):"ai";
        my$attr=$EH->{$prevpath}->findnodes("((uid/scalar)|(uid/list)|scalar|list)[\@name='$path']")->[0];
        $xdatatypename=&xdatatypename($attr);
        if(xsize($attr) eq "list")
        {
          print admstpathYacc_y "    /*AAAA0*/ p_slist l$idx=((p_$prevpath)$prevai->_item.p)->_$path;\n";
          print admstpathYacc_y "    for(;l$idx;l$idx=l$idx->next)\n";
          print admstpathYacc_y "    {\n";
          print admstpathYacc_y "      p_admst d$idx=adms_admst_newpa($prevai,ai,l$idx->data);".&ALLLASTPOS("d$idx")."\n";
          print admstpathYacc_y "    }\n";
        }
        else
        {
          print admstpathYacc_y "    /*BBBB0*/ ";
          if($xdatatypename eq "basicenumeration")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbe($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          elsif($xdatatypename eq "basicinteger")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbi($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          elsif($xdatatypename eq "basicreal")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbr($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          elsif($xdatatypename eq "basicstring")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbs($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          else
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newpa($prevai,ai,(p_adms)((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          print admstpathYacc_y &ALIASADMST("d$idx") if not $islast;
          print admstpathYacc_y &ALLLASTPOS("d$idx") if $islast;
          print admstpathYacc_y "d$idx->_valueto=(void*)adms_${prevpath}_valueto_$path;\n";
        }
        $idx++;
      }
      print admstpathYacc_y "  }\n";
    }
  }
  elsif(/MKlist/)
  {
    if($e eq "list" && $a eq "item") {$code="p_admst d=adms_admst_newpn(dot,dot,(p_admst)lii->data);";}
    elsif($d eq "basicstring")       {$code="p_admst d=adms_admst_newbs(ai,ai,(char*)lii->data);";}
    else                             {$code="p_admst d=adms_admst_newpa(ai,ai,lii->data);";}
    print admstpathYacc_y
"  else if(e==admse_$e)
  {
    p_slist lii=((p_$e)ai->_item.p)->_$a;
    for(;lii;lii=lii->next)
    {
      $code".&ALLLASTPOS("d")."
    }
  }
";
  }
  elsif(/MKscalar/)
  {
    print admstpathYacc_y "  else if(e==admse_$e)\n  {\n";
    if(/MKscalar\(attribute,adms\)/)
    {
      print admstpathYacc_y "    p_admst d=adms_admst_newpc(ai,ai,(p_attribute)ai->_item.p);";
    }
    elsif($d eq "sub")
    {
      print admstpathYacc_y "    p_admst d=adms_admst_newpp(ai,ai,admse_$a); d->_valueto=(void*)NULL;";
    }
    elsif($d eq "basicenumeration")
    {
      print admstpathYacc_y "    p_admst d=adms_admst_newbe(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    elsif($d eq "basicinteger")
    {
      print admstpathYacc_y "    p_admst d=adms_admst_newbi(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    elsif($d eq "basicreal")
    {
      print admstpathYacc_y "    p_admst d=adms_admst_newbr(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    elsif($d eq "basicstring")
    {
      print admstpathYacc_y "    p_admst d=adms_admst_newbs(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    else
    {
      print admstpathYacc_y "    p_admst d=adms_admst_newpa(ai,ai,(p_adms)((p_$e)(ai->_item.p))->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    print admstpathYacc_y &ALLLASTPOS("d")."\n  }\n";
  }
  if(/ENDIFIDENT/)
  {
    print admstpathYacc_y "  else\n";
    print admstpathYacc_y "  {\n";
    print admstpathYacc_y "    ".&LN("pn","ai")."\n";
    print admstpathYacc_y "    adms_message_error((\"%s: '$a' bad attribute\\n\",adms_transform_uid(p->_transform)))\n";
    print admstpathYacc_y "  }\n";
    print admstpathYacc_y "}\n";
  }
}
print admstpathYacc_y "\n";
foreach (@Location03)
{
  if(/MK\w+\((\w+),(\w+)\)/) {$e=$1;$d=$2;}
  if(/IFIDENT\((\w+)\)/)
  {
    $a=$1;
    print admstpathYacc_y "static void location031$a\(p_ptraverse p,p_path mypath,p_admst dot)\n";
    print admstpathYacc_y "{\n";
    print admstpathYacc_y "  p_admst ai=aread(dot);\n";
    print admstpathYacc_y "  admse e;\n";
    print admstpathYacc_y "  if(ai)\n";
    print admstpathYacc_y "    e=ai->_pseudo;\n";
    print admstpathYacc_y "  else\n";
    print admstpathYacc_y "  {\n";
    print admstpathYacc_y "    p_admst d=adms_admst_newpa(ai,ai,NULL);".&ALLLASTPOS("d")." return;\n";
    print admstpathYacc_y "  }\n";
    print admstpathYacc_y "  if(0) {}\n";
    if($a eq "dependency")
    {
      foreach(@MATHELEMENTS)
      {
        my($ealias,$e)=@{$_};
        print admstpathYacc_y "  else if(e==admse_$ealias)\n";
        print admstpathYacc_y "    e=admse_$e;\n";
      }
    }
    foreach(@{$ALIAS{$a}})
    {
      my($ealias,$e)=@{$_};
      print admstpathYacc_y "  else if(e==admse_$ealias)\n";
      print admstpathYacc_y "    e=admse_$e;\n";
    }
    print admstpathYacc_y "  if(0) {}\n";
    foreach(@{$ALIASBIS{$a}})
    {
      my($elt,$palias)=@{$_};
      my@ealias=split "/",$palias;
      print admstpathYacc_y "  else if(e==admse_$elt)\n";
      print admstpathYacc_y "  {\n";
      my$idx=0;
      my$xdatatypename;
      foreach(@ealias)
      {
        my$path=$_;
        my$islast=scalar(@ealias)==$idx+1;
        my$prevpath=$xdatatypename?$xdatatypename:$elt;
        my$prevai=$xdatatypename?"d".($idx-1):"ai";
        my$attr=$EH->{$prevpath}->findnodes("((uid/scalar)|(uid/list)|scalar|list)[\@name='$path']")->[0];
        $xdatatypename=&xdatatypename($attr);
        my$field;
        my$cast;
        if($xdatatypename eq "basicenumeration")
        {$field="i";$cast="";}
        elsif($xdatatypename eq "basicinteger")
        {$field="i";$cast="";}
        elsif($xdatatypename eq "basicreal")
        {$field="r";$cast="";}
        elsif($xdatatypename eq "basicstring")
        {$field="s";$cast="";}
        else
        {$field="p";$cast="(p_adms)";}
        if(xsize($attr) eq "list")
        {
          die if not $islast;
          print admstpathYacc_y "    /*AAAA1*/ p_admst d$idx=adms_admst_newla($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
        }
        else
        {
          print admstpathYacc_y "    /*BBBB1*/ ";
          if($xdatatypename eq "basicenumeration")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbe($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          elsif($xdatatypename eq "basicinteger")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbi($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          elsif($xdatatypename eq "basicreal")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbr($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          elsif($xdatatypename eq "basicstring")
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newbs($prevai,ai,((p_$prevpath)$prevai->_item.p)->_$path);";
          }
          else
          {
            print admstpathYacc_y "p_admst d$idx=adms_admst_newpa($prevai,ai,(p_adms)((p_$prevpath)$prevai->_item.p)->_$path);";
          }
        }
        print admstpathYacc_y &ALIASADMST("d$idx") if not $islast;
        print admstpathYacc_y &ALLLASTPOS("d$idx") if $islast;
        print admstpathYacc_y "d$idx->_valueto=(void*)adms_${prevpath}_valueto_$path;\n";
        $idx++;
      }
      print admstpathYacc_y "  }\n";
    }
  }
  if(/MKlist/)
  {
    if($e eq "list" && $a eq "item")
    {
      print admstpathYacc_y "  else if(e==admse_$e)\n  {\n    p_admst d=adms_admst_newln(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;".&ALLLASTPOS("d")."\n  }\n";
    }
    else
    {
      print admstpathYacc_y "  else if(e==admse_$e)\n  {\n    p_admst d=adms_admst_newla(ai,ai,(p_slist)(((p_$e)ai->_item.p)->_$a)); d->_valueto=(void*)adms_${e}_valueto_$a;".&ALLLASTPOS("d")."\n  }\n";
    }
  }
  elsif(/MKscalar/)
  {
    print admstpathYacc_y "  else if(e==admse_$e)\n  {\n    ";
    if(/MKscalar\(attribute,adms\)/)
    {
      print admstpathYacc_y "p_admst d=adms_admst_newpc(ai,ai,(p_attribute)ai->_item.p);";
    }
    elsif($d eq "sub")
    {
      print admstpathYacc_y "p_admst d=adms_admst_newpp(ai,ai,admse_$a); d->_valueto=(void*)NULL;";
    }
    elsif($d eq "basicenumeration")
    {
      print admstpathYacc_y "p_admst d=adms_admst_newbe(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    elsif($d eq "basicinteger")
    {
      print admstpathYacc_y "p_admst d=adms_admst_newbi(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    elsif($d eq "basicreal")
    {
      print admstpathYacc_y "p_admst d=adms_admst_newbr(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    elsif($d eq "basicstring")
    {
      print admstpathYacc_y "p_admst d=adms_admst_newbs(ai,ai,((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    else
    {
      print admstpathYacc_y "p_admst d=adms_admst_newpa(ai,ai,(p_adms)((p_$e)ai->_item.p)->_$a); d->_valueto=(void*)adms_${e}_valueto_$a;";
    }
    print admstpathYacc_y &ALLLASTPOS("d")."\n  }\n";
  }
  if(/ENDIFIDENT/)
  {
    print admstpathYacc_y "  else\n";
    print admstpathYacc_y "  {\n";
    print admstpathYacc_y "    ".&LN("pn","ai")."\n";
    print admstpathYacc_y "    adms_message_error((\"%s: '$a' bad attribute\\n\",adms_transform_uid(p->_transform)))\n";
    print admstpathYacc_y "  }\n";
    print admstpathYacc_y "}\n";
  }
}
print admstpathYacc_y "\n";
my@Locationalias03;
foreach(@$EA)
{
  foreach($_->findnodes("alias"))
  {
    push @Locationalias03, &xname($_);
  }
  push @Locationalias03,xname(&eparent($_)) if &eparent($_);
  next if (not vla(xname($_))) && xname($_) ne "adms";
  foreach(&attr($_))
  {
    push @Locationalias03, &xname($_);
  }
}
sub uniq
{
  my@array=@_;
  my%seen;
  my@new;
  map{push @new, $_ if not $seen{$_};$seen{$_}=1;} @array;
  return @new;
}
@Locationalias03=&uniq(@Locationalias03);

print admstpathYacc_y "
static void groupage (void) {}
static void location04 (void) {}
static void location05 (void) {}
";
print admstpathYacc_y "
p_slist globalfileoutputlist=NULL;
int break_found=0;

FILE *OUT (void)
{
  return globalfileoutputlist?(FILE*)globalfileoutputlist->data:stdout;
}
p_transform lookfortemplates(p_transform mytransform,const char*match)
{
  p_slist li;
  p_transform parent=root()->_itransform?((p_itransform)root()->_itransform->data)->_originalcall:mytransform;
  while(parent)
  {
    p_slist l;
    for(l=parent->_templates;l;l=l->next)
      if(!strcmp(((p_admst)((p_transform)l->data)->_textmatch->_token->data)->_item.s,match))
        return (p_transform)l->data;
    parent=parent->_parent;
  }
  for(li=root()->_invtransform;li;li=li->next)
  {
    p_slist l;
    parent=(p_transform)li->data;
    for(l=parent->_templates;l;l=l->next)
      if(!strcmp(((p_admst)((p_transform)l->data)->_textmatch->_token->data)->_item.s,match))
        return (p_transform)l->data;
  }
  parent=mytransform;
  while(parent)
  {
    p_slist l;
    for(l=parent->_templates;l;l=l->next)
      if(!strcmp(((p_admst)((p_transform)l->data)->_textmatch->_token->data)->_item.s,match))
        return (p_transform)l->data;
    parent=parent->_parent;
  }
  return NULL;
}
static void location02 (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_itransform myitransform=adms_itransform_new(mypath->_template);
  p_ptraverse p0=bar(dot,(p_path)mypath->_u->data);
  p_slist myl0=p0->_admst;
  myitransform->_originalcall=mypath->_transform;
  adms_slist_push(&root()->_itransform,(p_adms)myitransform);
  for(;myl0;myl0=myl0->next)
  {
    if(myl0->data&&((p_admst)myl0->data)->_item.p)
    {
      p_slist l;
      xtraverse(myitransform->_template->_children,(p_admst)myl0->data,dot);
      for(l=myitransform->_variable;l;l=l->next)
      {
        p_slist li;
        for(li=((p_admstvariable)l->data)->_value;li;li=li->next)
          deref((p_admst)li->data);
        adms_admstvariable_free((p_admstvariable)l->data);
      }
    }
  }
  adms_slist_pull(&root()->_itransform);
  free_ptraverse(p0);
  for(myl0=myitransform->_return;myl0;myl0=myl0->next)
  {
    ".&ALLLASTPOS("((p_admst)myl0->data)")."
  }
  adms_itransform_free(myitransform);
}
void Xassert (p_transform mytransform,p_admst dot,p_admst dotdot)
{
  if(!mytransform->_textformat)
  {
    adms_message_fatal_continue((\"assert failed\\n\"))
    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mytransform)))
  }
  else if(mytransform->_pathselect)
  {
    p_ptraverse pselect=bar(dot,mytransform->_pathselect);
    p_slist myli;
    for(myli=pselect->_admst;myli;myli=myli->next)
    {
      char* text=tsprintf((p_admst)myli->data,mytransform->_textformat);
      adms_message_fatal_continue((\"%s\",text))
      free(text);
    }
    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mytransform)))
    free_ptraverse(pselect);
  }
  else
  {
    char* text=tsprintf(dot,mytransform->_textformat);
    adms_message_fatal_continue((\"%s\",text))
    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mytransform)))
    free(text);
  }
}
typedef void (*p_transform_evaluate) (p_transform mytransform,p_admst dot);
void xtraverse (p_slist mytransforms,p_admst dot,p_admst dotdot)
{
  p_slist l;
  for(l=mytransforms;l;l=l->next)
  {
    int mypassed=1;
    p_transform mytransform=(p_transform)l->data;
    if(mytransform->_pathtest)
    {
      p_ptraverse myptraverse=bar(dot,mytransform->_pathtest);
      if(mytransform->_callback==(void*)Xassert)
        mypassed=!myptraverse->_admst;
      else
      {
        p_admst myadmst0;
        mypassed=(myptraverse->_admst&&(myadmst0=aread((p_admst)myptraverse->_admst->data))&&(myadmst0->_pseudo!=admse_empty));
      }
      free_ptraverse(myptraverse);
    }
    if(mypassed)
      ((p_transform_evaluate)mytransform->_callback)(mytransform,dot);
    if(break_found==1)
      break;
  }
}

static void root00 (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_admst d=adms_admst_newpa(dot,dot,(p_adms)root());".&ALLLASTPOS("d")."
}
void location03 (void) {}
 /*[nepasimprimer]*/
static void adms_fatal_on_obsolete_syntax(p_transform mytransform,p_admst myadmst)
{
  if(myadmst&&
    ((myadmst->_pseudo==admse_datatypename)
     ||(myadmst->_pseudo==admse_empty)
     ||(myadmst->_pseudo==admse_basicenumeration)
     ||(myadmst->_pseudo==admse_basicinteger)
     ||(myadmst->_pseudo==admse_basicreal)
     ||(myadmst->_pseudo==admse_basicstring)))
    return;
  if(myadmst&&(myadmst->_pseudo==admse_expression))
  {
    adms_message_warning_continue((\"[removed feature] admst:value-of 'expression'\\n\"))
    adms_message_warning_continue((\"[removed feature] Please use admst:apply-templates\\n\"))
    adms_message_warning_continue((\"[removed feature] see: %s\\n\",adms_transform_uid(mytransform)))
    adms_message_warning((\"[removed feature] For help contact me at r29173\@users.sourceforge.net\\n\"))
  }
  else if(myadmst->_pseudo==admse__arguments)
  {
    adms_message_warning_continue((\"[removed feature] admst:value-of 'arguments'\\n\"))
    adms_message_warning_continue((\"[removed feature] Please use admst:apply-templates\\n\"))
    adms_message_warning_continue((\"[removed feature] see: %s\\n\",adms_transform_uid(mytransform)))
    adms_message_warning((\"[removed feature] For help contact me at r29173\@users.sourceforge.net\\n\"))
  }
}
p_admst adms_pull_admst (p_transform mytransform)
{
  if(root()->_valueof)
  {
    adms_slist_pull(&root()->_valueof); /*pull transform*/
    return (p_admst)adms_slist_pull(&root()->_valueof);
  }
  adms_message_fatal_continue((\"stack '%%s' has no more element!\\n\"))
  adms_message_fatal((\"see %s\",adms_transform_uid(mytransform)))
  return NULL;
}
p_admstvariable lookup_dollar (const char* myid)
{
  p_itransform myitransform=root()->_itransform?(p_itransform)root()->_itransform->data:NULL;
  p_slist list;
  if(myitransform)
  {
    list=myitransform->_variable;
    while(list)
    {
      if(!strcmp(((p_admstvariable)list->data)->_name,myid))
        return (p_admstvariable)list->data;
      list=list->next;
    }
  }
  list=root()->_variable;
  while(list)
  {
    if(!strcmp(((p_admstvariable)list->data)->_name,myid))
      return (p_admstvariable)list->data;
    list=list->next;
  }
  return NULL;
}
char* tsprintf (p_kadmst myadmst, p_ktext mytext)
{
  char* myvalue=myadmst?adms_kclone(\"\"):((p_admst)mytext->_token->data)->_item.s;
  p_slist myl;
  for(myl=((myadmst&&mytext)?mytext->_token:NULL);myl;myl=myl->next)
  {
    p_adms token=myl->data;
    if(token->_datatypename==admse_text)
    {
      if(((p_text)token)->_admse==admse__s)
      {
        p_admst mystack=adms_pull_admst(mytext->_transform);
        if(mystack)
        {
          char* mynewvalue=aprintf(mytext->_transform,mystack);
          if(mynewvalue)
          {
            adms_fatal_on_obsolete_syntax(mytext->_transform,mystack);
            adms_k2strconcat(&myvalue,mynewvalue);
            free(mynewvalue);
          }
          deref(mystack);
        }
      }
      else
      {
        char* dollarname=tsprintf(myadmst,(p_text)token);
        p_admstvariable dollar=lookup_dollar(dollarname);
        if(dollar)
        {
          p_slist myl2=dollar->_value;
          for(;myl2;myl2=myl2->next)
          {
            char* val=aprintf(mytext->_transform,(p_admst)myl2->data);
            if(val)
            {
              adms_k2strconcat(&myvalue,val);
              free(val);
            }
          }
        }
        else
        {
          adms_message_obsolete_continue((\"variable \$(%s) is undefined\\n\",dollarname))
          adms_message_obsolete((\"Check if you really wanted to access an variable. If not replace '\$' by '\$'\\n\"))
          adms_message_obsolete((\"see %s\\n\",adms_transform_uid(mytext->_transform)))
          adms_k2strconcat(&myvalue,\"\$\");
          adms_k2strconcat(&myvalue,dollarname);
        }
        free(dollarname);
      }
    }
    else if(token->_datatypename==admse_admst)
      adms_k2strconcat(&myvalue,((p_admst)token)->_item.s);
    else if(token->_datatypename==admse_path)
    {
      p_ptraverse myptraverse=bar(myadmst,(p_path)token);
      p_slist myl;
      for(myl=myptraverse->_admst;myl;myl=myl->next)
      {
        char* mynewvalue=aprintf(((p_path)token)->_transform,(p_admst)myl->data);
        if(mynewvalue)
          adms_k2strconcat(&myvalue,mynewvalue);
        free(mynewvalue);
      }
      free_ptraverse(myptraverse);
    }
  }
  return myvalue;
}
void tprintf (p_kadmst myadmst, p_ktext mytext)
{
  p_slist myl;
  for(myl=mytext->_token;myl;myl=myl->next)
  {
    p_adms token=myl->data;
    if(token->_datatypename==admse_text)
    {
      if(((p_text)token)->_admse==admse__s)
      {
        p_admst mystack=adms_pull_admst(mytext->_transform);
        if(mystack)
        {
          char* mynewvalue=aprintf(mytext->_transform,mystack);
          if(mynewvalue)
          {
            adms_fatal_on_obsolete_syntax(mytext->_transform,mystack);
            fputs(mynewvalue,OUT());
            free(mynewvalue);
          }
          deref(mystack);
        }
      }
      else
      {
        char* dollarname=tsprintf(myadmst,(p_text)token);
        p_admstvariable dollar=lookup_dollar(dollarname);
        if(dollar)
        {
          p_slist myl2=dollar->_value;
          for(;myl2;myl2=myl2->next)
          {
            char* val=aprintf(mytext->_transform,(p_admst)myl2->data);
            if(val)
            {
              fputs(val,OUT());
              free(val);
            }
          }
        }
        else
        {
          adms_message_obsolete_continue((\"variable \$(%s) is undefined\\n\",dollarname))
          adms_message_obsolete((\"Check if you really wanted to access an variable. If not replace '\$' by '\$'\\n\"))
          adms_message_obsolete((\"see %s\\n\",adms_transform_uid(mytext->_transform)))
          fputs(\"\$\",OUT());
          fputs(dollarname,OUT());
        }
        free(dollarname);
      }
    }
    else if(token->_datatypename==admse_admst)
      fputs(((p_admst)token)->_item.s,OUT());
    else if(token->_datatypename==admse_path)
    {
      p_ptraverse myptraverse=bar(myadmst,(p_path)token);
      p_slist myl;
      for(myl=myptraverse->_admst;myl;myl=myl->next)
      {
        char* mynewvalue=aprintf(((p_path)token)->_transform,(p_admst)myl->data);
        if(mynewvalue)
          fputs(mynewvalue,OUT());
        free(mynewvalue);
      }
      free_ptraverse(myptraverse);
    }
  }
}
static void location26 (p_ptraverse p,p_path mypath,p_admst dot)
{
  tprintf(dot,mypath->_text);
}
static int retdatatypename(p_admst myadmst)
{
  if(
      (myadmst->_pseudo==admse_empty)
      ||
      (myadmst->_pseudo==admse_basicenumeration)
      ||
      (myadmst->_pseudo==admse_basicinteger)
      ||
      (myadmst->_pseudo==admse_basicreal)
      ||
      (myadmst->_pseudo==admse_basicstring)
      ||
      (myadmst->_pseudo==admse__ladms)
      ||
      (myadmst->_pseudo==admse__ladmst)
      ||
      (myadmst->_pseudo==admse_datatypename)
    )
    return myadmst->_pseudo;
  else if(myadmst->_pseudo==admse__croix)
    return ((p_attribute)myadmst->_item.p)->_value?((p_admst)((p_attribute)myadmst->_item.p)->_value)->_pseudo:admse_empty;
  else
    return myadmst->_item.p->_datatypename;
}
static p_attribute retcroixcroix (p_admst myadmst,p_path mypath)
{
  char*myname=tsprintf(myadmst,mypath->_text);
  p_attribute myattribute=adms_attribute_new(myname);
  free(myname);
  return myattribute;
}
static p_attribute retcroix (p_admst myadmst,p_path mypath)
{
  p_attribute myattribute=NULL;
  char*myname=tsprintf(myadmst,mypath->_text);
  p_slist list;
  p_adms item=myadmst->_item.p;
  if(
      (myadmst->_pseudo==admse_basicenumeration)
      ||
      (myadmst->_pseudo==admse_basicinteger)
      ||
      (myadmst->_pseudo==admse_basicreal)
      ||
      (myadmst->_pseudo==admse_basicstring)
      ||
      (myadmst->_pseudo==admse_datatypename)
    )
  {
    adms_message_fatal_continue((\"operator '#' applied to element '%s' - not supported!\\n\",ns_etostr(myadmst->_pseudo)))
    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mypath->_transform)))
  }
  list=item?item->_attribute:NULL;
  for(;list&&!myattribute;list=list->next)
    if(!strcmp(((p_attribute)list->data)->_name,myname))
      myattribute=(p_attribute)list->data;
  if(!myattribute)
  {
    myattribute=adms_attribute_new(myname);
    adms_slist_push(&item->_attribute,(p_adms)myattribute);
  }
  free(myname);
  return myattribute;
}
static p_slist retcroixstar (p_admst myadmst)
{
  return myadmst->_item.p?myadmst->_item.p->_attribute:NULL;
}
static int adms_slist_compare (p_admst myadmst0,p_admst myadmst1)
{
  admse mytype0=myadmst0->_pseudo;
  admse mytype1=myadmst1->_pseudo;
  if((mytype0==admse_empty)&&(mytype1==admse_empty))
    return 0;
  else if(mytype0==admse_empty)
    return -1;
  else if(mytype1==admse_empty)
    return +1;
  else if((mytype0==admse_basicinteger)&&(mytype1==admse_basicinteger))
    return (myadmst0->_item.i-myadmst1->_item.i);
  else
  {
    char* uid1=aprintf(NULL,myadmst0);
    char* uid2=aprintf(NULL,myadmst1);
    int res=strcmp(uid1,uid2);
    free(uid1);
    free(uid2);
    return res;
  }
}
static p_slist adms_slist_sort_merge (p_slist myl0, p_slist myl1)
{
  t_slist list;
  p_slist myli=&list;
  while(myl0&&myl1)
  {
    if(adms_slist_compare((p_admst)myl0->data,(p_admst)myl1->data)<0)
    {
      myli=myli->next=myl0;
      myl0=myl0->next;
    }
    else
    {
      myli=myli->next=myl1;
      myl1=myl1->next;
    }
  }
  myli->next=myl0?myl0:myl1;
  return list.next;
}
static p_slist adms_slist_sort (p_slist list)
{
  p_slist myl0, myl1;
  if(!list)
    return NULL;
  if(!list->next)
    return list;
  myl0=list;
  myl1=list->next;
  while((myl1=myl1->next)!=NULL)
  {
    if((myl1=myl1->next)==NULL)
      break;
    myl0=myl0->next;
  }
  myl1=myl0->next;
  myl0->next=NULL;
  return adms_slist_sort_merge(adms_slist_sort(list),adms_slist_sort(myl1));
}
";
sub L0
{
  my($desc,$id,$ddata)=(shift,shift,shift);
  return "/*$desc*/
static void location$id (p_ptraverse p,p_path mypath,p_admst dot)
{
  $ddata
}\n";
}
#print admstpathYacc_y &L0("returned",15,"));
print admstpathYacc_y "/*returned*/\nstatic void location15 (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist l;
  p_admst d=NULL;
  char* myname=tsprintf(dot,mypath->_text);
  p_itransform myitransform;
  if(!root()->_itransforminsideapplytemplate)
  {
    adms_message_fatal_continue((\"return('%s') allowed only inside admst:apply-templates\\n\",myname))
    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mypath->_transform)))
  }
  myitransform=(p_itransform)root()->_itransforminsideapplytemplate->data;
  for(l=myitransform->_return;l&&!d;l=l->next)
    if(!strcmp(((p_return)((p_admst)l->data)->_item.p)->_name,myname))
      d=(p_admst)l->data;
  if(d==NULL)
  {
    adms_message_fatal_continue((\"returned('%s'): undefined returned value\\n\",myname))
    adms_message_fatal((\"see %s\\n\",adms_transform_uid(mypath->_transform)))
  }
  free(myname);
  if(d->_refd) d->_refd++;".&ALLLASTPOS("d")."
}
";
print admstpathYacc_y "/*\"\%p\"*/\nstatic void location200 (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_admst d=adms_pull_admst(mypath->_transform);".&ALLLASTPOS("d")."
}
";
print admstpathYacc_y "/*'k'*/\nstatic void location09k (p_ptraverse p,p_path mypath,p_admst dot)
{
  ((p_admst)mypath->_text->_token->data)->_previouspath=dot;
  ".&ALLLASTPOS("((p_admst)mypath->_text->_token->data)")."
}
";
print admstpathYacc_y "/*int*/\nstatic void location10 (p_ptraverse p,p_path mypath,p_admst dot)
{
  ((p_admst)mypath->_text->_token->data)->_previouspath=dot;
  ".&ALLLASTPOS("((p_admst)mypath->_text->_token->data)")."
}
";
sub L1
{
  my($desc,$id,$type,$rhs)=@_;
  return "/*$desc*/
static void location$id (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_admst d=adms_admst_new$type(dot,dot,$rhs);".&ALLLASTPOS("d")."
}\n";
}
print admstpathYacc_y "
static p_admstvariable retaroba (p_admst myadmst,p_path mypath)
{
  char* myid=tsprintf(myadmst,mypath->_text);
  p_admstvariable mydollar;
  p_slist list;
  p_adms item=myadmst->_item.p;
  list=item->_variable;
  while(list)
  {
    if(!strcmp(((p_admstvariable)list->data)->_name,myid))
    {
      free(myid);
      return (p_admstvariable)list->data;
    }
    list=list->next;
  }
  mydollar=adms_admstvariable_new(myid);
  adms_slist_push(&item->_variable,(p_adms)mydollar);
  free(myid);
  return mydollar;
}
static p_admstvariable retdollar (p_admst myadmst,p_path mypath)
{
  char* id=tsprintf(aread(myadmst),mypath->_text);
  p_admstvariable dollar=lookup_dollar(id);
  if(!dollar)
    adms_message_fatal((\"variable \$(%s) is undefined! see %s\\n\",id,adms_transform_uid(mypath->_transform)))
  free(id);
  return dollar;
}
static int retcount (p_admst myadmst,p_path mypath)
{
  p_ptraverse p0=bar(myadmst,(p_path)mypath->_u->data);
  int count=p0->_admst?((p_admst)p0->_admst->data)->_count:0;
  free_ptraverse(p0);
  return count;
}\n";
print admstpathYacc_y &L1("datatypename","03datatypename","bd","retdatatypename(dot)");
print admstpathYacc_y &L1("ident","03dummy","bi","0");
print admstpathYacc_y &L1("real","07","br","adms_strtod(mypath->_transform,tsprintf(NULL,mypath->_text))");
print admstpathYacc_y &L1("position",18,"bi","(dot)->_position");
print admstpathYacc_y &L1("count",27,"bi","retcount(dot,mypath)");
print admstpathYacc_y &L1("''","09","ns","tsprintf(dot,mypath->_text)");
print admstpathYacc_y &L1("##",12,"pa","(p_adms)retcroixcroix(dot,mypath)");
print admstpathYacc_y &L1("#",13,"pc","retcroix(dot,mypath)");
print admstpathYacc_y &L1("admst",14,"pa","(p_adms)dot");
print admstpathYacc_y &L1("\$var","041","pd","retdollar(dot,mypath)");
print admstpathYacc_y &L1("\@var","051","pd","retaroba(dot,mypath)");
print admstpathYacc_y "/*index*/
static int retindex (p_slist mylii,p_ptraverse p0)
{
  p_admst myadmst1=(p_admst)mylii->data;
  p_adms myadms1=myadmst1->_item.p;
  admse mytype1=myadmst1->_pseudo;
  p_cmp mycmp=adms_admsxml_cmp(myadmst1);
  p_slist myl0;
  int myindex=-1;
  int found=0;
  for(myl0=p0->_admst;myl0&&!found;myl0=myl0->next)
  {
    p_admst myadmst0=(p_admst)myl0->data;
    p_adms myadms0=myadmst0->_item.p;
    admse mytype0=myadmst0->_pseudo;
    found=(mytype0==mytype1)&&(!mycmp(myadms0,myadms1));
    myindex++;
  }
  if(found)
    return myindex;
  else
    return minusone;
}
static void location19 (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_ptraverse p0=bar(dot,(p_path)mypath->_u->data);
  p_ptraverse p1=bar(dot,(p_path)mypath->_u->next->data);
  p_slist l1=p1->_admst;
  for(;l1;l1=l1->next)
  {
    p_admst d=adms_admst_newbi(dot,dot,retindex(l1,p0));".&ALLLASTPOS("d")."
  }
  free_ptraverse(p0); free_ptraverse(p1);
}\n";
sub L2
{
  my($desc,$id,$type,$rhs)=@_;
  return "/*$desc*/
static void location$id (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_ptraverse p0=bar(dot,(p_path)mypath->_u->data);
  p_slist l0=p0->_admst;
  for(;l0;l0=l0->next)
  {
    p_admst d=adms_admst_new$type(dot,dot,$rhs);".&ALLLASTPOS("d")."
  }
  free_ptraverse(p0);
}\n";
}
print admstpathYacc_y "
static char* retaprintf (p_path mypath,p_slist myl0)
{
  return aprintf(mypath->_transform,(p_admst)myl0->data);
}
static char* retpointer (p_slist myl0)
{
  char* mystring=malloc(sizeof(char)*50);
  sprintf(mystring,\"%p\",((p_admst)myl0->data)->_item.p);
  return mystring;
}
static char* retuc (p_path mypath,p_slist myl0)
{
  char* mystring=aprintf(mypath->_transform,(p_admst)myl0->data);
  adms_toupper(mystring);
  return mystring;
}
static char* retlc (p_path mypath,p_slist myl0)
{
  char* mystring=aprintf(mypath->_transform,(p_admst)myl0->data);
  adms_tolower(mystring);
  return mystring;
}\n";
print admstpathYacc_y &L2("uid",20,"ns","retaprintf(mypath,l0)");
print admstpathYacc_y &L2("id ",21,"ns","retpointer(l0)");
print admstpathYacc_y &L2("uc",24,"ns","retuc(mypath,l0)");
print admstpathYacc_y &L2("lc",25,"ns","retlc(mypath,l0)");
sub L3
{
  my($desc,$id,$func,$type,$rhs)=@_;
  return "/*$desc*/
static void location$id (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_ptraverse p0=bar(dot,(p_path)mypath->_u->data);
  p_slist lii=$func(p0->_admst);
  p0->_admst=lii;
  for(;lii;lii=lii->next)
  {
    p_admst d=adms_admst_new$type(dot,dot,$rhs);".&ALLLASTPOS("d")."
  }
  free_ptraverse(p0);
}\n";
}
print admstpathYacc_y &L3("sort",22,"adms_slist_sort","pn","(p_admst)lii->data");
print admstpathYacc_y &L3("reverse",23,"adms_slist_reverse","pn","(p_admst)lii->data");
sub L4
{
  my($desc,$id,$dii,$type,$rhs)=@_;
  return "/*$desc*/
static void location$id (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist lii=$dii;
  for(;lii;lii=lii->next)
  {
    p_admst d=adms_admst_new$type(dot,dot,$rhs);".&ALLLASTPOS("d")."
  }
}\n";
}
print admstpathYacc_y &L4("#*",130,"retcroixstar(dot)?retcroixstar(dot):NULL","pc","(p_attribute)lii->data");
print admstpathYacc_y &L4("\$var","040","retdollar(dot,mypath)->_value","pn","(p_admst)lii->data");
print admstpathYacc_y &L4("\@var","050","retaroba(dot,mypath)->_value","pn","(p_admst)lii->data");

print admstpathYacc_y "
static void attributecallback (p_path mypath0,p_slist npaths,p_slist npath)
{
  p_path mypath=(p_path)npath->data;
  mypath->_keeplist=(mypath0->_keeplist&&(npath->next==NULL)&&(mypath->_c==NULL))||mypath->_keeplist;
  if(mypath->_callback==(void*)location04)
  {
    mypath->_callback=(mypath->_keeplist)?(void*)location041:(void*)location040;
    mypath->_callbackname=(mypath->_keeplist)?\"location041\":\"location040\";
  }
  else if(mypath->_callback==(void*)location05)
  {
    mypath->_callback=(mypath->_keeplist)?(void*)location051:(void*)location050;
    mypath->_callbackname=(mypath->_keeplist)?\"location051\":\"location050\";
  }
  else if(mypath->_callback==(void*)location03)
  {
    if(0) {}\n";
foreach (@Locationalias03)
{
  print admstpathYacc_y "    else if(mypath->_text->_admse==admse_$_)\n";
  print admstpathYacc_y "    {\n";
  if($_ eq "datatypename")
  {
    print admstpathYacc_y "      mypath->_callback=(void*)location03datatypename;\n";
    print admstpathYacc_y "      mypath->_callbackname=\"location03datatypename.$_\";\n";
  }
  else
  {
    print admstpathYacc_y "      mypath->_callback=(mypath->_keeplist)?(void*)location031$_:(void*)location030$_;\n";
    print admstpathYacc_y "      mypath->_callbackname=(mypath->_keeplist)?\"location031$_\":\"location030$_\";\n";
  }
  print admstpathYacc_y "    }\n";
}
print admstpathYacc_y '    else
    {
      adms_message_fatal_continue(("element or attribute \'%s\' does not exist\\n",mypath->_text->_value))
      adms_message_fatal(("see %s\\n",adms_transform_uid(mypath->_transform)))
    }
  }
}
static void postpath (p_path mypath)
{
  p_slist l;
  if(mypath->_callback==(void*)location02)
  {
    const char* match=((p_admst)mypath->_text->_token->data)->_item.s;
    if(!(mypath->_template=lookfortemplates(mypath->_transform,match)))
    {
      adms_message_fatal_continue(("Template not found: %s\\n",match))
      adms_message_fatal(("  see:  %s\\n",adms_transform_uid(mypath->_transform)))
    }
  }
  for(l=mypath->_bar;l;l=l->next)
  {
    p_slist paths=(p_slist)l->data;
    for(;paths;paths=paths->next)
    {
      postpath((p_path)paths->data);
      attributecallback(mypath,l,paths);
    }
  }
  for(l=mypath->_arg;l;l=l->next)
  {
    p_slist paths=(p_slist)l->data;
    for(;paths;paths=paths->next)
    {
      postpath((p_path)paths->data);
      attributecallback(mypath,l,paths);
    }
  }
  for(l=mypath->_u;l;l=l->next)
    postpath((p_path)l->data);
  for(l=mypath->_b;l;l=l->next)
    postpath((p_path)l->data);
  for(l=mypath->_c;l;l=l->next)
    postpath((p_path)l->data);
  if(mypath->_text)
    posttext(mypath->_text);
}
';
print admstpathYacc_y "
static void location00 (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_admst d=adms_admst_newpn(dot->_previous,dot->_previouspath,dot);
  d->_position=dot->_position;
  d->_count=dot->_count;
  ".&fixme("d").";
}
static void location01 (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_admst d=adms_admst_newpn(dot->_previouspath->_previous,dot->_previouspath->_previouspath,dot->_previouspath);
  d->_position=dot->_previouspath->_position;
  d->_count=dot->_previouspath->_count;
  ".&fixme("d").";
}
typedef p_slist (*_p_cblocation) (p_ptraverse p,p_path mypath,p_admst dot);
typedef int (*_p_cbcondition) (const p_path mypath,p_admst myadmst);
";
print admstpathYacc_y "
void free_ptraverse (p_ptraverse p)
{
  p_slist l;
  p_slist ll;
  for(l=p->_alladmst;l;l=l->next)
    for(ll=(p_slist)l->data;ll;ll=ll->next)
      deref((p_admst)ll->data);
  for(l=p->_aliasadmst;l;l=l->next)
    deref((p_admst)l->data);
  for(l=p->_admst;l;l=l->next)
    deref((p_admst)l->data);
  adms_slist_free(p->_alladmst);
  adms_slist_free(p->_aliasadmst);
  adms_slist_free(p->_admst);
  free(p);
}
static p_slist slash (p_ptraverse p,p_slist paths,p_admst dot)
{
  p_slist output=NULL;
  int premier=1;
  adms_slist_push(&output,(p_adms)dot);
  for(;paths;paths=paths->next)
  {
    p_slist l;
    p_path mypath=(p_path)paths->data;
    p_slist output1=NULL;
    int dernier=(paths->next==NULL);
    if(mypath->_callback==(void*)groupage)
    {
      int count=0;
      for(l=output;l;l=l->next)
        if((((p_admst)l->data)->_selected==admse_yes)&&((p_admst)l->data)->_pseudo!=admse_empty)
        {
          p_ptraverse p0=bar((p_admst)l->data,mypath);
          adms_slist_concat(&output1,p0->_admst);
          adms_slist_free(p0->_alladmst);
          free(p0);
        }
      for(l=output1;l;l=l->next)
        ((p_admst)l->data)->_position=++count;
      p->_position=count;
    }
    else
    {
      p_admst ai;
      p->_position=0;
      p->_a1=p->_an=NULL;
      for(l=output;l;l=l->next)
        if((((p_admst)l->data)->_selected==admse_yes)&&((p_admst)l->data)->_pseudo!=admse_empty)
          ((_p_cblocation)mypath->_callback)(p,mypath,(p_admst)l->data);
      for(ai=p->_an;ai;ai=ai->_preva)
        adms_slist_push(&output1,(p_adms)ai);
    }
    if(premier)
      adms_slist_free(output),premier=0;
    if(!dernier)
      adms_slist_push(&p->_alladmst,(p_adms)output1);
    if((mypath->_callback!=(void*)location00)&&(mypath->_callback!=(void*)location01))
      for(l=output1;l;l=l->next)
        ((p_admst)l->data)->_count=p->_position;
    if(mypath->_c)
    {
      int count=0;
      for(l=output1;l;l=l->next)
        if(((_p_cbcondition)((p_path)mypath->_c->data)->_callback)((p_path)mypath->_c->data,(p_admst)l->data))
        {
          count++;
          if((mypath->_callback!=(void*)location00)&&(mypath->_callback!=(void*)location01))
            ((p_admst)l->data)->_position=count;
        }
        else
          ((p_admst)l->data)->_selected=admse_no;
      if((mypath->_callback!=(void*)location00)&&(mypath->_callback!=(void*)location01))
        for(l=output1;l;l=l->next)
          ((p_admst)l->data)->_count=count;
      if(dernier)
      {
        p_slist l=output1;
        p_slist n;
        p_slist lastnode=NULL;
        output1=NULL;
        for(;l;l=n)
          if(((p_admst)l->data)->_selected==admse_yes)
          {
            if(lastnode)
              lastnode->next=l,lastnode=l;
            else
              output1=lastnode=l;
            n=l->next;
          }
          else
            n=l->next,deref((p_admst)l->data),free(l);
        if(lastnode)
          lastnode->next=NULL;
      }
    }
    output=output1;
  }
  return output;
}
p_ptraverse bar (p_admst dot,p_path mypath)
{
  p_ptraverse p=adms_ptraverse_new(mypath->_transform);
  p_slist bars;
  p_slist output=NULL;
  for(bars=mypath->_bar;bars;bars=bars->next)
    adms_slist_concat(&output,slash(p,(p_slist)bars->data,dot));
  p->_admst=output;
  return p;
}
static void addptraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_slist paths1=(p_slist)mypath->_arg->next->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  p_ptraverse p1=adms_ptraverse_new(mypath->_transform);
  p_slist o1=slash(p1,paths1,dot);
  for(;o0&&o1;o0=o0->next,o1=o1->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    p_admst myadmst1=(p_admst)o1->data;
    admse mytype0=myadmst0->_pseudo;
    admse mytype1=myadmst1->_pseudo;
    if((mytype0==admse_basicinteger)&&(mytype1==admse_basicinteger))
    {
      ".&LLNN("bi","myadmst0->_item.i+myadmst1->_item.i")."
    }
    else if((mytype0==admse_basicreal)&&(mytype1==admse_basicreal))
    {
      ".&LLNN("bi","myadmst0->_item.r+myadmst1->_item.r")."
    }
    else
    {
      ".&LLNN("bi","adms_NAN")."
      adms_message_warning((\"%s: '+' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
    }
  }
  for(o0=o1?o1:o0;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basicinteger)
    {
      ".&LLNN("bi","myadmst0->_item.i")."
    }
    else if(mytype0==admse_basicreal)
    {
      ".&LLNN("br","myadmst0->_item.r")."
    }
    else
    {
      ".&LLNN("bi","adms_NAN")."
      adms_message_warning((\"%s: '+' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
    }
  }
}
static void addmtraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_slist paths1=(p_slist)mypath->_arg->next->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  p_ptraverse p1=adms_ptraverse_new(mypath->_transform);
  p_slist o1=slash(p1,paths1,dot);
  for(;o0&&o1;o0=o0->next,o1=o1->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    p_admst myadmst1=(p_admst)o1->data;
    admse mytype0=myadmst0->_pseudo;
    admse mytype1=myadmst1->_pseudo;
    if((mytype0==admse_basicinteger)&&(mytype1==admse_basicinteger))
    {
      ".&LLNN("bi","myadmst0->_item.i-myadmst1->_item.i")."
    }
    else if((mytype0==admse_basicreal)&&(mytype1==admse_basicreal))
    {
      ".&LLNN("bi","myadmst0->_item.r-myadmst1->_item.r")."
    }
    else
    {
      ".&LLNN("bi","adms_NAN")."
      adms_message_warning((\"%s: '-' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
    }
  }
  for(;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basicinteger)
    {
      ".&LLNN("bi","myadmst0->_item.i")."
    }
    else if(mytype0==admse_basicreal)
    {
      ".&LLNN("br","myadmst0->_item.r")."
    }
    else
    {
      ".&LLNN("bi","adms_NAN")."
      adms_message_warning((\"%s: '-' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
    }
  }
  for(;o1;o1=o1->next)
  {
    p_admst myadmst1=(p_admst)o1->data;
    admse mytype1=myadmst1->_pseudo;
    if(mytype1==admse_basicinteger)
    {
      ".&LLNN("bi","-myadmst1->_item.i")."
    }
    else if(mytype1==admse_basicreal)
    {
      ".&LLNN("br","-myadmst1->_item.r")."
    }
    else
    {
      ".&LLNN("bi","adms_NAN")."
      adms_message_warning((\"%s: '-' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
    }
  }
}
static void multtimetraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_slist paths1=(p_slist)mypath->_arg->next->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  p_ptraverse p1=adms_ptraverse_new(mypath->_transform);
  p_slist o1=slash(p1,paths1,dot);
  for(;o0&&o1;o0=o0->next,o1=o1->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    p_admst myadmst1=(p_admst)o1->data;
    admse mytype0=myadmst0->_pseudo;
    admse mytype1=myadmst1->_pseudo;
    if((mytype0==admse_basicinteger)&&(mytype1==admse_basicinteger))
    {
      ".&LLNN("bi","myadmst0->_item.i*myadmst1->_item.i")."
    }
    else if((mytype0==admse_basicreal)&&(mytype1==admse_basicreal))
    {
      ".&LLNN("bi","myadmst0->_item.r*myadmst1->_item.r")."
    }
    else
      adms_message_fatal((\"%s: '*' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
  }
  for(o0=o1?o1:o0;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basicinteger)
    {
      ".&LLNN("bi","myadmst0->_item.i")."
    }
    else
      adms_message_fatal((\"%s: '*' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
  }
}
static void multdivtraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_slist paths1=(p_slist)mypath->_arg->next->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  p_ptraverse p1=adms_ptraverse_new(mypath->_transform);
  p_slist o1=slash(p1,paths1,dot);
  for(;o0&&o1;o0=o0->next,o1=o1->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    p_admst myadmst1=(p_admst)o1->data;
    admse mytype0=myadmst0->_pseudo;
    admse mytype1=myadmst1->_pseudo;
    if((mytype0==admse_basicinteger)&&(mytype1==admse_basicinteger))
    {
      ".&LLNN("br","(double)myadmst0->_item.i/(double)myadmst1->_item.i")."
    }
    else if((mytype0==admse_basicreal)&&(mytype1==admse_basicreal))
    {
      ".&LLNN("bi","myadmst0->_item.r/myadmst1->_item.r")."
    }
    else
      adms_message_fatal((\"%s: ':' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
  }
  for(;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basicinteger)
    {
      ".&LLNN("br","(double)myadmst0->_item.i/1.0")."
    }
    else if(mytype0==admse_basicreal)
    {
      ".&LLNN("br","myadmst0->_item.r/1.0")."
    }
    else
      adms_message_fatal((\"%s: ':' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
  }
  for(;o1;o1=o1->next)
  {
    p_admst myadmst1=(p_admst)o1->data;
    admse mytype1=myadmst1->_pseudo;
    if(mytype1==admse_basicinteger)
    {
      ".&LLNN("br","1.0/(double)myadmst1->_item.i")."
    }
    else if(mytype1==admse_basicreal)
    {
      ".&LLNN("br","1.0/myadmst1->_item.r")."
    }
    else
      adms_message_fatal((\"%s: ':' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
  }
}
static void unaryptraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  for(;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basicinteger)
    {
      ".&LLNN("bi","+myadmst0->_item.i")."
    }
    else if(mytype0==admse_basicreal)
    {
      ".&LLNN("br","+myadmst0->_item.r")."
    }
    else
      adms_message_fatal((\"%s: '+' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
  }
}
static void unarymtraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  for(;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basicinteger)
    {
      ".&LLNN("bi","-myadmst0->_item.i")."
    }
    else if(mytype0==admse_basicreal)
    {
      ".&LLNN("br","-myadmst0->_item.r")."
    }
    else
      adms_message_fatal((\"%s: '-' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
  }
}
static void incrptraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  for(;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basiclist)
    {
      p_slist dollarlist=((p_admstvariable)myadmst0->_item.p)->_value;
      for(;dollarlist;dollarlist=dollarlist->next)
      {
        p_admst myadmst1=(p_admst)dollarlist->data;
        admse mytype1=myadmst1->_pseudo;
        if(mytype1==admse_basicinteger)
        {
          ".&LLNN("bi","myadmst1->_item.i")."
          myadmst1->_item.i+=1;
        }
        else if(mytype1==admse_basicreal)
        {
          ".&LLNN("br","myadmst1->_item.r")."
          myadmst1->_item.r+=1.0;
        }
        else
          adms_message_fatal((\"%s: '++' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
      }
    }
  }
}
static void incrmtraverse (p_ptraverse p,p_path mypath,p_admst dot)
{
  p_slist paths0=(p_slist)mypath->_arg->data;
  p_ptraverse p0=adms_ptraverse_new(mypath->_transform);
  p_slist o0=slash(p0,paths0,dot);
  for(;o0;o0=o0->next)
  {
    p_admst myadmst0=(p_admst)o0->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basiclist)
    {
      p_slist dollarlist=((p_admstvariable)myadmst0->_item.p)->_value;
      for(;dollarlist;dollarlist=dollarlist->next)
      {
        p_admst myadmst1=(p_admst)dollarlist->data;
        admse mytype1=myadmst1->_pseudo;
        if(mytype1==admse_basicinteger)
        {
          ".&LLNN("bi","myadmst1->_item.i")."
          myadmst1->_item.i-=1;
        }
        else if(mytype1==admse_basicreal)
        {
          ".&LLNN("br","myadmst1->_item.r")."
          myadmst1->_item.r-=1.0;
        }
        else
          adms_message_fatal((\"%s: '--' operator - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
      }
    }
  }
}
static int expression01 (const p_path mypath,p_admst dot)
{
  p_ptraverse p0=bar(dot,(p_path)mypath->_u->data);
  int pos=dot->_position;
  p_slist l;
  for(l=p0->_admst;l;l=l->next)
  {
    p_admst myadmst0=(p_admst)l->data;
    admse mytype0=myadmst0->_pseudo;
    if(mytype0==admse_basicinteger)
    {
      if(pos==myadmst0->_item.i)
      {
        free_ptraverse(p0);
        return 1;
      }
    }
    else
      adms_message_fatal((\"%s: 'integer' expected!\\n\",adms_transform_uid(mypath->_transform)))
  }
  free_ptraverse(p0);
  return 0;
}

#define admstpatherror(mypparse,msgid) admstpatherrorimpl(mypparse,msgid)

#define ISPATH     ((int)(long)globalctxt->data==0)
#define ISTEXT     ((int)(long)globalctxt->data==1)
#define ISTIC      ((int)(long)globalctxt->data==2)
#define SETPATH    adms_slist_push(&globalctxt,(p_adms)0);
#define SETTEXT    adms_slist_push(&globalctxt,(p_adms)1);
#define SETTIC     adms_slist_push(&globalctxt,(p_adms)2);
#define PARENMATCH (parenidx&&paren==(int)(long)parenidx->data)
#define INSIDEPATH (globalctxt&&(globalctxt->next)&&(((int)(long)globalctxt->next->data)==0))

static p_slist globalctxt=NULL;
static int goto_atext=0;
static int paren=0;
static p_slist parenidx=NULL;
static int globalignorequote=0;

static p_path BASICSTRING (const p_pparse mypparse,char *mytvalue)
{
  p_path ret=adms_path_new(mypparse->_transform,adms_text_new(mypparse->_transform,mytvalue));
  adms_slist_push(&(ret->_text->_token),(p_adms)adms_admst_newks(mytvalue));
  ret->_text->_admse=ns_strtoe(((p_admst)ret->_text->_token->data)->_item.s);
  ret->_aname=adms_kclone(mytvalue);
  return ret;
}
static p_path BASICINTEGER (const p_pparse mypparse,char *mytvalue)
{
  p_path ret=adms_path_new(mypparse->_transform,adms_text_new(mypparse->_transform,mytvalue));
  adms_slist_push(&(ret->_text->_token),(p_adms)adms_admst_newbi(NULL,NULL,adms_strtol(mypparse->_transform,mytvalue)));
  ((p_admst)ret->_text->_token->data)->_refd=0;
  ret->_text->_admse=admse__any;
  ret->_aname=adms_kclone(mytvalue);
  return ret;
}

int is_admst (const char* name)
{
  return (((strlen(name)==5)&&!strncmp(name,\"admst\",5))||((strlen(name)>5)&&!strncmp(name,\"admst:\",6)));
}
void dbtext (p_text mytext)
{
  p_slist li;
  if(mytext->_aname)
    adms_message_admstdbg_impl(\"<text n=\\\"%s\\\" v=\\\"\%s\\\" e=\\\"\%s\\\">\",mytext->_aname,mytext->_value,ns_etostr(mytext->_admse));
  else
    adms_message_admstdbg_impl(\"<text v=\\\"\%s\\\" e=\\\"\%s\\\">\",mytext->_value,ns_etostr(mytext->_admse));
  for(li=mytext->_token;li;li=li->next)
  {
    p_adms mytoken=li->data;
    if(mytoken->_datatypename==admse_text && ((p_text)mytoken)->_admse==admse__s)
      adms_message_admstdbg_impl(\"<token t=\\\"special-%%s\\\" e=\\\"%s\\\"/>\\n\",ns_etostr(((p_text)mytoken)->_admse));
    else if(mytoken->_datatypename==admse_text)
      dbtext((p_text)mytoken);
    else if(mytoken->_datatypename==admse_admst)
    {
      adms_message_admstdbg_impl(\"<token t=\\\"admst\\\" pseudo=\\\"\%s\\\">\",ns_etostr(((p_admst)mytoken)->_pseudo));
      adms_message_admstdbg_impl(\"\%s\",aprintf(mytext->_transform,(p_admst)mytoken));
      adms_message_admstdbg_impl(\"</token>\\n\");
    }
    else if(mytoken->_datatypename==admse_path)
    {
      adms_message_admstdbg_impl(\"<token t=\\\"%%\\\">\");
      dbpath((p_path)mytoken);
      adms_message_admstdbg_impl(\"</token>\");
    }
  }
  if(mytext->_aname)
    adms_message_admstdbg_impl(\"</text>\\n\",mytext->_aname);
  else
    adms_message_admstdbg_impl(\"</text>\\n\");
}
void dbpath (p_path mypath)
{
  if(mypath->_text||mypath->_bar||mypath->_arg||mypath->_u||mypath->_b||mypath->_c)
  {
    adms_message_admstdbg_impl(\"<\%s keeplist=\\\"%i\\\">\",mypath->_callbackname,mypath->_keeplist);
    if(mypath->_bar)
    {
      p_slist bar;
      for(bar=mypath->_bar;bar;bar=bar->next)
      {
        p_slist l=(p_slist)bar->data;
        adms_message_admstdbg_impl(\"<bar>\");
        for(;l;l=l->next)
          dbpath((p_path)l->data);
        adms_message_admstdbg_impl(\"</bar>\");
      }
    }
    if(mypath->_arg)
    {
      p_slist arg;
      for(arg=mypath->_arg;arg;arg=arg->next)
      {
        p_slist l=(p_slist)arg->data;
        adms_message_admstdbg_impl(\"<arg>\");
        for(;l;l=l->next)
          dbpath((p_path)l->data);
        adms_message_admstdbg_impl(\"</arg>\");
      }
    }
    if(mypath->_u)
    {
      p_slist l;
      adms_message_admstdbg_impl(\"<u>\");
      for(l=mypath->_u;l;l=l->next)
        dbpath((p_path)l->data);
      adms_message_admstdbg_impl(\"</u>\");
    }
    if(mypath->_b)
    {
      p_slist l;
      adms_message_admstdbg_impl(\"<b>\");
      for(l=mypath->_b;l;l=l->next)
        dbpath((p_path)l->data);
      adms_message_admstdbg_impl(\"</b>\");
    }
    if(mypath->_c)
    {
      p_slist l;
      adms_message_admstdbg_impl(\"<c>\");
      for(l=mypath->_c;l;l=l->next)
        dbpath((p_path)l->data);
      adms_message_admstdbg_impl(\"</c>\");
    }
    if(mypath->_text)
      dbtext(mypath->_text);
    adms_message_admstdbg_impl(\"</\%s>\",mypath->_callbackname);
  }
  else
    adms_message_admstdbg_impl(\"<\%s/>\",mypath->_callbackname);
}

static int binaryand (const p_path mypath,p_admst myadmst)
{
  if(((_p_cbcondition)((p_path)mypath->_b->data)->_callback)((p_path)mypath->_b->data,myadmst))
    return ((_p_cbcondition)((p_path)mypath->_b->next->data)->_callback)((p_path)mypath->_b->next->data,myadmst);
  return 0;
}
static int binaryor (const p_path mypath,p_admst myadmst)
{
  if(!((_p_cbcondition)((p_path)mypath->_b->data)->_callback)((p_path)mypath->_b->data,myadmst))
    return ((_p_cbcondition)((p_path)mypath->_b->next->data)->_callback)((p_path)mypath->_b->next->data,myadmst);
  return 1;
}

static int atomicnilled (const p_path mypath,p_admst myadmst)
{
  p_admst myadmst0;
  p_ptraverse p0=bar(myadmst,(p_path)mypath->_u->data);
  int passed=(p0->_admst==NULL)||(p0->_admst->data==NULL)||((myadmst0=aread((p_admst)p0->_admst->data))==NULL)||(myadmst0->_item.p==NULL);
  free_ptraverse(p0);
  return passed;
}
static int atomicexists (const p_path mypath,p_admst myadmst)
{
  return !atomicnilled(mypath,myadmst);
}";
print admstpathYacc_y "
static int atomicisdefault (const p_path mypath,p_admst myadmst)
{
";
foreach(@$EA)
{
  my$e=&xname($_);
  foreach($_->findnodes("alias"))
  {
    my$alias=&xname($_);
    my$newe=&xelement($_);
    my$a=&xattribute($_);
    print admstpathYacc_y "  if(myadmst->_pseudo==admse_$e && mypath->_text->_admse==admse_$alias) return ((p_$newe)(myadmst->_item.p))->_${a}_isdefault;\n";
  }
}
#va elements + attributes
foreach(@$EA)
{
  my$e=xname($_);
  next if not vla($e);
  foreach(&attr($_))
  {
    my$a=xname($_);
    print admstpathYacc_y "  if(myadmst->_pseudo==admse_$e && mypath->_text->_admse==admse_$a) return ((p_$e)(myadmst->_item.p))->_${a}_isdefault;\n";
  }
}
print admstpathYacc_y "  return 0;
}
static int atomicgroupage (const p_path mypath,p_admst myadmst)
{
  return ((_p_cbcondition)((p_path)mypath->_u->data)->_callback)(((p_path)mypath->_u->data),myadmst);
}
static int unary00 (const p_path mypath,p_admst myadmst)
{
  return !((_p_cbcondition)((p_path)mypath->_u->data)->_callback)(((p_path)mypath->_u->data),myadmst);
}
static int atomictrue (const p_path mypath,p_admst myadmst)
{
  return 1;
}
static int atomicfalse (const p_path mypath,p_admst myadmst)
{
  return 0;
}
static int atomiceq (const p_path mypath,p_admst myadmst)
{
  p_ptraverse p0;
  p_ptraverse p1;
  p_slist myl0;
  p_slist myl1;
  int passed=1;
  p_path root0=(p_path)mypath->_u->data;
  p_path root1=(p_path)mypath->_u->next->data;
  p_slist n000=(p_slist)root0->_bar->data;
  p_slist n01=root0->_bar->next;
  p_slist n001=n000->next;
  p_path l000=(p_path)n000->data;
  p_slist n100=(p_slist)root1->_bar->data;
  p_slist n11=root1->_bar->next;
  p_slist n101=n100->next;
  p_path l100=(p_path)n100->data;
  int isdatatypename0=(l000->_callback==(void*)location03datatypename)&&(n001==NULL)&&(n01==NULL);
  int isquotedstring1=((l100->_callback==(void*)location09)||(l100->_callback==(void*)location09k))&&(n11==NULL)&&(n101==NULL)&&(n11==NULL);
  if(isdatatypename0 && isquotedstring1)
  {
    p_admst myadmst0=aread(myadmst);
    if(myadmst0)
      return (retdatatypename(myadmst0)==l100->_text->_admse);
    else
      return (admse_empty==l100->_text->_admse);
  }
  p0=bar(myadmst,(p_path)mypath->_u->data);
  p1=bar(myadmst,(p_path)mypath->_u->next->data);
  myl0=p0->_admst;
  myl1=p1->_admst;
  if(adms_slist_length(myl0)!=adms_slist_length(myl1))
    passed=0;
  for(;myl0&&passed;myl0=myl0->next,myl1=myl1->next)
  {
    p_admst myadmst0=aread((p_admst)myl0->data);
    p_admst myadmst1=aread((p_admst)myl1->data);
    if(!myadmst0)
      passed=0;
    else
    {
      admse admse0=myadmst0->_pseudo;
      admse admse1=myadmst1->_pseudo;
      if(admse0==admse_empty)
      {}
      else if(admse0==admse_datatypename)
      {
        char* mystring=aprintf(mypath->_transform,myadmst1);
        passed=(myadmst0->_item.i==ns_strtoe(mystring));
        free(mystring);
      }
      else if((admse0==admse_basicinteger)&&(admse1==admse_basicstring))
      {
        char *s=adms_integertostring(myadmst0->_item.i);
        passed=!strcmp(s,myadmst1->_item.s);
        free(s);
      }
      else if((admse0==admse_basicstring)&&(admse1==admse_basicinteger))
      {
        char *s=adms_integertostring(myadmst1->_item.i);
        passed=!strcmp(myadmst0->_item.s,s);
        free(s);
      }
      else if((admse0==admse_basicenumeration)&&(admse1==admse_basicstring))
        passed=!strcmp(ns_etostr(myadmst0->_item.i),myadmst1->_item.s);
      else if((admse0==admse_basicstring)&&(admse1==admse_basicenumeration))
        passed=!strcmp(myadmst0->_item.s,ns_etostr(myadmst1->_item.i));
      else if((admse0==admse_basicenumeration)&&(admse1==admse_basicinteger))
        passed=(myadmst0->_item.i==myadmst1->_item.i);
      else if((admse0==admse_basicinteger)&&(admse1==admse_basicenumeration))
        passed=(myadmst0->_item.i==myadmst1->_item.i);
      else if(admse0!=admse1)
        passed=0;
      else if(admse0==admse_basicenumeration)
        passed=(myadmst0->_item.i==myadmst1->_item.i);
      else if(admse0==admse_basicinteger)
        passed=(myadmst0->_item.i==myadmst1->_item.i);
      else if(admse0==admse_basicreal)
        passed=(myadmst0->_item.r>=myadmst1->_item.r-1.0e-307)&&(myadmst0->_item.r<=myadmst1->_item.r+1.0e-307);
      else if(admse0==admse_basicstring)
        passed=!strcmp(myadmst0->_item.s,myadmst1->_item.s);
      else
        passed=(myadmst0->_item.p==myadmst1->_item.p);
    }
  }
  free_ptraverse(p0);
  free_ptraverse(p1);
  return passed;
}
static int atomicne (const p_path mypath,p_admst myadmst)
{
  return !atomiceq(mypath,myadmst);
}
static int atomiclt (const p_path mypath,p_admst myadmst)
{
  p_ptraverse p0=bar(myadmst,(p_path)mypath->_u->data);
  p_ptraverse p1=bar(myadmst,(p_path)mypath->_u->next->data);
  p_slist myl0=p0->_admst;
  p_slist myl1=p1->_admst;
  int passed=1;
  if(adms_slist_length(myl0)!=adms_slist_length(myl1))
    passed=0;
  else
  {
    for(;myl0&&passed;myl0=myl0->next,myl1=myl1->next)
    {
      p_admst myadmst0=(p_admst)myl0->data;
      p_admst myadmst1=(p_admst)myl1->data;
      admse mytype0=myadmst0->_pseudo;
      admse mytype1=myadmst1->_pseudo;
      if((mytype0==admse_basicinteger)&&(mytype1==admse_basicinteger))
      {
        int myinteger0=myadmst0->_item.i;
        int myinteger1=myadmst1->_item.i;
        passed=(myinteger0<myinteger1);
      }
      else if((mytype0==admse_basicreal)&&(mytype1==admse_basicreal))
      {
        double myreal0=myadmst0->_item.r;
        double myreal1=myadmst1->_item.r;
        passed=(myreal0<myreal1);
      }
      else
        adms_message_fatal((\"%s: 'less than' - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
    }
  }
  free_ptraverse(p0);
  free_ptraverse(p1);
  return passed;
}
static int atomicgt (const p_path mypath,p_admst myadmst)
{
  p_ptraverse p0=bar(myadmst,(p_path)mypath->_u->data);
  p_ptraverse p1=bar(myadmst,(p_path)mypath->_u->next->data);
  p_slist myl0=p0->_admst;
  p_slist myl1=p1->_admst;
  int passed=1;
  if(adms_slist_length(myl0)!=adms_slist_length(myl1))
    passed=0;
  else
  {
    for(;myl0&&passed;myl0=myl0->next,myl1=myl1->next)
    {
      p_admst myadmst0=(p_admst)myl0->data;
      p_admst myadmst1=(p_admst)myl1->data;
      admse mytype0=myadmst0->_pseudo;
      admse mytype1=myadmst1->_pseudo;
      if((mytype0==admse_basicinteger)&&(mytype1==admse_basicinteger))
      {
        int myinteger0=myadmst0->_item.i;
        int myinteger1=myadmst1->_item.i;
        passed=(myinteger0>myinteger1);
      }
      else if((mytype0==admse_basicreal)&&(mytype1==admse_basicreal))
      {
        double myreal0=myadmst0->_item.r;
        double myreal1=myadmst1->_item.r;
        passed=(myreal0>myreal1);
      }
      else
        adms_message_fatal((\"%s: 'greater than' - case not implemented - sorry!\\n\",adms_transform_uid(mypath->_transform)))
    }
  }
  free_ptraverse(p0);
  free_ptraverse(p1);
  return passed;
}

#if defined(STANDALONE)
#define adms_transform_uid(txt) \"unknown\"
#endif

static void admstpatherrorimpl (const p_pparse mypparse,char const *s)
{
  adms_message_fatal_continue((\"\%s - invalid path \\\"\%s\\\"\\n\",s,mypparse->_value))
  adms_message_fatal((\"see \%s\\n\",adms_transform_uid(mypparse->_transform)))
}
";

#lexer

print admstpathYacc_y "
%}
%token tktk_cr
%token tktk_lf
%token tktk_tab
%token tktk_percent
%token tktk_percents
%token tktk_ticptr
%token tktk_anytext
%token tktk_goto_atext
%token tktk_cquote

";

map{map{print admstpathYacc_y "\%token "."tktk_".$_."\n";} split " ", xid($_);} $admstpathxml->findnodes("//constant|//regexp");
map{print admstpathYacc_y "\%type <_".xret($_)."> ".xid($_)."\n" if defined xret($_);} $admstpathxml->findnodes("//productionset");
print admstpathYacc_y "
%union
{
p_slist _slist;
p_path _path;
p_text _text;
}
%%
";

foreach($admstpathxml->findnodes("//productionset"))
{
  my$set=$_;
  print admstpathYacc_y xid($set)."\n";
  my$isfirst=1;
  foreach($set->findnodes("production"))
  {
    my$production=$_;
    my$rootcounter=0;
    my$semantic=$isfirst?" :":" |";
    my$xid=xid($production);
    my$id=$xid<10?"0$xid":"$xid";
    my$label=xlabel($production)?xlabel($production):xid($set).$id;
    $isfirst=0;
    foreach($production->findnodes("item"))
    {
      my$itemproduction=$admstpathxml->findnodes("//productionset[\@id='".xlinkto($_)."']")->[0];
      $semantic.=" ".($itemproduction?xid($itemproduction):"tktk_".xlinkto($_));
      $rootcounter++ if xlinkto($_) eq "pathroot";
    }
    $semantic.="\n";
    $semantic.=" {/*".xid($_).(xlabel($_)?" ".xlabel($_):"")."*/\n";
    my$T="   ";
    if(xid($set) eq "root")
    {
      if($xid == 0)
      {
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"");'."\n";
        $semantic.=$T.'mytext->_token=adms_slist_reverse($2);'."\n";
        $semantic.=$T.'((p_pparse)mypparse)->_text=mytext;'."\n";
      }
    }
    elsif(xid($set) eq "pathroot_l")
    {
      if($xid == 0)
      {
        $semantic.=$T.'((p_pparse)mypparse)->_path=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'((p_pparse)mypparse)->_path=$3;'."\n";
      }
    }
    elsif(xid($set) eq "pathroot")
    {
      $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
      $semantic.=$T.'adms_slist_inreverse(&$1);'."\n";
      $semantic.=$T.'$$->_bar=$1;'."\n";
      $semantic.=$T.'$$->_callback=NULL;'."\n";
      $semantic.=$T.'$$->_callbackname="path";'."\n";
    }
    elsif(xid($set) eq "pathbar")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)$1);'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=$1;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)$3);'."\n";
      }
    }
    elsif(xid($set) eq "pathadd")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'p_path addp=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'addp->_callback=(void*)addptraverse;'."\n";
        $semantic.=$T.'addp->_callbackname="addptraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&addp->_arg,(p_adms)$3);'."\n";
        $semantic.=$T.'adms_slist_push(&addp->_arg,(p_adms)$1);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)addp);'."\n";
      }
      elsif($xid == 2)
      {
        $semantic.=$T.'p_path addm=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'addm->_callback=(void*)addmtraverse;'."\n";
        $semantic.=$T.'addm->_callbackname="addmtraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&addm->_arg,(p_adms)$3);'."\n";
        $semantic.=$T.'adms_slist_push(&addm->_arg,(p_adms)$1);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)addm);'."\n";
      }
    }
    elsif(xid($set) eq "pathmult")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'p_path multp=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'multp->_callback=(void*)multtimetraverse;'."\n";
        $semantic.=$T.'multp->_callbackname="multtimetraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&multp->_arg,(p_adms)$3);'."\n";
        $semantic.=$T.'adms_slist_push(&multp->_arg,(p_adms)$1);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)multp);'."\n";
      }
      elsif($xid == 2)
      {
        $semantic.=$T.'p_path multm=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'multm->_callback=(void*)multdivtraverse;'."\n";
        $semantic.=$T.'multm->_callbackname="multdivtraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&multm->_arg,(p_adms)$3);'."\n";
        $semantic.=$T.'adms_slist_push(&multm->_arg,(p_adms)$1);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)multm);'."\n";
      }
    }
    elsif(xid($set) eq "pathunary")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'p_path unaryp=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'unaryp->_callback=(void*)unaryptraverse;'."\n";
        $semantic.=$T.'unaryp->_callbackname="unaryptraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&unaryp->_arg,(p_adms)$2);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)unaryp);'."\n";
      }
      elsif($xid == 2)
      {
        $semantic.=$T.'p_path unarym=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'unarym->_callback=(void*)unarymtraverse;'."\n";
        $semantic.=$T.'unarym->_callbackname="unarymtraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&unarym->_arg,(p_adms)$2);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)unarym);'."\n";
      }
    }
    elsif(xid($set) eq "pathincr")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'p_path incrp=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'p_path path0=(p_path)adms_slist_last($1)->data;'."\n";
        $semantic.=$T.'path0->_keeplist=1;'."\n";
        $semantic.=$T.'incrp->_callback=(void*)incrptraverse;'."\n";
        $semantic.=$T.'incrp->_callbackname="incrptraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&incrp->_arg,(p_adms)$1);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)incrp);'."\n";
      }
      elsif($xid == 2)
      {
        $semantic.=$T.'p_path incrm=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'p_path path0=(p_path)adms_slist_last($1)->data;'."\n";
        $semantic.=$T.'path0->_keeplist=1;'."\n";
        $semantic.=$T.'incrm->_callback=(void*)incrmtraverse;'."\n";
        $semantic.=$T.'incrm->_callbackname="incrmtraverse";'."\n";
        $semantic.=$T.'adms_slist_push(&incrm->_arg,(p_adms)$1);'."\n";
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)incrm);'."\n";
      }
    }
    elsif(xid($set) eq "pathsep0")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
        $semantic.=$T.'adms_slist_inreverse(&$$);'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=$1;'."\n";
        $semantic.=$T.'adms_slist_inreverse(&$$);'."\n";
      }
      elsif($xid == 2)
      {
        $semantic.=$T.'p_path pathroot=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'pathroot->_callback=(void*)root00;'."\n";
        $semantic.=$T.'pathroot->_callbackname="root00";'."\n";
        $semantic.=$T.'$$=$2;'."\n";
        $semantic.=$T.'adms_slist_inreverse(&$$);'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)pathroot);'."\n";
      }
      elsif($xid == 3)
      {
        $semantic.=$T.'p_path pathroot=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'pathroot->_callback=(void*)root00;'."\n";
        $semantic.=$T.'pathroot->_callbackname="root00";'."\n";
        $semantic.=$T.'$$=$2;'."\n";
        $semantic.=$T.'adms_slist_inreverse(&$$);'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)pathroot);'."\n";
      }
    }
    elsif(xid($set) eq "pathsep")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)$1);'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=$1;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)$3);'."\n";
      }
    }
    elsif(xid($set) eq "locationcondition")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
        $semantic.=$T.'if($2) adms_slist_push(&$$->_c,(p_adms)$2);'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_callback=(void*)location00;'."\n";
        $semantic.=$T.'$$->_callbackname="location00";'."\n";
        $semantic.=$T.'if($1) adms_slist_push(&$$->_c,(p_adms)$1);'."\n";
      }
      elsif($xid == 2)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 3)
      {
        $semantic.=$T.'$$=$2;'."\n";
        $semantic.=$T.'$$->_callbackname="groupage";'."\n";
        $semantic.=$T.'$$->_callback=(void*)groupage;'."\n";
      }
    }
    elsif(xid($set) eq "location")
    {
      if(0) {}
      elsif($label eq "intp")
      {
        $semantic.=$T.'$$=BASICINTEGER((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
      }
      elsif($label eq "realp")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
      }
      elsif($label eq "ident")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
        $semantic.=$T.'if(($$->_text->_admse==admse_past_name)||($$->_text->_admse==admse_datatypefamily))'."\n";
        $semantic.=$T.'{'."\n";
        $semantic.=$T.'  $$->_callback=(void*)location03dummy;'."\n";
        $semantic.=$T.'  $$->_callbackname="location03dummy";'."\n";
        $semantic.=$T.'}'."\n";
        $semantic.=$T.'else'."\n";
        $semantic.=$T.'{'."\n";
        $semantic.=$T.'  $$->_callback=(void*)location03;'."\n";
        $semantic.=$T.'  $$->_callbackname="location03";'."\n";
        $semantic.=$T.'}'."\n";
      }
      elsif($label eq "q")
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_text=$1;'."\n";
        $semantic.=$T.'if((adms_slist_length($$->_text->_token)==1)&&($$->_text->_token->data->_datatypename==admse_admst))'."\n";
        $semantic.=$T.'{'."\n";
        $semantic.=$T.'  $$->_callback=(void*)location09k;'."\n";
        $semantic.=$T.'  $$->_callbackname="location09k.'.$label.'";'."\n";
        $semantic.=$T.'}'."\n";
        $semantic.=$T.'else'."\n";
        $semantic.=$T.'{'."\n";
        $semantic.=$T.'  $$->_callback=(void*)location09;'."\n";
        $semantic.=$T.'  $$->_callbackname="location09.'.$label.'";'."\n";
        $semantic.=$T.'}'."\n";
      }
      elsif($label eq "ident.index")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,"index");'."\n";
      }
      elsif($label eq "ticptr")
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"\'%p\'");'."\n";
        $semantic.=$T.'adms_slist_push(&(mytext->_token),(p_adms)adms_admst_newks(adms_kclone("%p")));'."\n";
        $semantic.=$T.'mytext->_admse=admse__p;'."\n";
        $semantic.=$T.'$$->_text=mytext;'."\n";
      }
      elsif($label eq "dollar")
      {
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"$");'."\n";
        $semantic.=$T.'mytext->_token=adms_slist_reverse($2);'."\n";
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,mytext);'."\n";
      }
      elsif($label eq "aroba")
      {
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"@");'."\n";
        $semantic.=$T.'mytext->_token=adms_slist_reverse($2);'."\n";
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,mytext);'."\n";
      }
      elsif($label eq "croix")
      {
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"#");'."\n";
        $semantic.=$T.'mytext->_token=adms_slist_reverse($2);'."\n";
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,mytext);'."\n";
      }
      elsif($label eq "croixcroix")
      {
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"##");'."\n";
        $semantic.=$T.'mytext->_token=adms_slist_reverse($2);'."\n";
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,mytext);'."\n";
      }
      elsif($label eq "kdollar")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
      }
      elsif($label eq "karoba")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
      }
      elsif($label eq "kcroix")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
      }
      elsif($label eq "kcroixcroix")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
      }
      elsif($label eq "function.admst")
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
      }
      elsif($label eq "function.ident")
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($label eq "function.returned")
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_text=$3;'."\n";
      }
      elsif($label eq "function.text")
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_text=$3;'."\n";
      }
      else
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
      }
      if($rootcounter==2)
      {
        $semantic.=$T.'adms_slist_push(&$$->_u,(p_adms)$5);'."\n";
      }
      if($rootcounter)
      {
        $semantic.=$T.'adms_slist_push(&$$->_u,(p_adms)$3);'."\n";
      }
      if(($label ne "q")&&($label ne "ident"))
      {
        $semantic.=$T.'$$->_callback=(void*)'.xid($set).$id.';'."\n";
        $semantic.=$T.'$$->_callbackname="'.xid($set).$id.'.'.$label.'";'."\n";
      }
    }
    elsif(xid($set) eq "identifier")
    {
      $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
    }
    elsif(xid($set) eq "quotedstring")
    {
      if($xid == 0)
      {
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"qtext");'."\n";
        $semantic.=$T.'mytext->_token=adms_slist_reverse($2);'."\n";
        $semantic.=$T.'if(adms_slist_length(mytext->_token)==1)'."\n";
        $semantic.=$T.'{'."\n";
        $semantic.=$T.'  if(mytext->_token->data->_datatypename==admse_path)'."\n";
        $semantic.=$T.'    mytext->_admse=admse__path;'."\n";
        $semantic.=$T.'  else if(mytext->_token->data->_datatypename==admse_admst)'."\n";
        $semantic.=$T.'    mytext->_admse=ns_strtoe(((p_admst)mytext->_token->data)->_item.s);'."\n";
        $semantic.=$T.'}'."\n";
        $semantic.=$T.'$$=mytext;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'p_text mytext=adms_text_new(((p_pparse)mypparse)->_transform,"");'."\n";
        $semantic.=$T.'adms_slist_push(&(mytext->_token),(p_adms)adms_admst_newks(adms_kclone("")));'."\n";
        $semantic.=$T.'$$=mytext;'."\n";
      }
    }
    elsif(xid($set) eq "atext")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=NULL;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)$1);'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=$1;'."\n";
        $semantic.=$T.'adms_slist_push(&$$,(p_adms)$2);'."\n";
      }
    }
    elsif(xid($set) eq "atextitem")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=(p_path)adms_admst_newks(adms_kclone("\n"));'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=(p_path)adms_admst_newks(adms_kclone("\r"));'."\n";
      }
      elsif($xid == 2)
      {
        $semantic.=$T.'$$=(p_path)adms_admst_newks(adms_kclone("\t"));'."\n";
      }
      elsif($xid == 3)
      {
        $semantic.=$T.'$$=(p_path)adms_admst_newks(adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
      }
      elsif($xid == 4)
      {
        $semantic.=$T.'$$=(p_path)adms_text_new(((p_pparse)mypparse)->_transform,"%s");'."\n";
        $semantic.=$T.'((p_text)$$)->_admse=admse__s;'."\n";
      }
      elsif($xid == 5)
      {
        $semantic.=$T.'$$=(p_path)adms_text_new(((p_pparse)mypparse)->_transform,"$");'."\n";
        $semantic.=$T.'((p_text)$$)->_token=adms_slist_reverse($2);'."\n";
      }
      elsif($xid == 6)
      {
        $semantic.=$T.'char* mytvalue=adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen);'."\n";
        $semantic.=$T.'$$=(p_path)adms_text_new(((p_pparse)mypparse)->_transform,"$");'."\n";
        $semantic.=$T.'adms_slist_push(&(((p_text)$$)->_token),(p_adms)adms_admst_newks(mytvalue));'."\n";
      }
      elsif($xid == 7)
      {
        $semantic.=$T.'$$=$2;'."\n";
      }
    }
    elsif(xid($set) eq "condition")
    {
      $semantic.=$T.'$$=$2;'."\n";
    }
    elsif(xid($set) eq "expression")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_callback=(void*)expression01;'."\n";
        $semantic.=$T.'$$->_callbackname="expression01";'."\n";
        $semantic.=$T.'adms_slist_push(&$$->_u,(p_adms)$1);'."\n";
      }
    }
    elsif(xid($set) eq "binaryor")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_callback=(void*)binaryor;'."\n";
        $semantic.=$T.'$$->_callbackname="binaryor";'."\n";
        $semantic.=$T.'adms_slist_push(&$$->_b,(p_adms)$3);'."\n";
        $semantic.=$T.'adms_slist_push(&$$->_b,(p_adms)$1);'."\n";
      }
    }
    elsif(xid($set) eq "binaryand")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_callback=(void*)binaryand;'."\n";
        $semantic.=$T.'$$->_callbackname="binaryand";'."\n";
        $semantic.=$T.'adms_slist_push(&$$->_b,(p_adms)$3);'."\n";
        $semantic.=$T.'adms_slist_push(&$$->_b,(p_adms)$1);'."\n";
      }
    }
    elsif(xid($set) eq "unary")
    {
      if($xid == 0)
      {
        $semantic.=$T.'$$=$1;'."\n";
      }
      elsif($xid == 1)
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_callback=(void*)unary00;'."\n";
        $semantic.=$T.'$$->_callbackname="unary00";'."\n";
        $semantic.=$T.'adms_slist_push(&$$->_u,(p_adms)$2);'."\n";
      }
    }
    elsif(xid($set) eq "atomic")
    {
      if($label eq "isdefault")
      {
        $semantic.=$T.'$$=BASICSTRING((p_pparse)mypparse,adms_knclone(((p_pparse)mypparse)->_tkstart,((p_pparse)mypparse)->_tklen));'."\n";
        $semantic.=$T.'$$->_callback=(void*)atomicisdefault;'."\n";
        $semantic.=$T.'$$->_callbackname="atomicisdefault";'."\n";
      }
      elsif($label eq "groupage")
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_callback=(void*)atomicgroupage;'."\n";
        $semantic.=$T.'$$->_callbackname="atomicgroupage";'."\n";
        $semantic.=$T.'adms_slist_push(&$$->_u,(p_adms)$2);'."\n";
      }
      else
      {
        $semantic.=$T.'$$=adms_path_new(((p_pparse)mypparse)->_transform,NULL);'."\n";
        $semantic.=$T.'$$->_callback=(void*)atomic'.$label.';'."\n";
        $semantic.=$T.'$$->_callbackname="atomic'.$label.'";'."\n";
      }
      if($rootcounter)
      {
        $semantic.=$T.'adms_slist_push(&$$->_u,(p_adms)$3);'."\n";
      }
      if($rootcounter==2)
      {
        $semantic.=$T.'adms_slist_push(&$$->_u,(p_adms)$1);'."\n";
      }
    }
    print admstpathYacc_y $semantic;
    print admstpathYacc_y " }\n";
  }
  print admstpathYacc_y ";\n";
}
print admstpathYacc_y "
%%
static const char* tparsecb (p_pparse mypparse)
{
  const char* t=mypparse->_position;
  mypparse->_tkstart=t;
  if(0) {}
  else if(*t=='\\\''&&INSIDEPATH)
  {
    adms_slist_pull(&globalctxt);
    t+=1, mypparse->_tkid= (admse) tktk_cquote;
  }
  else if(*t=='\%')
  {
    if(*(t+1)=='\\0')
      t+=1, mypparse->_tkid= (admse) tktk_anytext;
    else if(*(t+1)=='s')
      t+=2, mypparse->_tkid= (admse) tktk_percents;
    else if(*(t+1)=='\%')
    {
      mypparse->_tkstart=t+1;
      t+=2, mypparse->_tkid= (admse) tktk_anytext;
    }
    else if(*(t+1)=='(')
    {
      paren++; adms_slist_push(&parenidx,(p_adms)(long)paren); SETPATH
      mypparse->_tkstart=t+2;
      t+=2, mypparse->_tkid= (admse) tktk_percent;
    }
    else
      t+=2, mypparse->_tkid= (admse) tktk_anytext;
  }
  else if(*t=='\\\\')
  {
    if(*(t+1)=='\\0')
      t+=1, mypparse->_tkid= (admse) tktk_anytext;
    if(*(t+1)=='n')
      t+=2, mypparse->_tkid= (admse) tktk_cr;
    else if(*(t+1)=='r')
      t+=2, mypparse->_tkid= (admse) tktk_cr;
    else if(*(t+1)=='t')
      t+=2, mypparse->_tkid= (admse) tktk_tab;
    else
    {
      mypparse->_tkstart=t+1;
      t+=2, mypparse->_tkid= (admse) tktk_anytext;
    }
  }
  else if(*t=='\$')
  {
    if(*(t+1)=='\\0')
      t+=1, mypparse->_tkid= (admse) tktk_anytext;
    if(*(t+1)=='(')
    {
      paren++; adms_slist_push(&parenidx,(p_adms)(long)paren); SETTEXT
      mypparse->_tkstart=t+2;
      t+=2, mypparse->_tkid= (admse) tktk_dollarvariable;
    }
    else
    {
      mypparse->_tkstart=t+1;
      t++;
      while(*t&&(isalnum(*t)||*t=='_'))
        t++;
      mypparse->_tkid= (admse) tktk_kdollarvariable;
    }
  }
  else if(ISTEXT&&(*t==')'))
  {
    if(PARENMATCH)
    {
      t+=1, mypparse->_tkid= (admse) tktk_closeE, adms_slist_pull(&parenidx);
      adms_slist_pull(&globalctxt);
    }
    else
      t+=1, mypparse->_tkid= (admse) tktk_anytext;
    paren--;
  }
  else
  {
    if(INSIDEPATH)
    {
      if(ISTEXT) while(*t&&(*t!=')')&&(*t!='\\0')&&(*t!='\%')&&(*t!='\\\\')&&(*t!='\$')&&(*t!='\\''))
      {
        if(*t=='(') paren++;
        t++;
      }
      else if(ISTIC) while(*t&&(*t!='\\0')&&(*t!='\%')&&(*t!='\\\\')&&(*t!='\$')&&(*t!='\\''))
      {
        t++;
      }
      else
        adms_message_fatal((\"%s:'%s' unexpected error in apath parser - please report to r29173\@users.sourceforge.net\\n\",adms_transform_uid(mypparse->_transform),mypparse->_value))
    }
    else
    {
      if(ISTEXT) while(*t&&(*t!=')')&&(*t!='\\0')&&(*t!='\%')&&(*t!='\\\\')&&(*t!='\$'))
      {
        if(*t=='(') paren++;
        t++;
      }
      else if(ISTIC) while(*t&&(*t!='\\0')&&(*t!='\%')&&(*t!='\\\\')&&(*t!='\$'))
      {
        t++;
      }
      else
        adms_message_fatal((\"%s:'%s' unexpected error in apath parser - please report to r29173\@users.sourceforge.net\\n\",adms_transform_uid(mypparse->_transform),mypparse->_value))
    }
    mypparse->_tkid= (admse) tktk_anytext;
  }
  mypparse->_tklen=(int)(long)(t-mypparse->_tkstart);
  return t;
}
static int admstpathlex (p_pparse mypparse)
{
  const char* t=mypparse->_position;
  const char* tstart;
  if(*t=='\\0')
    return 0;
  if(goto_atext)
  {
    goto_atext=0;
    return tktk_goto_atext;
  }
";
map {print admstpathYacc_y "  while(".xvalue($_).") t++;\n";} $admstpathxml->findnodes("//ignore");
print admstpathYacc_y "  tstart=t;
  if(!strncmp(t,\"\\\'%p\\'\",4)&&!isalpha(*(t+4)))
    t+=4, mypparse->_tkid= (admse) tktk_ticptr;
  else if(!ISPATH)
    t=tparsecb(mypparse);
";
foreach($admstpathxml->findnodes("//constant|//regexp"))
{
  if($_->nodeName eq "regexp")
  {
    print admstpathYacc_y $_->textContent;
  }
  elsif(xvalue($_)=~/\w$/)
  {
    my$len=length(xvalue($_));
    print admstpathYacc_y "  else if(!strncmp(t,\"".xvalue($_)."\",$len)&&!isalpha(*(t+$len)))\n";
    print admstpathYacc_y "    t+=$len, mypparse->_tkid= (admse) tktk_".xid($_).";\n";
  }
  else
  {
    my$len=length(xvalue($_));
    print admstpathYacc_y "  else if(!strncmp(t,\"".xvalue($_)."\",$len))\n";
    print admstpathYacc_y "    t+=$len, mypparse->_tkid= (admse) tktk_".xid($_).";\n";
  }
}
print admstpathYacc_y "  else
  {
    if(*t=='\\0')
      return 0;
    adms_message_fatal((\"%s: invalid path - unexpected character: %s\\n\",
    adms_transform_uid(mypparse->_transform),
    mypparse->_value))
  }
  mypparse->_position=t;
  return mypparse->_tkid;
}
p_path pparse (p_transform mytransform,const char* aname,const char* avalue)
{
  p_path mypath;
  p_pparse mypparse=adms_pparse_new(mytransform,aname,avalue);
  goto_atext=0;
  SETPATH
  globalignorequote=0;
  if(*avalue)
  {
    mypparse->_position=avalue;
    admstpathparse (mypparse);
    mypath=mypparse->_path;
    mypath->_aname=adms_kclone(aname);
    mypath->_avalue=avalue?adms_kclone(avalue):adms_kclone(\"\");
  }
  else
  {
    mypath=NULL;
    adms_message_fatal_continue((\"invalid path \\\"\%s\\\"\\n\",mypparse->_value))
    adms_message_fatal((\"see \%s\\n\",adms_transform_uid(mypparse->_transform)))
  }
  adms_slist_pull(&globalctxt);
  adms_pparse_free(mypparse);
  return mypath;
}
p_text tparse (p_ktransform mytransform, const char* aname, const char* avalue)
{
  p_text mytext;
  if(!avalue)
    mytext=adms_text_new(mytransform,avalue);
  else if(*avalue=='\\0')
  {
    p_text mytext=adms_text_new(mytransform,avalue);
    adms_slist_push(&(mytext->_token),(p_adms)adms_admst_newks(adms_kclone(\"\")));
    return mytext;
  }
  else
  {
    p_pparse mypparse=adms_pparse_new(mytransform,aname,avalue);
    SETTIC
    goto_atext=1;
    globalignorequote=1;
    mypparse->_position=avalue;
    admstpathparse (mypparse);
    mytext=mypparse->_text;
    adms_pparse_free(mypparse);
    mytext->_value=adms_kclone(avalue);
    adms_slist_pull(&globalctxt);
  }
  mytext->_admse=ns_strtoe(avalue);
  if((adms_slist_length(mytext->_token)==1)&&(mytext->_token->data->_datatypename==admse_path))
    mytext->_admse=admse__path;
  else if(!strcmp(avalue,\"\%p\"))
    mytext->_admse=admse__p;
  mytext->_aname=adms_kclone(aname);
  return mytext;
}
int apath_main (int argc,char **argv)
{
  int argci;
  int doatext=0;
  char*avalue=(argc==1)?\"a/b/c|c|c|c\":argv[1];
  for(argci=1;argci<argc;argci++)
  {
#if YYDEBUG
    admstpathdebug=admstpathdebug||!strcmp(\"-d\",argv[argci]);
#endif
    doatext=!strcmp(\"-t\",argv[argci]);
  }
  printf(\"parsing=%s\\n\",doatext?\"text\":\"apath\");
  goto_atext=doatext;
  printf(\"%s \\\"%s\\\"\\n\",argv[0],avalue);
  rootnew(adms_admsmain_new(\"admsmain\"));
  if(doatext)
    dbtext(tparse(adms_transform_new(\"<stdin>\",\"<stdin>\",0,\"*\",NULL),\"input:text\",avalue));
  else
  {
    char*aname=\"stdin\";
    p_pparse mypparse=adms_pparse_new(adms_transform_new(\"<stdin>\",\"<stdin>\",0,\"*\",NULL),aname,avalue);
    mypparse->_position=avalue;
    SETPATH
    globalignorequote=0;
    admstpathparse (mypparse);
    printf(\"<p id=\\\"%s\\\">\",avalue);
    mypparse->_path->_aname=adms_kclone(aname);
    postpath(mypparse->_path);
    dbpath(mypparse->_path);
    printf(\"</p>\\n\");
  }
  return 0;
}
";
close admstpathYacc_y;
print "$top_srcdir/admsXml/admstpathYacc.y: created\n";

