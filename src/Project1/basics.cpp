#include <fstream>
#include <cstdio>
#include <iostream>
#include <list>
#include <vector>

struct myEvent 
{
	float time;
	int thing;
};

using namespace std;

constexpr auto dimx = 512u, dimy = 512u;

unsigned char imageBuff[dimx][dimy][3];
int liveBuff[dimx][dimy][3];
bool isLive[dimx][dimy];

#define RED 0
#define GREEN 1
#define BLUE 2

void defaultPixels()
{
	for (int i = 0; i < dimx; i++)
		for (int j = 0; j < dimy; j++)
		{
			imageBuff[i][j][RED] = 0;
			imageBuff[i][j][GREEN] = 0;
			imageBuff[i][j][BLUE] = 0;
		}
}

/// <summary>
/// Set the color of a pixel.
/// </summary>
/// <param name="x">
/// [0, 1] x-coordinate of the pixel.
/// </param>
/// <param name="y">
/// [0, 1] y-coordinate of the pixel.
/// </param>
/// <param name="color">
/// { RED, GREEN, BLUE } color values.
/// </param>
void drawPixel(float x, float y, unsigned char color[3])
{
	int ix = (int)(x * dimx);
	int iy = (int)((1.0f - y) * dimy);

	if (ix < 0 || ix >= dimx || iy < 0 || iy >= dimy)
		return;
	
	imageBuff[iy][ix][RED] = color[RED];
	imageBuff[iy][ix][GREEN] = color[GREEN];
	imageBuff[iy][ix][BLUE] = color[BLUE];
}

void drawPoints(float points[][2], int numPoints, unsigned char color[3])
{
	for (int i = 0; i < numPoints; i++)
		drawPixel(points[i][0], points[i][1], color);
}

/// <summary>
/// Draw a line from point1 to point2.
/// </summary>
/// <param name="point1">
/// { x, y } coordinates of the first point.
/// </param>
/// <param name="point2">
/// { x, y } coordinates of the second point.
/// </param>
/// <param name="color">
/// { RED, GREEN, BLUE } color values.
/// </param>
void drawLine(float point1[2], float point2[2], unsigned char color[3])
{
	float x1 = point1[0];
	float y1 = point1[1];

	float x2 = point2[0];
	float y2 = point2[1];

	float dx = abs(x2 - x1);
	float dy = abs(y2 - y1);
	
	float px = dx * 2;
	float py = dy * 2;
	
	float x = x1;
	float y = y1;
	
	float xInc = x2 > x1 ? 1.0f / dimx : -1.0f / dimx;
	float yInc = y2 > y1 ? 1.0f / dimy : -1.0f / dimy;

	// draw the last pixel ahead of time
	drawPixel(x2, y2, color);
	
	if (dx > dy)
	{
		float accum = -dx;
		for (x = x1; x < x2; x += xInc)
		{
			drawPixel(x, y, color);
			accum += py;
			if (accum > 0.0f)
			{
				y += yInc;
				accum -= px;
			}
		}
	}
	else
	{
		float accum = -dy;
		for (y = y1; y < y2; y += yInc)
		{
			drawPixel(x, y, color);
			accum += px;
			if (accum > 0.0f)
			{
				x += xInc;
				accum -= py;
			}
		}
	}
}

/// <summary>
/// Draw a wireframed triangle given the vertices.
/// </summary>
/// <param name="point1">
/// { x, y } coordinates of the first point.
/// </param>
/// <param name="point2">
/// { x, y } coordinates of the second point.
/// </param>
/// <param name="point3">
/// { x, y } coordinates of the third point.
/// </param>
/// <param name="color">
/// { RED, GREEN, BLUE } color values.
/// </param>
void drawTriangle(float point1[2], float point2[2], float point3[2], unsigned char color[3])
{
	drawLine(point1, point2, color);
	drawLine(point2, point3, color);
	drawLine(point3, point1, color);
}

