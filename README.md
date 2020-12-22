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

it supports elementary command line input eg:

-N = Max value of Primes to test to - Supports up to Long Long Int

-V = Toggle verbose mode

-S = a value [1-3] that changes how the primes are tested & calculated. 1 -> Default; 2 -> Most optimiaztions used; 3 -> Some experimental options enabled.


# Please take note that it outputs to a file. This can take a very long time and be very large ~1.5GB

