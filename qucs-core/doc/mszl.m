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

H=100; W=H*4/3+25;

eglobpar;                % access to global parameters
ePageOrientation=0;      % set page orientation

ePageWidth=W+40;         % set page size and ratio
ePageHeight=H+25;
eWinWidth=ePageWidth;
eWinHeight=ePageHeight;

eopen ("mszl.eps",ePageOrientation,ePageWidth,ePageHeight);

% apply plot rectangle area
ePlotAreaPos=[22 15];
ePlotAreaHeight=H;
ePlotAreaWidth=W;

% set grid options
eXGridVisible=1; eXGridDash=1;
eYGridVisible=1; eYGridDash=1;

% set axis layout
eXAxisNorthValueVisible=0;
eXAxisSouthScale=[0 1 5];
eYAxisEastValueVisible=0;
eYAxisWestScale=[0 50 250];
eYAxisWestLabelDistance=8;

% set axes options
eAxesValueFontSize=5;
eAxesLabelFontSize=eAxesValueFontSize;
eAxesLabelTextFont=1;
%eAxesTicShortLength=0;

% set legend options
ePlotLegendFontSize=eAxesValueFontSize;
ePlotLegendTextFont=eAxesLabelTextFont;


Z0     = 376.73031346958504364963;
M_PI_2 = 1.5707963267948966192313216916397514;
M_PI_4 = 0.7853981633974483096156608458198757;
M_PI   = 3.1415926535897932384626433832795029;
M_2_PI = 0.6366197723675813430755350534900574;
M_1_PI = 0.3183098861837906715377675267450287;

u = linspace(0.01,5.01,101);
k = [1 2.1 2.5 3.78 9.8 12.9 16];

eXAxisSouthLabelText="normalised strip width W/h";
eYAxisWestLabelText="impedance ZL in Ohm";
eAxesLabelTextFont=1;

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
  eplot (u,zl(:,n),"",0,[0 0 0],1);
endfor

eclose (1,0);
%eview;
