# fastSieve
A fairly fast c based prime calculator. Calculates in parallel &amp; sequentially

Makes use of OpenMPI to assist in paralell computation

https://www.open-mpi.org/

It takes into account the cache size of the CPU it is calculating, showing vast improvments to calculation speed.
It estimates the cachline of the cpu.

For running on Linux the following is required to be installed before execution:
sudo apt update

sudo apt install build-essential

sudo apt-get install --reinstall openmpi-bin libopenmpi-dev

Compile by typing `make` in bash and running the resultant `a.out`

As this supports OpenMPI it can be executed like:
`mpiexec a.out -s 2 -c -v 1 -n 100`

it supports elementary command line input eg:

-N = Max value of Primes to test to factor. 1 = MAX_INT, 2 = 2*MAX_INT etc

-V = Toggle verbose mode

-S = a value [1-3] that changes how the primes are tested & calculated. 1 -> Default; 2 -> Most optimiaztions used; 3 -> Some experimental options enabled.

-C to estimate the L2 cache of the system and use it effeciently -NB option for speed

## Please take note that options 1 & 3 output to files which grow in size quickly. at N=1 ~ 1.5GB & N=10 ~ 112GB

