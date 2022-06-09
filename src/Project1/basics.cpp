#include <fstream>
#include <cstdio>
#include <iostream>
#include <list>

struct myEvent {
	float time;
	int thing;
};

using namespace std;

constexpr auto dimx = 512u, dimy = 512u;

unsigned char imageBuff[dimx][dimy][3];
unsigned int liveBuff[dimx][dimy][3];
bool isLive[dimx][dimy];

#define RED 0
#define GREEN 1
#define BLUE 2

int putPixel(int x, int y, int r, int g, int b)
{
	if (x < 0 || x >= dimx || y < 0 || y >= dimy)
		return -1;
	liveBuff[x][y][RED] = r;
	liveBuff[x][y][GREEN] = g;
	liveBuff[x][y][BLUE] = b;
	return 0;
}

// Prepare isLive for Game of Life
int setupLife()
{
	for (int i = 0; i < dimx; i++)
		for (int j = 0; j < dimy; j++)
		{
			isLive[i][j] = false;
			imageBuff[i][j][RED] = imageBuff[i][j][GREEN] = imageBuff[i][j][BLUE] = 0;
		}
	return 0;
}

// Set live cell at (x,y)
int setLive(int x, int y)
{
	if (x < 0 || x >= dimx || y < 0 || y >= dimy)
		return -1;
	isLive[x][y] = true;
	return 0;
}

// Trigger the next life generation
int nextLife()
{
	for (int i = 0; i < dimx; i++)
		for (int j = 0; j < dimy; j++)
		{
			int count = 0;
			for (int k = -1; k <= 1; k++)
				for (int l = -1; l <= 1; l++)
					if (i + k >= 0 && i + k < dimx && j + l >= 0 && j + l < dimy)
						if (isLive[i + k][j + l])
							count++;
			if (isLive[i][j])
			{
				if (count < 2 || count > 3)
				{
					imageBuff[i][j][RED] = imageBuff[i][j][GREEN] = imageBuff[i][j][BLUE] = 0;
					isLive[i][j] = false;
				}
				else
				{
					imageBuff[i][j][RED] = liveBuff[i][j][RED];
					imageBuff[i][j][GREEN] = liveBuff[i][j][GREEN];
					imageBuff[i][j][BLUE] = liveBuff[i][j][BLUE];
				}
			}
			else
			{
				if (count == 3)
				{
					imageBuff[i][j][RED] = liveBuff[i][j][RED];
					imageBuff[i][j][GREEN] = liveBuff[i][j][GREEN];
					imageBuff[i][j][BLUE] = liveBuff[i][j][BLUE];
					isLive[i][j] = true;
				}
				else
				{
					imageBuff[i][j][RED] = imageBuff[i][j][GREEN] = imageBuff[i][j][BLUE] = 0;
				}
			}
		}
	return 0;
}

int myTexture()
{
	memset(imageBuff, 0, sizeof(imageBuff));

	/*
	for (int i = 0; i < dimx; i++)
		for (int j = 0; j < dimy; j++)
		{
			if (((i / 16) % 2) == 0)
			{
				if (((j / 16) % 2) == 0)
				{
					imageBuff[i][j][RED] = 0;
					imageBuff[i][j][GREEN] = 0;
					imageBuff[i][j][BLUE] = 255;
				}
			}
			else
			{
				if (((j / 16) % 2) == 1)
				{
					imageBuff[i][j][RED] = 0;
					imageBuff[i][j][GREEN] = 0;
					imageBuff[i][j][BLUE] = 255;
				}
			}
		}
	*/

	// write checkerboard
	for (int i = 0; i < dimx; i++)
		for (int j = 0; j < dimy; j++)
		{
			if (((i / 16) % 2) == 0)
			{
				if (((j / 16) % 2) == 0)
					putPixel(i, j, 0, 255 * (double)i / dimx, 255 * (double)j / dimy);
				else
					putPixel(i, j, 0, 0, 0);
			}
			else
			{
				if (((j / 16) % 2) == 1)
					putPixel(i, j, 255 * (double)(dimx - i) / dimx, 255 * (double)(dimy - j) / dimy, 0);
				else
					putPixel(i, j, 0, 0, 0);
			}
		}

	return 0;
}