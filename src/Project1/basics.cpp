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
unsigned int liveBuff[dimx][dimy][3];
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
void drawPixel(float x, float y, vector<unsigned char> color)
{
	int ix = (int)(x * dimx);
	int iy = (int)((1.0f - y) * dimy);

	if (ix < 0 || ix >= dimx || iy < 0 || iy >= dimy)
		return;
	
	imageBuff[ix][iy][RED] = color[RED];
	imageBuff[ix][iy][GREEN] = color[GREEN];
	imageBuff[ix][iy][BLUE] = color[BLUE];
}

void drawPoints(vector<vector<float>> points, int numPoints, vector<unsigned char> color)
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
void drawLine(vector<float> point1, vector<float> point2, vector<unsigned char> color)
{
	float x1 = point1[0];
	float y1 = point1[1];

	float x2 = point2[0];
	float y2 = point2[1];

	float dx = x2 - x1;
	float dy = y2 - y1;
	
	float slope = dy / dx;
	
	float x = x1;
	float y = y1;
	
	while (x <= x2)
	{
		drawPixel(x, y, color);
		x += 1.0f;
		y += slope;
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
void drawTriangle(vector<float> point1, vector<float> point2, vector<float> point3, vector<unsigned char> color)
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
void drawRectangle(vector<float> point1, vector<float> point2, vector<float> point3, vector<float> point4, 
	vector<unsigned char> color)
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
void drawCircle(vector<float> center, float radius, vector<unsigned char> color)
{
	float x = 0.0f;
	float y = radius;
	
	float d = 1.0f - radius;
	
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

		if (d < 0.0f)
			d += 2.0f * x + 3.0f;
		else
		{
			d += 2.0f * (x - y) + 5.0f;
			y -= 1.0f;
		}
		
		x += 1.0f;
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
void drawBezier(vector<float> point1, vector<float> point2, vector<float> point3, vector<float> point4, 
	int numSegments, vector<unsigned char> color)
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

		drawPixel(x, y, color);

		t += step;
		
		if (t > 1.0f)
			t = 1.0f;
	}
	
	//drawPixel(x1, y1, color);
	//drawPixel(x2, y2, color);
	//drawPixel(x3, y3, color);
	//drawPixel(x4, y4, color);
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
					putPixel(i, j, 255 * (double)(dimx - i) / dimx, 
						255 * (double)(dimy - j) / dimy, 0);
				else
					putPixel(i, j, 0, 0, 0);
			}
		}

	return 0;
}