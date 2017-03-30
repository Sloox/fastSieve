#include "mPrimes.h"
#include "clineest.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <string.h>


void primesMPIFile(Number cachesize, Number start, Number pEnd, int rank){
	Number from;
	Number to;
	for ( from = start; from < pEnd; from += cachesize){
        to = from + cachesize;
        if (to > pEnd)
            to = pEnd;
        SieveFile(from, to, rank);
    }
}

Number primesMPI(Number cachesize, Number start, Number pEnd){
	int found = 0;
	Number from;
	Number to;
	for ( from = start; from < pEnd; from += cachesize){
        to = from + cachesize;
        if (to > pEnd)
            to = pEnd;
        found += SieveCache(from, to);
    }
	return found;
}

Number findPrimesSeq(char opt, Number Limit, char verboseoutput){
	struct timeb tstart, tend, cStart, cEnd;
	int found = 0;
    Number ssize = 256*1024;//i have a 256 cacheline
    if(opt){//max opt for differnt machines
        unsigned int cacheLine = get_cache_line(1024*1024);
        if(cacheLine>1024)
            ssize = cacheLine;
        if(verboseoutput)
			printf("Found cacheline of:%i\n",cacheLine);
    }
    
    if(verboseoutput)
		ftime(&tstart);
		
     Number from;

     for ( from = 2; from <= Limit; from += ssize){
        Number to = from + ssize;
        if (to > Limit)
            to = Limit;
     found += SieveCache(from, to);
    }
    
	 if(verboseoutput){
		ftime(&tend);
		float diff =((float) (1000.0 * (tend.time - tstart.time) + (tend.millitm - tstart.millitm)))/1000.0;
		printf("Primes found:%i\n",found+1);
		printf("Computation Time:%f\n",diff);
	}
		
    return found +1;
}




__inline Number SieveCache(Number start, Number endnum) {
    // initialize
    Number blockSize = (endnum - start +1)/2;
    Number i,j,index,blockstart;
    unsigned char* isPrime;
    isPrime = malloc((blockSize+1)* sizeof(unsigned char));
    memset(isPrime, 1, sizeof(unsigned char)*(blockSize+1));

    // find all non-primes
    for (i = 3; i*i<endnum; i+=2){
    
		if (i >= 9 && i % 3 == 0)
			continue;
		if (i >= 25 && i % 5 == 0)
			continue;
		if (i >= 49 && i % 7 == 0)
			continue;
		if (i >= 121 && i % 11 == 0)
			continue;
		if (i >= 169 && i % 13 == 0)
			continue;
		if(i >= 17*17 && i%17==0)
			continue;

        blockstart = ((start+i-1)/i)*i;

        if (blockstart < i*i) //esnure min is reached
                blockstart = i*i;

        // start value must be odd
        if ((blockstart & 1) == 0)
            blockstart += i;

        for (j = blockstart; j <= endnum; j += 2*i){
            index = j - start;
            isPrime[index/2] = 0;
        }
    }
    // sieve is complete, count primes
    int found = 0;
    for (i = 0; i < blockSize; i++)
    if(isPrime[i])
        found +=isPrime[i];
    free(isPrime);
    return found;
}

__inline void SieveFile(Number start, Number endnum, int rank) {
    // initialize
    Number blockSize = (endnum - start +1)/2;
    Number i,j,index,blockstart;
    unsigned char* isPrime;
    isPrime = malloc((blockSize+1)* sizeof(unsigned char));
    memset(isPrime, 1, sizeof(unsigned char)*(blockSize+1));

    // find all non-primes
    for (i = 3; i*i<endnum; i+=2){
    
		if (i >= 9 && i % 3 == 0)
			continue;
		if (i >= 25 && i % 5 == 0)
			continue;
		if (i >= 49 && i % 7 == 0)
			continue;
		if (i >= 121 && i % 11 == 0)
			continue;
		if (i >= 169 && i % 13 == 0)
			continue;
		if(i >= 17*17 && i%17==0)
			continue;

        blockstart = ((start+i-1)/i)*i;

        if (blockstart < i*i) //esnure min is reached
                blockstart = i*i;

        // start value must be odd
        if ((blockstart & 1) == 0)
            blockstart += i;

        for (j = blockstart; j <= endnum; j += 2*i){
            index = j - start;
            isPrime[index/2] = 0;
        }
    }
 
    savetofile(&isPrime,blockSize+1,rank, start,endnum);
    free(isPrime);
}

void savetofile(unsigned char ** arr, Number pLimit, int rank, Number start, Number endn){
	FILE *fp;
	char buffer[10];
	snprintf(buffer, sizeof(buffer), "P%i.txt", rank);
	
	fp=fopen(buffer, "a+");
	if(fp == NULL)
	   return;
	Number i;
	
	for(i=1;i<pLimit;++i)
		if((*arr)[i])
			fprintf(fp,"%llu \n",i*2+1+start);
			
	fclose(fp);
}
//debug
/*static void DebugPrint(int rank) {
    char a;
    MPI_Bcast(&a, 1, MPI_BYTE, 0, MPI_COMM_WORLD);
    printf("%d: DEBUG reached\n", rank);
}

static void DebugWait(int rank) {
    char a;
    scanf(&a);
    MPI_Bcast(&a, 1, MPI_BYTE, 0, MPI_COMM_WORLD);
    printf("%d: DEBUG reached\n", rank);
}
*/
