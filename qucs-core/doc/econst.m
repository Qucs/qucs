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

function constants = econst (d, n, o, p)
  for k = 0 : o + 1
    fac1 = 1;
    i = 1;
    while i <= k - 1
      fac1 = fac1 * i; i++;
    endwhile
    fac2 = fac1 * k;
    s = 0;
    if k != 0
      if k == 1 && p >= length(d)
	l = p;
      else
	l = p - 1;
      endif
      for i = -1 : l
	s = s - 1 / fac1 * d(i+2) * (p - i)^(k - 1);
      endfor
    else
      fac2 = 1;
    endif
    for i = -1 : p - 1
      s = s - 1 / fac2 * n(i+2) * (p - i)^(k);
    endfor
    constants(k+1) = s;
  endfor
endfunction
