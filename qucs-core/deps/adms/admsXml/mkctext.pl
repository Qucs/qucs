#!/bin/perl -w
#
# mktext.pl
#
# perl helper script for adms build system
#
# takes two argments, a file name and a path intended to
# be the top level source directory path for adms.
#
# It creates a small C file, which copies the input file into a C
# character array. The name of the C file is generated from the
# input file name e.g.
#
# > echo "Some text" > infile.txt
#
# > mktext.pl infile.txt /my/top/level/source/directory
# svn version: Unversioned directory
# created: infile.txt.c
#
# > cat infile.txt.c
# /* File automatically created by mkctext.pl*/
#
# const char * infile_txt = ""
# "Some text\n"
# ;
#
# if the directory containing the file is under subversion
# version control, the text SVN_VERSION in the file is replaced
# with the version number reported by svnversion
#


my $filename=shift;
my $top_srcdir=shift;
$top_srcdir=".." if not defined $top_srcdir;

#git
# cygpath is a Cygwin utility to convert Unix and Windows format paths
my$cygpath_top_srcdir=$top_srcdir;
if($cygpath_top_srcdir=`cygpath -ad $top_srcdir 2>/dev/null`)
{
  chomp $cygpath_top_srcdir;
  $cygpath_top_srcdir="\"$cygpath_top_srcdir\"";
}
my$GIT;
$GIT=`git log --pretty=format:'%h' -n 1u` or $GIT="unknown";
#print "git version: $GIT\n";

sub text2ccode
{
  my$fileFullName = shift;
  $fileFullName =~ m/\/?([^\/]+)$/;
  my$fileName = $1;
  my$OutputCfile = "$fileName.c";
  my$cName = $fileName;
  $cName =~ s/\./_/g;
  open IF, "<${fileFullName}";
  open OCF, ">${OutputCfile}";
  printf(OCF "/* File automatically created by " . __FILE__ . "*/\n");
  printf(OCF "\n");
  printf(OCF "const char * %s = \"\"\n",${cName});
  while(<IF>)
  {
    my $line=$_;
    $line=~s/\r?$//;
    $line=~s/\n$//;
    $line=~s/\\/\\\\/g;
    $line=~s/"/\\"/g;
    $line=~s/\@GIT_VERSION=[^@]*\@/\@GIT_VERSION=$GIT\@/g;
    printf(OCF "\"%s\\n\"\n", $line);
  }
  printf(OCF ";\n");
  close IF;
  close OCF;
  print "created: ${OutputCfile}\n";
}

&text2ccode($filename);
