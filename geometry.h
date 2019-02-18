#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct RECT_
{
	int x;
	int y;
	int size_x;
	int size_y;
} 
RECT;

typedef struct POINT2_
{
	int x;
	int y;
}
POINT2;

typedef struct POINT3_
{
	int x;
	int y;
	int z;
}
POINT3;

typedef struct VECTOR2_
{
	float x;
	float y;
}
VECTOR2;

typedef struct VECTOR3_
{
	float x;
	float y;
	float z;
}
VECTOR3;

RECT rect(int x, int y, int size_x, int size_y);

POINT2 point2(int x, int y);
POINT3 point3(int x, int y, int z);

VECTOR2 vector2(float x, float y);
VECTOR3 vector3(float x, float y, float z);

int inside_rect(int x, int y, RECT rect);

#endif