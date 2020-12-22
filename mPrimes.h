#ifndef MPRIMES_H_INCLUDED
#define MPRIMES_H_INCLUDED

#define MAX_INT_SIZE 2147483646
#define MAX_LONGLONG_SIZE 9223372036854775806


typedef unsigned long long int Number;

Number findPrimesSeq(char opt, Number Limit, char verboseoutput);

Number primesMPI(Number cachesize, Number start, Number pEnd);
void primesMPIFile(Number cachesize, Number start, Number pEnd, int rank);

Number SieveCache(Number start, Number endnum) ;

//debug functions for MPI

/*void DebugWait(int rank);
void DebugWait(int rank);
*/

#endif // MPRIMES_H_INCLUDED
