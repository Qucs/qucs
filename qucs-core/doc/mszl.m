% -*-octave-*-
% 
% Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
%
% This is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This software is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this package; see the file COPYING.  If not, write to
% the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
% Boston, MA 02111-1307, USA.  
%

gset terminal postscript eps "Times-Roman" 20;
gset output "mszl.eps";
hold off;
gplot clear;
gset nokey;

Z0     = 376.73031346958504364963;
M_PI_2 = 1.5707963267948966192313216916397514;
M_PI_4 = 0.7853981633974483096156608458198757;
M_PI   = 3.1415926535897932384626433832795029;
M_2_PI = 0.6366197723675813430755350534900574;
M_1_PI = 0.3183098861837906715377675267450287;

u = linspace(0.01,5.01,101);
k = [1 2.1 2.5 3.78 9.8 12.9 16];

gset xrange [0:5];
gset yrange [0:250];
gset grid;
ylabel("impedance ZL in Ohm");
xlabel("normalised strip width W/h");

for n = 1 : length (k)
  er = k(n);
  for i = 1 : length (u)
    if (u(i) < 3.3)
      c = log (4 / u(i) + sqrt (16 / u(i)^2 + 2));
      z = c - 0.5 * (er - 1) / (er + 1) * (log (M_PI_2) + log (2*M_2_PI) / er);
      z = z * Z0 / M_PI / sqrt (2 * (er + 1));
    else
      c = u(i) / 2 + log (4) / M_PI;
      c = c + (er + 1) / 2 / M_PI / er * log (M_PI_2 * e * (u(i) / 2 + 0.94));
      c = c + (er - 1) / 2 / M_PI / er^2 * log (e * M_PI^2 / 16);
      z = Z0 / 2 / sqrt (er) / c;
    endif
    zl(i,n) = z;
  endfor
  plot (u,zl(:,n),"-1;;");
  hold on;
endfor
