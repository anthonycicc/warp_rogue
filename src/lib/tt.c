/*
 * Copyright (C) 2002-2007 The Warp Rogue Team
 *
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * See the license.txt file for more details.
 */

/*
 * Random number generator (True Twister)
 *
 *
 * NAMESPACE: TT_* / tt_* / mt_*
 *
 */


#include "tt.h"


#define TT_DEFAULT_SEED		1981l


static int			tt_rand(int);

static void			mt_init(unsigned long);
static unsigned long		mt_genrand_int32(void);



/*
 * True Twister seed
 */
static unsigned long		TT_Seed = TT_DEFAULT_SEED;



/*
 * seeds the RNG
 */
void tt_seed(unsigned long seed)
{

	TT_Seed = seed;

	mt_init(TT_Seed);
}



/*
 * returns the seed of the RNG
 */
unsigned long tt_get_seed(void)
{

	return TT_Seed;
}



/*
 * returns a random integer >= a and <= b
 */
int tt_random_int(int a, int b)
{
	int r;

	r = tt_rand(b - a);

	r += a;

	return r;
}



/*
 * returns a random integer from 0..range inclusive.
 */
static int tt_rand(int range)
{
	int mask, result;

	if (range == 0) return 0;

	if (range < 0) return -tt_rand(-range);

    	/* Find the next largest power of two. */
    	mask = 1;

	while (mask <= range) mask <<= 1;

	/* Find the bit mask. */
	--mask;

	/*
	 * Now repeatedly rand & mask until less than range.
	 * This avoids any round off problems with large values of range
	 */
	do {

		result = mt_genrand_int32();

		result &= mask;

	} while (result > range);

	return result;
}



/*
   MT19937

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/* Period parameters */
#define N		624
#define M		397
#define MATRIX_A	0x9908b0dfUL /* constant vector a */
#define UPPER_MASK	0x80000000UL /* most significant w-r bits */
#define LOWER_MASK	0x7fffffffUL /* least significant r bits */


static unsigned long	mt[N]; /* the array for the state vector  */
static int		mti=N+1; /* mti==N+1 means mt[N] is not initialized */


/*
 * initializes mt[N] with a seed
 */
static void mt_init(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] =
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}



/*
 * generates a random number on [0,0xffffffff]-interval
 */
static unsigned long mt_genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            mt_init(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

