#include "clineest.h"
#include "fMath.h"

#include <stddef.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#define sizeof_array(arr)	(sizeof(arr)/sizeof((arr)[0]))



struct size_of_data unitfy_data_size(unsigned int sizeInBytes)
{
	struct size_of_data retVal;
	int i;

	/* Feel free to add more - just append items to the array :) */
	const char* units[] = {
		"B",
		"KB",
		"MB",
		"GB",
		"TB",
		"PB",
		"EB",
		"ZB",
		"YB"	/* We'll never get past a yottabyte of anything... right? */
	};

	retVal.quantity = sizeInBytes;
	retVal.unit = units[0];

	for(i = 1; (retVal.quantity >= 1024) && (i < sizeof_array(units)); ++i)
	{
		retVal.quantity /= 1024;
		retVal.unit = units[i];
	}

	return retVal;
}




/*
	This function is basically manually code generated.
	DON'T TOUCH UNLESS YOU UNDERSTAND WHAT AN OPCODE IS.

	See: http://igoro.com/archive/gallery-of-processor-cache-effects/
*/
static void iterate_through_data(char* data, unsigned int dataSize)
{
	static const unsigned int steps = 64*1024*1024; /* Arbitrary. */

	unsigned int lengthMod = dataSize - 1;
	unsigned int i;

	assert(is_power_of_two(dataSize));

	/*
		NOTE:
			If n is "steps", and m is maxAlignment, then this code is proportional
			to O(n log m). Therefore, be careful about increasing the step size to
			fix this assert. Try decreasing the maxAlignment instead. Or "max", as
			external code knows it by.
	*/
	assert(dataSize < steps);

	for(i = 0; i < steps; ++i)
		++data[(i * 64) & lengthMod];
}

static clock_t timed_iteration(char* data, unsigned int dataSize)
{
	clock_t begin, end;

	begin = clock();
	iterate_through_data(data, dataSize);
	end = clock();

	return end - begin;
}

static void fill_timing_data(
		clock_t* timingData,
		unsigned int timingDataLength,
		unsigned int maxAlignment
	)
{
	unsigned int currentAlignment;
	unsigned int i;

	char* targetArray = NULL;

	for(currentAlignment = 1, i = 0; currentAlignment < maxAlignment; currentAlignment *= 2, ++i)
	{
		targetArray = realloc(targetArray, currentAlignment);
		timingData[i] = timed_iteration(targetArray, currentAlignment);
	}

	free(targetArray);
}

/* There's probably a magical math formula involving logarithms that should be used here, but I'm too tired to find it. */
static unsigned int determine_size_of_timing_data_required(unsigned int maxAlignment)
{
	unsigned int size, currentAlignment;

	for(
		size = 0, currentAlignment = 1;
		currentAlignment < maxAlignment;
		++size, currentAlignment *= 2
	);

	return size;
}

/*
	This function essentially finds the biggest "jump" in timings.
	A decent heuristic - it worked on my machine!
*/
static unsigned int get_cache_line_size_from_timing_data(
		clock_t* timingData,
		unsigned int numberOfDataPoints
	)
{
	unsigned int i;
	clock_t delta;

	clock_t biggestJumpAmount = -9001;	/* It's under(?) 9000! */

	unsigned int locationOfBiggestJumpAmount = 0;

	/* We ignore the first point. You can't get a delta from just one point! */
	for(i = 1; i < numberOfDataPoints; ++i)
	{
		delta = timingData[i] - timingData[i - 1];

		if(delta > biggestJumpAmount)
		{
			biggestJumpAmount = delta;
			locationOfBiggestJumpAmount = i;
		}
	}

	/*
		The best timing data is at the point before the biggest jump,
		because it's at the magical boundary that's painful to access.
	*/
	return int_pow(2, locationOfBiggestJumpAmount - 1);
}

unsigned int get_cache_line(unsigned int max)
{
	unsigned int startAlignment = 1;

	unsigned int timingDataLength = determine_size_of_timing_data_required(max);
	clock_t* timingData = malloc(timingDataLength * sizeof(clock_t));

	unsigned int result;

	fill_timing_data(
		timingData,
		timingDataLength,
		max
	);

	result = get_cache_line_size_from_timing_data(
		timingData,
		timingDataLength
	);

	free(timingData);

	return result;
}

unsigned int estimatecache(){
/* One megabyte. Surely our cache is smaller than that! */
	unsigned int cacheLine = get_cache_line(1*1024*1024);
    struct size_of_data formattedResult = unitfy_data_size(cacheLine);

	printf("Cache line -> [%u%s]\n", formattedResult.quantity, formattedResult.unit);
    return cacheLine;
}

