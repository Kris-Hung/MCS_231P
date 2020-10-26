#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"
    
int main(int argc, char **argv){
    //help message
    if(argc != 5){
	printf("USAGE: %s max met t verb\n",argv[0]);
	printf("max     : Maximum number to scan primes on.\n");
	printf("met     : Method: Single thread = 's'\n");
	printf("                  Multi thread  = 'm'\n");
	printf("t       : Number of threads to use\n");
	printf("          Ignored when met=s\n");
	printf("verb    : 0=prints only execution time.\n");
	printf("          1=prints also the numbers.\n");
	exit(1);
    }

    //get the command line arguments
    unsigned int max, met, t, verb;
    max=atoi(argv[1]);
    met=*argv[2];
    t=atoi(argv[3]);
    verb=atoi(argv[4]);
    if(10000000 < max){
	printf("You can search primes up ");
	printf("to 10^7.\n");
	exit(1);
    }
    if(met == 'm' && t < 1){
	printf("The number of threads 't' (%u) must ", t);
	printf("be >= 1.\n");
	exit(1);
    }

    //run and measure time taken
    double end, start;
    if(met == 's'){
	start = stopwatch();
	primes_st(max, verb);
	end = stopwatch();
    }else if(met == 'm'){
	start = stopwatch();
	primes_mt(max, t, verb);
	end = stopwatch();
    }else{
	printf("Error: incorrect value of 'met' (%c).\n", met);
	exit(1);
    }

    //print execution time
    printf("%.8f\n", end - start);
    
    return 0;
}
