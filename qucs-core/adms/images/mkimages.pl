#!/usr/local/bin/perl -w

## RCS Info
## $Id: mkimages.pl 891 2008-03-07 16:18:12Z r29173 $
## 
## Log
## $Log$
## Revision 1.3  2006/09/04 08:51:05  r29173
## tons of simplifications
##
## Revision 1.2  2005/05/27 16:30:38  r29173
## moving code from xmlParserNew.c to xml - added subexpression
##
## Revision 1.1  2005/05/25 09:11:54  r29173
## add png image builder
##

use GD;
my$size=10;
my$hframe=1.2*$size; #x-axis
my$vframe=$size;
sub build_png
{
  my%arg; while(@_) {my($k,$v)=(shift,shift);$arg{$k}=$v;}
  my$png=$arg{"PNG"};
  my$command=$arg{"COMMAND"};
  my$im=new GD::Image($hframe,$vframe);
  my$whitesmoke=$im->colorAllocate(245, 245, 245);
  my$black=$im->colorAllocate(0, 0, 0),
  my$red=$im->colorAllocate(255, 0, 0);
  my$blue=$im->colorAllocate(0,0,255);
  my$yellow=$im->colorAllocate(255,250,205);
  $im->transparent($whitesmoke);
  $im->interlaced(1);
  eval($command);
  open PNG,">$png.png";
  binmode PNG;
  print PNG $im->png;
  close PNG;
  print "image created ... $png.png\n";
}
&build_png(
  PNG=>'adms-enumeration',
  COMMAND=>'$im->line($size/2,0,$size/2,$size,$red);
            $im->line(0,$size/2,$size/2,$size/2,$red);'
);
&build_png(
  PNG=>'adms-list',
  COMMAND=>'$im->ellipse($size/2,$size/2,0.8*$size,0.8*$size,$blue);'
);
&build_png(
  PNG=>'adms-alias',
  COMMAND=>'$im->filledEllipse($size/2,$size/2,0.8*$size,0.8*$size,$blue);'
);
&build_png(
  PNG=>'adms-scalar',
  COMMAND=>'$im->ellipse($size/2,$size/2,0.8*$size,0.8*$size,$red);'
);
&build_png(
  PNG=>'adms-uid',
  COMMAND=>'$im->filledEllipse($size/2,$size/2,0.8*$size,0.8*$size,$red);'
);
&build_png(
  PNG=>'adms-element',
  COMMAND=>'$im->ellipse($size/2,$size/2,0.8*$size,0.8*$size,$red);
            $im->rectangle(0.1*$size,0.1*$size,0.9*$size,0.9*$size,$blue);'
);
