#ifndef __TYPES_H
#define __TYPES_H

#define TRUE   1
#define FALSE  0

#define MAX_WIDTH 512
#define MAX_HEIGHT 512

typedef unsigned char  BOOL;

typedef unsigned char  BYTE;
typedef unsigned short HALFWORD;
typedef unsigned int   WORD;

typedef struct {
	WORD  Height;
	WORD  Width;
	BYTE  Pixels[MAX_WIDTH * MAX_HEIGHT * 2 + MAX_HEIGHT];
} IMAGE;

#endif /* __TYPES_H */
