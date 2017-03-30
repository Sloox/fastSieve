#include "fMath.h"

static int is_odd(unsigned int num)
{
	return num & 1;
}

static unsigned int square(unsigned int num)
{
	return num*num;
}

unsigned int int_pow(unsigned int base, unsigned int exponent)
{
	if(exponent >= 2)
	{
		if(is_odd(exponent))
			return int_pow(base, exponent - 1) * base;
		else
			return square(int_pow(base, exponent / 2));
	}
	else
	{
		if(exponent == 1)
			return base;

		else
			return 1;
	}
}

int is_power_of_two(unsigned int num)
{
	return ((num & (num - 1)) == 0) && num;
}
