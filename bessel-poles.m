#!/usr/bin/octave -qf

for n=1:20;

b=1;
for k=1:n+1;
b(k)=factorial(2*n-(k-1))/((2^(n-(k-1)))*factorial(k-1)*factorial(n-(k-1)));
endfor;
b=fliplr(b);
#disp(b);
printf("Bessel polynomial %dth order poles:",n);
roots(b)

endfor;
