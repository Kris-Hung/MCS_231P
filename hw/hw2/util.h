/**
 ** Takes the current time in seconds
 **/
extern double stopwatch(void);

/**
 ** Finds and print all prime numbers beween 1 and @max.
 ** If @verb != 0 then print the prime numbers as they
 ** are found.
 **/
extern void primes_st(unsigned int max, unsigned int verb);

/**
 ** Finds and print all prime numbers beween 1 and @max
 ** utilizing @threads threads. If @verb != 0 then print
 ** the prime numbers as they are found.
 **/
extern void primes_mt(unsigned int max, unsigned int threads,	\
		      unsigned int verb);
