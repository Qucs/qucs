#!/bin/perl -w

my$fileFullName=shift;
my $top_srcdir=shift;
$top_srcdir=".." if not defined $top_srcdir;

#svn
my$SVN;
$SVN=`svnversion -n $top_srcdir 2>mkctext.pl.err` or $SVN="unknown";
print "svn version: $SVN\n";

$fileFullName =~ m/\/?([^\/]+)$/;
my$fileName = $1;
my$OutputCfile = "$fileName.c";
my$cName = $fileName;
$cName =~ s/\./_/g;
open IF, "<${fileFullName}";
open OF, ">${OutputCfile}";
printf(OF "/* File automatically created by " . __FILE__ . "*/\n");
printf(OF "\n");
printf(OF "const char * %s = \"\"\n",${cName});
while(<IF>)
{
  my $line=$_;
  $line=~s/\r?$//;
  $line=~s/\n$//;
  $line=~s/\\/\\\\/g;
  $line=~s/"/\\"/g;
  $line=~s/\@SVN_VERSION=[^@]*\@/\@SVN_VERSION=$SVN\@/g;
  printf(OF "\"%s\\n\"\n", $line);
}
printf(OF ";\n");
close IF;
close OF;
print "created: ${OutputCfile}\n";
