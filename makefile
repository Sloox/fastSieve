
all:
	mpicc main.c fMath.c clineest.c mPrimes.c -o a.out -lm -O3 -Wno-deprecated-declarations -Wno-implicit-function-declaration
