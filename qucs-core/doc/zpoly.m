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

function area = zpoly (d, n, z)
  for k = 1 : length (z)
    ds = 0;
    ns = 0;
    for l = 1 : length(d)
      ds = ds + d(l) * z(k)^(-l);
    endfor
    for l = 1 : length(n)
      ns = ns + n(l) * z(k)^(-l);
    endfor
    area(k) = - ns / ds;
  endfor
endfunction
