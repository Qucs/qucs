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

gset terminal postscript eps "Times-Roman" 28;
gset output "radius.eps";
hold on;
gplot clear;

gset xrange [0.6:0.7];
gset yrange [-0.5:1];
gset grid;

Is = 1e-13;
Ut = 25e-3;

u  = linspace(0.6,0.7,100);
i0 = Is * (exp(u/Ut) - 1);
i1 = Is/Ut * exp(u/Ut);
i2 = Is/Ut/Ut * exp(u/Ut);
i3 = Is/Ut/Ut/Ut * exp(u/Ut);
z0 = linspace(0,0,100);

r0 = ((1 + i1.^2).^(3/2)) ./ i2;
r1 = (((3/2)*((1+i1.^2).^(1/2))*2.*i1) ./ i2);
r1 = r1 .+ (((1+i1.^2).^(3/2)) ./ (i2.^2));

r1 = i2 .* 3/2.*(1+i1.^2).^(1/2)*2.*i1.*i2 - (1+i1.^2).^(3/2) .* i3;
r1 = r1./(i2.^2);
r1 = r1./20;

gset key top left Left reverse;

plot (u,r0,"-1;curve radius;");
plot (u,r1,"-2;derivative of curve radius;");
plot (u,z0,"-3;;");
