#ifndef LOADER_H
#define LOADER_H

struct TYPE_CASE
{
	char name[12];
	unsigned char result;
	unsigned char error;
	unsigned char reserved;
};
typedef struct TYPE_CASE CASE;
#endif
