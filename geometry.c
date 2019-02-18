#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"

RECT rect(int x, int y, int size_x, int size_y)
{
	RECT rectangle;

	rectangle.x = x;
	rectangle.y = y;
	rectangle.size_x = size_x;
	rectangle.size_y = size_y;

	return rectangle;
}

POINT2 point2(int x, int y)
{
	POINT2 point;

	point.x = x;
	point.y = y;

	return point;
}

POINT3 point3(int x, int y, int z)
{
	POINT3 point;

	point.x = x;
	point.y = y;
	point.z = z;

	return point;
}

VECTOR2 vector2(float x, float y)
{
	VECTOR2 vector;
	
	vector.x = x;
	vector.y = y;

	return vector;
}

VECTOR3 vector3(float x, float y, float z)
{
	VECTOR3 vector;
	
	vector.x = x;
	vector.y = y;
	vector.z = z;

	return vector;
}

int inside_rect(int x, int y, RECT rect)
{
	if(	x >= rect.x && x < rect.x + rect.size_x &&
		y >= rect.y && y < rect.y + rect.size_y)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}