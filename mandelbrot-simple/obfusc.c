#include <complex.h>
main(){complex z,c=-2-I;for(int x=0;x<6400;++x){z=c;char *f="██";for(int t=0;t<100;++t){z=cpow(z,2)+c;cabs(z)>2?f="  ":f;}printf(f);c+=.03125;if(!(x%80)){c=-2+(cimag(c)+.025)*I;puts("");}}}
