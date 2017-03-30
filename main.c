#include <stdio.h>
#include <stdint.h>
#include "clineest.h"
#include "mPrimes.h"
#include <string.h>
#include "mpi.h"
#include <sys/timeb.h>
#include <malloc.h>


void MPIPrimesdefault(int argc,char* argv[], Number Limit, int CACHESIZE,int id, int p);
void MPIPrimesMAX(int argc,char* argv[], Number Limit, int CACHESIZE, char vb,int id, int p);
void MPIPrimesFILESSPEED(int argc,char* argv[], Number Limit, int CACHESIZE, char vb,int id, int p);
void MPIPrimesExperimental(int argc,char* argv[], Number Limit, int CACHESIZE, char vb,int id, int p);

int main(int argc,char* argv[]){
    unsigned int cachesize = 256*1024;//default
    int choice = -1;//other options 4 available
    char verbose = 0;
    
    Number pLimit = MAX_INT_SIZE; //assume as default
    
    
    
     //MPI
    int id;
    int p;
    MPI_Status status;
    
    //Initialise
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    
    
    
    
	int i;
	for(i = 1;i<argc;++i)
		if(strstr(argv[i], "-C")!=NULL){
			printf("Attempting to find cacheline for Node %i...\n",id);
			cachesize = get_cache_line(1024*1024);
			 if(cachesize<32768)
				cachesize = 256*1024;
			printf("Node %i Cacheline:%i\n\n",id,cachesize);
		} 
	
	if (argc<3){//asume default, output prime file at decent speed
		MPIPrimesdefault(argc,argv, pLimit, cachesize,id,p);//assume default cacheline of 64kb
	}else{
		int i;
		for (i=1; i< argc; i++) {
			switch(argv[i][1]){
				case 'S':
				case 's':
					choice = atoi(argv[i+1]);
					break;
				case 'V':
				case 'v':
					verbose = 1;
					break;
				case 'N':
				case 'n':
					pLimit = (unsigned)atoll(argv[i+1]);
					if((pLimit<1)||(pLimit>MAX_LONGLONG_SIZE)){
						printf("Invalid input size\n");
						return -2;
					}
					else{
						printf("Prime calc for:%llu\n",pLimit);
					}
						
				default:
					break;
					
			}//end of switch
		}//end of for
		
		MPI_Barrier(MPI_COMM_WORLD);//stop before finish
		
		if(choice == 1){
			MPIPrimesdefault(argc,argv, pLimit, cachesize,id,p);
		}else if(choice ==2){
			MPIPrimesMAX(argc,argv, pLimit, cachesize,verbose,id,p);
		}else if(choice ==3){
			MPIPrimesExperimental(argc,argv, pLimit, cachesize,verbose,id,p);
		}else{
			printf("usage: -C -S [1-3] -V -N[MAX_LONG_LONG_INT-1]\n Invalid choice:%i",choice);
			return -1;
		}
	}
  return 0;
}

void MPIPrimesdefault(int argc,char* argv[], Number Limit, int CACHESIZE,int id, int p){
	if(id==0)
		printf("Prime default calc for:%llu numbers to be considered\n\n",Limit);
    ftime(&tstart);
    
    
    
    //seperate into chunks to calculate
    Number a = (Limit)/(p);
    Number from = a*(id);
    Number to = from + a;
    
    primesMPIFile(CACHESIZE,from,to,id);
	
	
	ftime(&tend);
	MPI_Barrier(MPI_COMM_WORLD);//stop before finish
	
	
	
	float diff=((float) (1000.0 * (tend.time - tstart.time) + (tend.millitm - tstart.millitm)))/1000.0;;
	printf("Node %i) Prime Computation complete, time taken:%f\n\n",id,diff);
	if(id==0){
		printf("Saving to file...\n");
		system("sh fileConcat.sh &");
	}
	
	
	MPI_Finalize();

}


void MPIPrimesExperimental(int argc,char* argv[], Number Limit, int CACHESIZE, char vb,int id, int p){
   
		if(id==0)
			printf("Prime Experimental calc for:%llu numbers to be considered\n\n",Limit);
		
    if(vb)
		if(id==0)
			printf("\nExperimental with memory alloc changes, usually crashes, file forking\n");
		
			
    //timing
    ftime(&tstart);
    
    mallopt(M_MXFAST, 1024);//mem size addressing
    
    //seperate into chunks to calculate
    Number a = (Limit)/(p);
    Number from = a*(id);
    Number to = from + a;
    
    primesMPIFile(CACHESIZE,from,to,id);
	
	
	ftime(&tend);
	
	 if(vb)
		printf("\nNode %i)Barrier reached\n",id);
	MPI_Barrier(MPI_COMM_WORLD);//stop before finish
	if(vb)
		printf("\nNode %i)Barrier left\n\n",id);
	
	float diff=((float) (1000.0 * (tend.time - tstart.time) + (tend.millitm - tstart.millitm)))/1000.0;;
	if(vb){
		printf("Node %i) Prime Computation complete, time taken:%f\n\n",id,diff);
		printf("Node %i) Calc from:%llu to:%llu\n",id, from, to);
	}
	else{
		printf("Node %i) Computation time taken:%f\n\n",id,diff);
	}
	
	if(id==0){
		printf("Saving to file...\n");
		system("sh fileConcat.sh &");
	}
	
	MPI_Finalize();

}

void MPIPrimesMAX(int argc,char* argv[], Number Limit, int CACHESIZE, char vb,int id, int p){
    if(id==0)
			printf("Prime Max speed calc for:%llu numbers to be considered\n\n",Limit);
    if(vb)
		if(id==0)
			printf("\nMAX:Aiming to achieve lowest prime calculation time\n");
			
    //timing
    ftime(&tstart);
    
     
    
    //seperate into chunks to calculate
    Number a = (Limit)/(p);
    Number from = a*(id);
    Number to = from + a;
    
    Number found = primesMPI(CACHESIZE,from,to);
	
	
	ftime(&tend);
	 if(vb)
		printf("\nNode %i)Barrier reached\n",id);
	MPI_Barrier(MPI_COMM_WORLD);//stop before finish
	 if(vb)
		printf("\nNode %i)Barrier left\n\n",id);
	
	
	float diff=((float) (1000.0 * (tend.time - tstart.time) + (tend.millitm - tstart.millitm)))/1000.0;
	if(vb){
		printf("Node %i) Prime Computation complete, time taken:%f\n",id,diff);
		printf("Node %i) Calc from:%llu to:%llu, Primes found:%llu\n",id, from, to, found);
	}
	else{
		printf("Node %i) Computation time taken:%f, Primes found:%llu\n\n",id,diff,found);
	}
	
	
	MPI_Finalize();

}