/// <summary>
/// Draw a wireframed rectangle given the vertices.
/// </summary>
/// <param name="point1">
/// { x, y } coordinates of the first point.
/// </param>
/// <param name="point2">
/// { x, y } coordinates of the second point.
/// </param>
/// <param name="point3">
/// { x, y } coordinates of the third point.
/// </param>
/// <param name="point4">
/// { x, y } coordinates of the fourth point.
/// </param>
/// <param name="color">
/// { RED, GREEN, BLUE } color values.
/// </param>
void drawRectangle(float point1[2], float point2[2], float point3[2], float point4[2], 
	unsigned char color[3])
{
	drawLine(point1, point2, color);
	drawLine(point2, point3, color);
	drawLine(point3, point4, color);
	drawLine(point4, point1, color);
}

/// <summary>
/// Draw a wireframed circle around the center with a given radius and color.
/// </summary>
/// <param name="center">
/// { x, y } coordinates of the center.
/// </param>
/// <param name="radius">
/// Radius of the circle.
/// </param>
/// <param name="color">
/// { RED, GREEN, BLUE } color values.
/// </param>
void drawCircle(float center[2], float radius, unsigned char color[3])
{
	float x = 0.0f;
	float y = radius;

	float p = 1.0f - radius;

	while (y >= 0.0f)
	{
		drawPixel(center[0] + x, center[1] + y, color);
		drawPixel(center[0] + x, center[1] - y, color);
		drawPixel(center[0] - x, center[1] + y, color);
		drawPixel(center[0] - x, center[1] - y, color);
		drawPixel(center[0] + y, center[1] + x, color);
		drawPixel(center[0] + y, center[1] - x, color);
		drawPixel(center[0] - y, center[1] + x, color);
		drawPixel(center[0] - y, center[1] - x, color);

		if (p < 0.0f)
			p += 2.0f * x + 3.0f;
		else
		{
			p += 2.0f * (x - y) + 5.0f;
			y -= 1.0f / dimy;
		}

		x += 1.0f / dimx;
	}
}

/// <summary>
/// Draw a Bezier curve from point1 to point4 with a given color.
/// </summary>
/// <param name="point1">
/// { x, y } coordinates of the starting point.
/// </param>
/// <param name="point2">
/// { x, y } coordinates of the first control point.
/// </param>
/// <param name="point3">
/// { x, y } coordinates of the second control point.
/// </param>
/// <param name="point4">
/// { x, y } coordinates of the ending point.
/// </param>
/// <param name="numSegments">
/// Number of line segments to make the curve. 
/// More segments = smoother curve but less efficient.
/// </param>
/// <param name="color">
/// { RED, GREEN, BLUE } color values.
/// </param>
void drawBezier(float point1[2], float point2[2], float point3[2], float point4[2], 
	int numSegments, unsigned char color[3])
{
	float t = 0.0f;
	float step = 1.0f / numSegments;

	float x1 = point1[0];
	float y1 = point1[1];
	
	float x2 = point2[0];
	float y2 = point2[1];
	
	float x3 = point3[0];
	float y3 = point3[1];

	float x4 = point4[0];
	float y4 = point4[1];

	float x = 0.0f;
	float y = 0.0f;

	while (t <= 1.0f)
	{
		x = powf(1.0f - t, 3) * x1 +
			3.0f * t * powf(1.0f - t, 2) * x2 +
			3.0f * powf(t, 2.0f) * (1.0f - t) * x3 +
			powf(t, 3.0f) * x4;

		y = powf(1.0f - t, 3.0f) * y1 +
			3.0f * t * powf(1.0f - t, 2.0f) * y2 +
			3.0f * powf(t, 2.0f) * (1.0f - t) * y3 +
			powf(t, 3.0f) * y4;

		drawPixel(x / dimx, y / dimy, color);

		t += step;
	}
}

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

	// write checkerboard
	/*
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
					putPixel(i, j, 255 * (double)(dimx - i) / dimx, 
						255 * (double)(dimy - j) / dimy, 0);
				else
					putPixel(i, j, 0, 0, 0);
			}
		}
		*/

	return 0;
}