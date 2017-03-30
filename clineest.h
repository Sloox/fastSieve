#ifndef CLINEEST_H_INCLUDED
#define CLINEEST_H_INCLUDED



unsigned int estimatecache();


struct size_of_data
{
	unsigned int quantity;
	const char* unit;
};

struct size_of_data unitfy_data_size(unsigned int sizeInBytes);


#endif // CLINEEST_H_INCLUDED
