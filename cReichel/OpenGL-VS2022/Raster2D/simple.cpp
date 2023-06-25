// 2D
// Simple framework for 2D-Drawing

#define FREEGLUT_STATIC

#include <windows.h>		// Must have for Windows platform builds
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)

#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <Eigen/Dense>

const int RESOLUTION = 512;

struct RGBPixel {
	GLfloat R, G, B;
	RGBPixel(GLfloat r = 0, GLfloat g = 0, GLfloat b = 0) : R(r), G(g), B(b) {};
};

struct RGBColour {
	GLfloat r, g, b;
};

struct MyPoint {
	int x, y, w;
	MyPoint operator+(const MyPoint& b) {
		return { x + b.x, y + b.y };
	};
	MyPoint operator-(const MyPoint& b) {
		return { x - b.x, y - b.y };
	};
	MyPoint operator+(const int& b) {
		return { x + b, y + b };
	};
	MyPoint operator-(const int& b) {
		return { x - b, y - b };
	};
	MyPoint operator*(const int& b) {
		return { x * b, y * b };
	};
	MyPoint operator*(const float& b) {
		return { (int)lrint(x * b), (int)lrint(y * b) };
	};
};
MyPoint operator+(const int& lhs, const MyPoint& rhs) { return { lhs + rhs.x, lhs + rhs.y }; };
MyPoint operator*(const int& lhs, const MyPoint& rhs) { return { lhs * rhs.x, lhs * rhs.y }; };
MyPoint operator*(const float& lhs, const MyPoint& rhs) { return { (int)lrint(rhs.x * lhs), (int)lrint(rhs.y * lhs) }; };

struct ScanlineEdge {
	int xmin, ymin, xmax, ymax;
	float xs, deltaX;
};

enum Transformation { translate, rotate, scale, shear };

struct TransformationSettings {
	Transformation type;
	float param1;	// transformation amount X, rotation angle, scaling factor, shear amount X
	float param2; // transformation amount Y, shear amount Y
	bool keepLocation;
	MyPoint origin;
};

RGBPixel framebuffer[RESOLUTION][RESOLUTION];

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
	framebuffer[x < 0 ? 0 : (x >= RESOLUTION ? RESOLUTION - 1 : x)][y < 0 ? 0 : (y >= RESOLUTION ? RESOLUTION - 1 : y)] =
		RGBPixel(r, g, b);
	glutPostRedisplay();
}

RGBPixel getPixel(int x, int y) {
	return framebuffer[x < 0 ? 0 : (x >= RESOLUTION ? RESOLUTION - 1 : x)][y < 0 ? 0 : (y >= RESOLUTION ? RESOLUTION - 1 : y)];
}

///////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat d = 2.0f / GLfloat(RESOLUTION);
	for (int y = 0; y < RESOLUTION; y++)
		for (int x = 0; x < RESOLUTION; x++) {
			const RGBPixel& p = framebuffer[x][y];
			if (p.R == 0 && p.G == 0 && p.B == 0)
				continue;
			glColor3f(p.R, p.G, p.B);
			GLfloat vpx = GLfloat(x) * 2.0f / GLfloat(RESOLUTION) - 1.0f;
			GLfloat vpy = GLfloat(y) * 2.0f / GLfloat(RESOLUTION) - 1.0f;
			glRectf(vpx, vpy, vpx + d, vpy + d);
		}

	// Flush drawing commands
	glFlush();
}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void KeyPress(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q': exit(1);
	}
}

void ExportToPpm(std::string fname)
{
	std::ofstream out;
	out.open(fname.c_str(), std::ofstream::out | std::ofstream::trunc);
	out << std::string("P3\n") << std::to_string(RESOLUTION) << " " << std::to_string(RESOLUTION) << " 255\n";
	RGBPixel value;
	for (int y = RESOLUTION - 1; y >= 0; y--) {
		for (int x = 0; x < RESOLUTION; x++) {
			value = getPixel(x, y);
			out << std::to_string(lrintf(value.R * 255));
			out << " ";
			out << std::to_string(lrintf(value.G * 255));
			out << " ";
			out << std::to_string(lrintf(value.B * 255));
			out << "\n";
		}
	}
	out.close();
}

void Bresenham(int x1, int const x2, int y1, int const y2)
{
	int const dx = x2 - x1;
	int const dy = y2 - y1;
	int const const1 = dx + dx;
	int const const2 = dy + dy;
	int error = const2 - dx;
	while (x1 <= x2) {
		setPixel(x1, y1, 1.0f, 1.0f, 1.0f);
		x1++;
		if (error <= 0)
			error += const2;
		else {
			y1++;
			error += const2 - const1;
		}
	}
}

void BresenhamGeneric(int x1, int x2, int y1, int y2, float r, float g, float b)
{
	int const dx = x2 - x1;
	int const dy = y2 - y1;
	int const constX = 2 * dx;
	int const constY = 2 * dy;
	bool const isXBased = abs(dx) >= abs(dy);
	bool const mainDirectionIsNegative = isXBased ? (dx < 0) : (dy < 0);
	bool const secondaryDirectionIsNegative = isXBased ? (dy < 0) : (dx < 0);
	int error;
	if (isXBased) {
		error = abs(constY) - abs(dx);
	}
	else {
		error = abs(constX) - abs(dy);
	}
	bool whileCondition = true;
	if (isXBased) {
		whileCondition = mainDirectionIsNegative ? (x1 >= x2) : (x1 <= x2);
	}
	else {
		whileCondition = mainDirectionIsNegative ? (y1 >= y2) : (y1 <= y2);
	}
	while (whileCondition) {
		setPixel(x1, y1, r, g, b);
		if (isXBased) {
			if (mainDirectionIsNegative) {
				x1--;
			}
			else {
				x1++;
			}
		}
		else {
			if (mainDirectionIsNegative) {
				y1--;
			}
			else {
				y1++;
			}
		}

		if (error <= 0)
			if (isXBased) {
				error += abs(constY);
			}
			else {
				error += abs(constX);
			}
		else {
			if (isXBased) {
				if (secondaryDirectionIsNegative) {
					y1--;
				}
				else {
					y1++;
				}
				error += abs(constY) - abs(constX);
			}
			else {
				if (secondaryDirectionIsNegative) {
					x1--;
				}
				else {
					x1++;
				}
				error += abs(constX) - abs(constY);
			}
		}

		if (isXBased) {
			whileCondition = mainDirectionIsNegative ? (x1 >= x2) : (x1 <= x2);
		}
		else {
			whileCondition = mainDirectionIsNegative ? (y1 >= y2) : (y1 <= y2);
		}

	}
}

void Midpoint(int x1, int x2, int y1, int y2, float r, float g, float b)
{
	int x = x1;
	int y = y1;
	int const dx = x2 - x1;
	int const dy = y2 - y1;
	int f = dy - (dx / 2);
	for (int i = 1; i <= dx; i++) {
		setPixel(x, y, r, g, b);
		x++;
		if (f > 0) {
			y += 1;
			f -= dx;
		}
		f += dy;
	}
}

void MidpointCircle(int x, int y, int radius, float r, float g, float b)
{
	int x1 = 0;
	int y1 = radius;
	int f = 1 - radius;
	int dx = 3;
	int dy = 2 - 2 * radius;

	while (x1 <= y1) {
		setPixel(x + x1, y - y1, r, g, b);
		setPixel(x + x1, y + y1, r, g, b);
		setPixel(x - x1, y - y1, r, g, b);
		setPixel(x - x1, y + y1, r, g, b);
		setPixel(x + y1, y - x1, r, g, b);
		setPixel(x + y1, y + x1, r, g, b);
		setPixel(x - y1, y - x1, r, g, b);
		setPixel(x - y1, y + x1, r, g, b);

		x1++;

		if (f > 0) {
			y1--;
			f += dy;
			dy += 2;
		}
		f += dx;
		dx += 2;
	}
}

// resultPoints is b(i,j) in script, points is b(i)
MyPoint DeCasteljau(float t, std::vector<MyPoint> points)
{
	float firstX, secondX, firstY, secondY;
	std::vector<MyPoint> resultPoints = points;
	while (resultPoints.size() > 1) {
		std::vector<MyPoint> newResult = {};
		for (int i = 1; i < resultPoints.size(); i++) {
			MyPoint result1 = resultPoints.at(i - 1);
			firstX = result1.x * (1.0f - t);
			firstY = result1.y * (1.0f - t);

			MyPoint result2 = resultPoints.at(i);
			secondX = result1.x * t;
			secondY = result1.y * t;

			newResult.push_back({ (int)lrint(firstX + secondX), (int)lrint(firstY + secondY) });
		}
		resultPoints = newResult;
	}
	return resultPoints.back();
}

MyPoint DeCasteljauRecursive(float t, std::vector<MyPoint> points)
{
	std::vector<MyPoint> resultPoints = {};
	float lastX = points.front().x;
	float lastY = points.front().y;
	float currentX, currentY;
	for (int i = 1; i < points.size(); i++) {
		lastX = lastX * (1.0f - t);
		lastY = lastY * (1.0f - t);

		currentX = points.at(i).x;
		currentY = points.at(i).y;

		resultPoints.push_back({ (int)lrint(lastX + (currentX * t)), (int)lrint(lastY + (currentY * t)) });
		lastX = currentX;
		lastY = currentY;
	}

	if (resultPoints.size() == 1) {
		return resultPoints.front();
	}
	return DeCasteljauRecursive(t, resultPoints);
}

void DeCasteljauLine(float resolution, std::vector<MyPoint> points, float r, float g, float b)
{
	if (resolution <= 0 || resolution > 1) {
		resolution = 0.1f;
	}
	MyPoint current;
	MyPoint last = DeCasteljau(0.0f, points);
	float t = resolution;
	while (t <= 1) {
		current = DeCasteljau(t, points);
		std::cout << "current X: " << current.x << " current Y: " << current.y << std::endl;
		std::cout << "last X: " << last.x << " last Y: " << last.y << std::endl;
		BresenhamGeneric(last.x, current.x, last.y, current.y, r, g, b);
		last = current;
		t += resolution;
	}
	BresenhamGeneric(last.x, points.back().x, last.y, points.back().y, r, g, b);
}

MyPoint DeBoor(float t, std::vector<MyPoint> points, std::vector<float> nodes) {
	std::cout << "===============================================================================" << std::endl;
	std::cout << "STARTING NEW POINT AT t = " << t << std::endl;
	int n = 0;
	float const startValue = nodes.at(0);
	while (nodes.at(n + 1) == startValue) {
		n++;
	}
	int i = n;
	while (!(nodes.at(i) <= t && nodes.at(i + 1) > t)) {
		i++;
	}

	std::vector<MyPoint> resultRow = std::vector<MyPoint>();
	std::vector<MyPoint> lastRow = std::vector<MyPoint>();
	float newT;
	MyPoint newPoint = { 0,0 };
	int l;

	std::cout << "i: " << i << ", n: " << n << std::endl;

	for (int j = 0; j < n; j++) {
		resultRow = std::vector<MyPoint>();
		std::cout << "STARTING ROW" << std::endl;
		std::cout << "j: " << j << std::endl;
		std::cout << "last row is " << lastRow.size() << " items long" << std::endl;
		std::cout << "points is " << points.size() << " items long" << std::endl;
		for (int iterator = 1; iterator <= n - j; iterator++) {
			l = iterator + i - n + j - 1;
			std::cout << "l: " << l << ", iterator: " << iterator << std::endl;
			if (j == 0) {
				resultRow.push_back(points.at(l));
			}
			else {
				newT = (t - nodes.at(l)) / (nodes.at(l + n + 1 - j) - nodes.at(l));
				newPoint.x = lrint(((1 - newT) * (float)((lastRow).at(iterator - 1).x) + newT * (float)((lastRow).at(iterator).x)));
				newPoint.y = lrint(((1 - newT) * (float)((lastRow).at(iterator - 1).y) + newT * (float)((lastRow).at(iterator).y)));
				std::cout << "adding point with x: " << newPoint.x << ", y: " << newPoint.y << std::endl;
				resultRow.push_back(newPoint);
			}
		}
		lastRow = resultRow;
		std::cout << "FINISHED MY ROW YES YES" << std::endl;
	}
	return resultRow.back();
}

void DeBoorLine(float resolution, std::vector<MyPoint> points, std::vector<float> nodes, float r, float g, float b)
{
	if (resolution <= 0 || resolution > 1) {
		resolution = 0.1f;
	}
	MyPoint current;
	MyPoint last = DeBoor(0.0f, points, nodes);
	float t = resolution;
	while (t <= 1) {
		current = DeBoor(t, points, nodes);
		std::cout << "current X: " << current.x << " current Y: " << current.y << std::endl;
		std::cout << "last X: " << last.x << " last Y: " << last.y << std::endl;
		BresenhamGeneric(last.x, current.x, last.y, current.y, r, g, b);
		last = current;
		t += resolution;
	}
	BresenhamGeneric(last.x, points.back().x, last.y, points.back().y, r, g, b);
}

void drawRectangle(int x0, int x1, int y0, int y1, float r, float g, float b) {
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			setPixel(x, y, r, g, b);
		}
	}
}


int initialTriangleDecider(int xMin, int yMin, MyPoint const& p0, MyPoint const& p1) {
	return (p0.y - p1.y) * (xMin - p0.x) + (p1.x - p0.x) * (yMin - p0.y);
}

void drawTriangle(MyPoint const& p0, MyPoint const& p1, MyPoint const& p2, float r, float g, float b) {
	int const xMin = min(p0.x, min(p1.x, p2.x));
	int const yMin = min(p0.y, min(p1.y, p2.y));
	int const xMax = max(p0.x, max(p1.x, p2.x));
	int const yMax = max(p0.y, max(p1.y, p2.y));
	int initialDecider0 = initialTriangleDecider(xMin, yMin, p0, p1);
	int initialDecider1 = initialTriangleDecider(xMin, yMin, p1, p2);
	int initialDecider2 = initialTriangleDecider(xMin, yMin, p2, p0);

	int decider0, decider1, decider2;
	for (int y = yMin; y <= yMax; y++) {
		decider0 = initialDecider0;
		decider1 = initialDecider1;
		decider2 = initialDecider2;

		for (int x = xMin; x <= xMax; x++) {
			if (decider0 >= 0 && decider1 >= 0 && decider2 >= 0) {
				setPixel(x, y, r, g, b);
			}
			decider0 += (p0.y - p1.y);
			decider1 += (p1.y - p2.y);
			decider2 += (p2.y - p0.y);
		}
		initialDecider0 += (p1.x - p0.x);
		initialDecider1 += (p2.x - p1.x);
		initialDecider2 += (p0.x - p2.x);
	}
}

void drawColourTriangle(MyPoint const& p0, MyPoint const& p1, MyPoint const& p2, RGBColour const& c0, RGBColour const& c1, RGBColour const& c2) {
	int const xMin = min(p0.x, min(p1.x, p2.x));
	int const yMin = min(p0.y, min(p1.y, p2.y));
	int const xMax = max(p0.x, max(p1.x, p2.x));
	int const yMax = max(p0.y, max(p1.y, p2.y));
	int initialDecider0 = initialTriangleDecider(xMin, yMin, p0, p1);
	int initialDecider1 = initialTriangleDecider(xMin, yMin, p1, p2);
	int initialDecider2 = initialTriangleDecider(xMin, yMin, p2, p0);
	float normFactor;
	float r, g, b;

	int decider0, decider1, decider2;
	for (int y = yMin; y <= yMax; y++) {
		decider0 = initialDecider0;
		decider1 = initialDecider1;
		decider2 = initialDecider2;

		for (int x = xMin; x <= xMax; x++) {
			if (decider0 >= 0 && decider1 >= 0 && decider2 >= 0) {
				normFactor = 1 / (float)(decider0 + decider1 + decider2);
				r = normFactor * decider0 * c2.r + normFactor * decider1 * c0.r + normFactor * decider2 * c1.r;
				g = normFactor * decider0 * c2.g + normFactor * decider1 * c0.g + normFactor * decider2 * c1.g;
				b = normFactor * decider0 * c2.b + normFactor * decider1 * c0.b + normFactor * decider2 * c1.b;
				setPixel(x, y, r, g, b);
			}
			decider0 += (p0.y - p1.y);
			decider1 += (p1.y - p2.y);
			decider2 += (p2.y - p0.y);
		}
		initialDecider0 += (p1.x - p0.x);
		initialDecider1 += (p2.x - p1.x);
		initialDecider2 += (p0.x - p2.x);
	}
}

void drawPolygon(std::vector<MyPoint> const& points, float r, float g, float b) {
	struct
	{
		bool operator()(ScanlineEdge a, ScanlineEdge b) const { return a.xs < b.xs; }
	} customSortFunction;

	int yMax = points[0].y;
	int yMin = points[0].y;
	for (MyPoint const& point : points) {
		if (point.y < yMin) {
			yMin = point.y;
		}
		if (point.y > yMax) {
			yMax = point.y;
		}
	}

	// init passive edges
	std::vector<ScanlineEdge> passiveEdges = std::vector<ScanlineEdge>();
	std::vector<ScanlineEdge> activeEdges = std::vector<ScanlineEdge>();
	for (int i = 0; i < points.size(); i++) {
		MyPoint const current = points[i];
		MyPoint const last = i == 0 ? points[points.size() - 1] : points[i - 1];
		if (current.y < last.y) {
			passiveEdges.push_back({ current.x, current.y, last.x, last.y, 0.f, 0.f });
		}
		else {
			passiveEdges.push_back({ last.x, last.y, current.x, current.y, 0.f, 0.f });
		}
	}

	for (int y = yMin; y <= yMax; y++) {
		// add newly active edges to active edges
		for (ScanlineEdge edge : passiveEdges) {
			if (edge.ymin == y) {
				// initialise xs and deltaX
				// if condition to avoid divide by zero issues - these would be discarded in the next step anyway
				if (edge.ymax != edge.ymin) {
					edge.xs = edge.xmin;
					edge.deltaX = ((float)(edge.xmax - edge.xmin)) / ((float)(edge.ymax - edge.ymin));
					activeEdges.push_back(edge);
				}
			}
		}
		// remove no longer active edges from active edges
		activeEdges.erase(std::remove_if(activeEdges.begin(), activeEdges.end(), [&](const ScanlineEdge& scanlineEdge) {return scanlineEdge.ymax == y; }), activeEdges.end());

		// sort active edges
		std::sort(activeEdges.begin(), activeEdges.end(), customSortFunction);
		// set pixels
		bool isWriting = false;
		int currentX;
		for (int i = 0; i < activeEdges.size(); i++) {
			if (isWriting) {
				for (int x = currentX; x <= activeEdges[i].xs; x++) {
					setPixel(x, y, r, g, b);
				}
			}
			isWriting = !isWriting;
			currentX = (int)lrint(activeEdges[i].xs);
		}
		// increment all xs
		for (ScanlineEdge& edge : activeEdges) {
			edge.xs += edge.deltaX;
		}
	}
}

MyPoint CatmullRomSpline(float t, float tension, std::vector<MyPoint> const& points, int index) {
	Eigen::RowVector4f tValues{
		{1, t, std::powf(t, 2), std::powf(t, 3)}
	};
	Eigen::Matrix4f mainMatrix{
		{0.f, 1.f, 0.f, 0.f},
		{-1.f * tension, 0.f, tension, 0.f},
		{2.f * tension, tension - 3.f, 3.f - 2.f * tension, -1.f * tension},
		{-1.f * tension, 2.f - tension, tension - 2.f, tension}
	};
	Eigen::Vector4f xPoints{
		{(float)points[index].x, (float)points[index + 1].x, (float)points[index + 2].x, (float)points[index + 3].x}
	};
	Eigen::Vector4f yPoints{
		{(float)points[index].y, (float)points[index + 1].y, (float)points[index + 2].y, (float)points[index + 3].y}
	};
	return { (int)lrint(tValues * mainMatrix * xPoints), (int)lrint(tValues * mainMatrix * yPoints) };
}

void CatmullRomLine(float resolution, float tension, std::vector<MyPoint> const& points, float r, float g, float b) {
	// NOTE: this ignores the first and last segment, because it has to.
	if (resolution <= 0 || resolution > 1) {
		resolution = 0.1f;
	}
	MyPoint current;
	MyPoint last;
	MyPoint origin;
	for (int i = 0; i < points.size() - 3; i++) {
		std::cout << "================================================================" << std::endl;
		std::cout << "drawing line from " << points[i + 1].x << ", " << points[i + 1].y << " to " << points[i + 2].x << ", " << points[i + 2].y << std::endl;
		last = CatmullRomSpline(0.0f, tension, points, i);
		float t = resolution;
		while (t <= 1) {
			current = CatmullRomSpline(t, tension, points, i);
			std::cout << "current X: " << current.x << " current Y: " << current.y << std::endl;
			std::cout << "last X: " << last.x << " last Y: " << last.y << std::endl;
			BresenhamGeneric(last.x, current.x, last.y, current.y, r, g, b);
			last = current;
			t += resolution;
		}
		BresenhamGeneric(last.x, points[i + 2].x, last.y, points[i + 2].y, r, g, b);
	}
}

Eigen::Matrix3f getTransformationMatrixForTransformationSettings(TransformationSettings const& transformSettings) {
	switch (transformSettings.type) {
	case Transformation::translate:
		return Eigen::Matrix3f{
			{1, 0, transformSettings.param1},
			{0, 1, transformSettings.param2},
			{0, 0, 1}
		};
		break;
	case Transformation::rotate:
		return Eigen::Matrix3f{
			{std::cos(transformSettings.param1), -1 * std::sin(transformSettings.param1), 0},
			{std::sin(transformSettings.param1), std::cos(transformSettings.param1), 0},
			{0, 0, 1}
		};
		break;
	case Transformation::scale:
		return Eigen::Matrix3f{
			{transformSettings.param1, 0, 0},
			{0, transformSettings.param2, 0},
			{0, 0, 1}
		};
		break;
	case Transformation::shear:
		return Eigen::Matrix3f{
			{1, transformSettings.param1, 0},
			{transformSettings.param2, 1, 0},
			{0, 0, 1}
		};
		break;
	default:
		break;
	}

	return Eigen::Matrix3f{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1}
	};
}

void transformPipeline(std::vector<MyPoint>& points, std::vector<TransformationSettings> const& transformations) {
	// apply transformation to each point
	Eigen::Vector3f pointVector;
	std::cout << "=============================================================================================" << std::endl;
	std::cout << "STARTING TRANSFORM PIPELINE" << std::endl;
	for (MyPoint& point : points) {
		// convert to homogenous coordinates
		point.w = 1;

		Eigen::Matrix3f transformationMatrix{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1}
		};

		std::cout << "NEW POINT" << std::endl;
		// apply transformations
		pointVector = Eigen::Vector3f{ {(float)point.x, (float)point.y, (float)point.w} };
		for (TransformationSettings transformationSettings : transformations) {
			std::cout << "POINT X: " << pointVector(0) << " Y: " << pointVector(1) << " W: " << pointVector(2) << std::endl;
			if (transformationSettings.keepLocation) {
				transformationMatrix = getTransformationMatrixForTransformationSettings({ Transformation::translate, -1 * (float)transformationSettings.origin.x, -1 * (float)transformationSettings.origin.y });
				pointVector = transformationMatrix * pointVector;
			}
			transformationMatrix = getTransformationMatrixForTransformationSettings(transformationSettings);
			pointVector = transformationMatrix * pointVector;
			if (transformationSettings.keepLocation) {
				transformationMatrix = getTransformationMatrixForTransformationSettings({ Transformation::translate, (float)transformationSettings.origin.x, (float)transformationSettings.origin.y });
				pointVector = transformationMatrix * pointVector;
			}
		}
		std::cout << "FINAL POINT X: " << pointVector(0) << " Y: " << pointVector(1) << " W: " << pointVector(2) << std::endl;

		point.x = (int)lrint(pointVector(0));
		point.y = (int)lrint(pointVector(1));
		point.w = (int)lrint(pointVector(2));

		// convert back to non-homogenous coordinates
		if (point.w != 1) {
			point.x = point.x / point.w;
			point.y = point.y / point.w;
			point.w = point.w;
		}
	}

}

void transform(std::vector<MyPoint>& points, TransformationSettings const& transformSettings) {
	std::vector<TransformationSettings> transformations = std::vector<TransformationSettings>{ transformSettings };
	transformPipeline(points, transformations);
}

void drawPythagoras(int x, int y, float scaleX, float scaleY, float angle, float shear) {
	std::vector<MyPoint> trianglePoints = { {0, 0}, {40, 0}, {0, 30} };
	std::vector<MyPoint> squarePoints = { {0, 0}, {10, 0}, {10, 10}, {0, 10} };

	transform(trianglePoints, { Transformation::scale, scaleX, scaleY });
	transform(trianglePoints, { Transformation::translate, scaleY * 30.f, scaleX * 40.f });

	std::vector<MyPoint> leftSquare = squarePoints;
	// no need for preserveLocation, as square is already at 0,0 as of current
	transform(leftSquare, { Transformation::scale, scaleY * 3.f, scaleY * 3.f });
	transform(leftSquare, { Transformation::translate, 0.f, scaleX * 40.f });

	std::vector<MyPoint> bottomSquare = squarePoints;
	transform(bottomSquare, { Transformation::scale, scaleX * 4.f, scaleX * 4.f });
	transform(bottomSquare, { Transformation::translate, scaleY * 30.f, 0.f });

	std::vector<MyPoint> topSquare = squarePoints;
	float topScale = std::sqrtf(std::powf((float)scaleX * 40.f, 2) + std::powf((float)scaleY * 30.f, 2)) / 10.f;
	transform(topSquare, { Transformation::scale, topScale, topScale });
	// TODO
	transform(topSquare, { Transformation::translate, scaleY * 30.f + scaleX * 40.f, scaleX * 40.f });
	// technically the angle would be -0.6431766666666..., but this is accurate enough for all we care.
	float topAngle = std::atanf((40.f * scaleX) / (30.f * scaleY));
	transform(topSquare, { Transformation::rotate, topAngle, 0, true, topSquare[0] });

	std::vector<TransformationSettings> transformations = {};
	transformations.push_back({ Transformation::rotate, angle, 0, true, {leftSquare[0].x, bottomSquare[0].y} });
	transformations.push_back({ Transformation::shear, shear, 0, true, {leftSquare[0].x, bottomSquare[0].y} });
	transformations.push_back({ Transformation::translate, (float)x, (float)y });

	transformPipeline(trianglePoints, transformations);
	transformPipeline(leftSquare, transformations);
	transformPipeline(bottomSquare, transformations);
	transformPipeline(topSquare, transformations);

	drawPolygon(trianglePoints, 0.f, 1.f, 0.f);
	drawPolygon(leftSquare, 0.f, 0.f, 1.f);
	drawPolygon(bottomSquare, 0.f, 0.f, 1.f);
	drawPolygon(topSquare, 0.f, 0.f, 1.f);
}

MyPoint intersection(MyPoint const& p0, MyPoint const& p1, int coord, bool isXAxis) {
	float otherCoord = (float)(isXAxis ? p0.y : p0.x);
	float m = isXAxis ? (float)(p0.y - p1.y) / (float)(p0.x - p1.x) : (float)(p0.x - p1.x) / (float)(p0.y - p1.y);
	int currentCoord = isXAxis ? p0.x : p0.y;
	while (currentCoord != coord) {
		if (currentCoord > coord) {
			otherCoord -= m;
			currentCoord--;
		}
		else {
			otherCoord += m;
			currentCoord++;
		}
	}
	return isXAxis ? MyPoint{ coord, (int)lrint(otherCoord) } : MyPoint{ (int)lrint(otherCoord), coord };
}

int Outcode(MyPoint const& coords, int xmin, int xmax, int ymin, int ymax) {
	int c = 0;
	if (coords.x < xmin) {
		c = c | 8;
	}
	if (coords.x > xmax) {
		c = c | 4;
	}
	if (coords.y < ymin) {
		c = c | 2;
	}
	if (coords.y > ymax) {
		c = c | 1;
	}
	return c;
}

MyPoint getClippedPoint(MyPoint const& point, MyPoint const& otherPoint, int outcode, int xmin, int xmax, int ymin, int ymax) {
	if (outcode & 8) {
		return intersection(point, otherPoint, xmin, true);
	}
	if (outcode & 4) {
		return intersection(point, otherPoint, xmax, true);
	}
	if (outcode & 2) {
		return intersection(point, otherPoint, ymin, false);
	}
	if (outcode & 1) {
		return intersection(point, otherPoint, ymax, false);
	}
	return point;
}

// no const reference for  the points because we need to change them during the algorithm
void CohenSutherlandLine(MyPoint p0, MyPoint p1, MyPoint const& border0, MyPoint const& border1, float r, float g, float b) {
	// for debugging: compare bresenham from start points to final line
	// BresenhamGeneric(p0.x, p1.x, p0.y, p1.y, 1.f, 1.f, 1.f);
	int const xmin = min(border0.x, border1.x);
	int const xmax = max(border0.x, border1.x);
	int const ymin = min(border0.y, border1.y);
	int const ymax = max(border0.y, border1.y);

	int const outcode0 = Outcode(p0, xmin, xmax, ymin, ymax);
	int const outcode1 = Outcode(p1, xmin, xmax, ymin, ymax);

	if ((outcode0 | outcode1) == 0) {
		BresenhamGeneric(p0.x, p1.x, p0.y, p1.y, r, g, b);
	}
	else if ((outcode0 & outcode1) == 0) {
		if (outcode0 != 0) {
			p0 = getClippedPoint(p0, p1, outcode0, xmin, xmax, ymin, ymax);
		}
		else {
			p1 = getClippedPoint(p1, p0, outcode1, xmin, xmax, ymin, ymax);
		}
		BresenhamGeneric(p0.x, p1.x, p0.y, p1.y, r, g, b);
	}
	// otherwise line is entirely outside, so do not draw
}

// BUG mit sutherland-hodgman in script: bei dreieckbeispiel wird eckpunkt an ecke nicht übernommen!
// außerdem werden, wenn linie zwischen zwei außen liegenden Punkten durch bildbereich kreuzt, diese schnittpunkte ebenfalls ignoriert
void SutherlandHodgmanPolygon(std::vector<MyPoint> const& points, MyPoint const& border0, MyPoint const& border1, float r, float g, float b) {
	int const xmin = min(border0.x, border1.x);
	int const xmax = max(border0.x, border1.x);
	int const ymin = min(border0.y, border1.y);
	int const ymax = max(border0.y, border1.y);
	std::vector<MyPoint> resultPolygon = {};
	int outcode0, outcode1;

	std::cout << "=========================================================================" << std::endl;

	for (int i = 0; i < points.size(); i++) {
		std::cout << "point " << i << std::endl;
		outcode0 = Outcode(points[i == 0 ? points.size() - 1 : i - 1], xmin, xmax, ymin, ymax);
		outcode1 = Outcode(points[i], xmin, xmax, ymin, ymax);

		std::cout << "Outcodes: " << outcode0 << " and " << outcode1 << std::endl;

		if ((outcode0 | outcode1) == 0) {
			std::cout << "line was entirely inside!" << std::endl;
			// case 1: line is entirely inside
			resultPolygon.push_back(points[i]);
		}
		else if (outcode0 == 0 || outcode1 == 0) {
			// one point is inside and one is outside
			if (outcode0 != 0) {
				std::cout << "last point was outside, current is inside!" << std::endl;
				// case 2: last point was outside, current point is inside
				resultPolygon.push_back(getClippedPoint(points[i == 0 ? points.size() - 1 : i - 1], points[i], outcode0, xmin, xmax, ymin, ymax));
				resultPolygon.push_back(points[i]);
			}
			else {
				std::cout << "current point is outside, last was inside!" << std::endl;
				// case 3: current point is outside, last point was inside
				resultPolygon.push_back(getClippedPoint(points[i == 0 ? points.size() - 1 : i - 1], points[i], outcode1, xmin, xmax, ymin, ymax));
			}
		}
		// otherwise, entire line is outside and can be discarded (ignoring bugs mentioned above)
	}

	std::cout << "Finished! Polygon is made up of" << std::endl;
	for (MyPoint const& point : resultPolygon) {
		std::cout << point.x << ", " << point.y << std::endl;
	}

	if (resultPolygon.size() > 2) {
		drawPolygon(resultPolygon, r, g, b);
	}
}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutCreateWindow("2D-Test");
	glutReshapeWindow(512, 512);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(KeyPress);

	SetupRC();
	////////// put your framebuffer drawing code here /////////////

	for (int n = 0; n < RESOLUTION; n++) {
		setPixel(n, n, GLfloat(n) / RESOLUTION, 0, 1.0f - GLfloat(n) / RESOLUTION);
	}

	// BLATT 1
	/*
	setPixel(200, 100, 1.0f, 0.0f, 0.0f);
	setPixel(200, 101, 1.0f, 0.0f, 0.0f);
	setPixel(201, 100, 1.0f, 0.0f, 0.0f);
	setPixel(201, 101, 1.0f, 0.0f, 0.0f);

	for (int n = 10; n < 100; n++) {
		for (int m = 10; m < 20; m++) {
			setPixel(2 * n, m, 0.0f, 1.0f, 0.5f);
		}
	}

	Bresenham(100, 150, 50, 60);

	// first and fifth octant
	BresenhamGeneric(60, 80, 150, 160, 1.0f, 1.0f, 1.0f);
	BresenhamGeneric(40, 20, 140, 130, 1.0f, 0.0f, 0.0f);

	// second and sixth octant
	BresenhamGeneric(60, 70, 150, 170, 0.0f, 1.0f, 0.0f);
	BresenhamGeneric(40, 30, 140, 120, 0.0f, 0.0f, 1.0f);

	// third and seventh octant
	BresenhamGeneric(40, 30, 150, 170, 1.0f, 1.0f, 0.0f);
	BresenhamGeneric(60, 70, 140, 120, 1.0f, 0.0f, 1.0f);

	// fourth and eighth octant
	BresenhamGeneric(40, 20, 150, 160, 0.0f, 1.0f, 1.0f);
	BresenhamGeneric(60, 80, 140, 130, 1.0f, 1.0f, 1.0f);

	Midpoint(200, 240, 200, 220, 0.5f, 1.0f, 0.8f);
*/

// BLATT 2
// Aufgabe 1
/*
for (int x = 5; x < RESOLUTION / 2; x+=2) {
	MidpointCircle(RESOLUTION / 2, RESOLUTION / 2, x, 1.0f - (2 * (float)x / RESOLUTION), 0.0f, 2 * (float)x / RESOLUTION);
}
*/

// Aufgabe 3
	/*
	std::vector<MyPoint> points = { {60, 20}, {10, RESOLUTION - 10}, {RESOLUTION - 50, RESOLUTION - 15}, {RESOLUTION - 20, 50} };
	for (int i = 1; i < points.size(); i++) {
		BresenhamGeneric(points.at(i - 1).x, points.at(i).x, points.at(i - 1).y, points.at(i).y, 0.f, 0.f, 1.0f);
	}
	DeCasteljauLine(0.01f, points, 1.0, 0.0, 0.0);

	ExportToPpm("out.ppm");
	// used for debugging
	ExportToPpm("out.txt");
	*/

	// Aufgabe 4
	/*
	std::vector<MyPoint> points = { {10, 20}, {30, RESOLUTION - 50}, {RESOLUTION - 100, RESOLUTION - 50},{RESOLUTION - 40, 30}, {RESOLUTION - 10, 20}, {RESOLUTION - 10, RESOLUTION - 20} };
	for (int i = 1; i < points.size(); i++) {
		BresenhamGeneric(points.at(i - 1).x, points.at(i).x, points.at(i - 1).y, points.at(i).y, 0.f, 0.f, 1.0f);
	}
	std::vector<float> nodes = { 0.0f, 0.0f, 0.0f,  0.2f, 0.49f, 0.75f, 1.0f, 1.0f, 1.0f };
	DeBoorLine(0.1f, points, nodes, 0.f, 1.f, 0.f);
*/

//BLATT 3
// Aufgabe 1
	/*
	drawRectangle(10, 15, 30, 60, 0.f, 1.f, 0.f);
	drawTriangle({ 40, 40 }, { 200, 50 }, { 70, 70 }, 1.f, 0.f, 0.f);
	*/
	/*
	std::vector<MyPoint> points = { {40,50}, {60,80}, {20,100}, {60,110}, {70,150}, {80,110}, {120,100}, {80, 80} ,{100,50}, {70,70} };
	drawPolygon(points, 1.f, 1.f, 0.f);
	*/

	// Aufgabe 2
	/*
	std::vector<MyPoint> points = { {20, 200}, {80, 140}, {140, 100}, {200, 300}, {260, 120}, {340, 130}, {360, 160} };
	for (int i = 1; i < points.size(); i++) {
		BresenhamGeneric(points.at(i - 1).x, points.at(i).x, points.at(i - 1).y, points.at(i).y, 0.f, 0.f, 1.0f);
	}
	CatmullRomLine(0.05f, 0.5, points, 0.f, 1.f, 0.f);
	for (MyPoint const& point : points) {
		setPixel(point.x, point.y, 1.f, 0.f, 0.f);
	}
	*/

	// Aufgabe 3a/b
	/*
	std::vector<MyPoint> points = { {100,30 }, {150, 30}, {150, 70}, {100, 70} };
	drawPolygon(points, 1.f, 0.f, 0.f);
	transform(points, { Transformation::translate, 30, 20 });
	drawPolygon(points, 1.f, 1.f, 0.f);

	points = { {20,30 }, {70, 30}, {70, 70}, {20, 70} };
	drawPolygon(points, 1.f, 0.f, 0.f);
	transform(points, { Transformation::rotate, 0.5, 0, true, points[0] });
	drawPolygon(points, 0.f, 1.f, 0.f);

	points = { {20,130 }, {70, 130}, {70, 170}, {20, 170} };
	drawPolygon(points, 1.f, 0.f, 0.f);
	transform(points, { Transformation::scale, 0.5f, 0.5f, true, points[0] });
	drawPolygon(points, 0.f, 1.f, 1.f);

	points = { {100,130 }, {150, 130}, {150, 170}, {100, 170} };
	drawPolygon(points, 1.f, 0.f, 0.f);
	transform(points, { Transformation::shear, 1.f, 0.f, true, points[0] });
	drawPolygon(points, 0.f, 0.f, 1.f);
	*/

	// Aufgabe 3c
/*
	drawPythagoras(50, 10, 1.f, 1.f, 0.f, 0.f);
	drawPythagoras(300, 10, 2.f, 1.f, 0.f, 0.f);
	drawPythagoras(50, 300, 1.f, 1.f, 0.5f, 0.f);
	drawPythagoras(300, 300, 1.f, 1.f, 0.f, 1.f);
*/
// drawPythagoras(0, 0, 2.f, 4.f, 0.f, 0.f);

// Blatt 4
	// Aufgabe 1a
/*
	MyPoint border0 = { 100, 100 };
	MyPoint border1 = { 400, 400 };
	BresenhamGeneric(border0.x, border1.x, border0.y, border0.y, 1.f, 1.f, 0.f);
	BresenhamGeneric(border1.x, border1.x, border0.y, border1.y, 1.f, 1.f, 0.f);
	BresenhamGeneric(border0.x, border1.x, border1.y, border1.y, 1.f, 1.f, 0.f);
	BresenhamGeneric(border0.x, border0.x, border0.y, border1.y, 1.f, 1.f, 0.f);
	CohenSutherlandLine({ 150, 150 }, { 160, 250 }, border0, border1, 1.f, 0.f, 0.f);
	CohenSutherlandLine({ 40, 90 }, { 60, 80 }, border0, border1, 1.f, 0.f, 0.f);
	CohenSutherlandLine({ 40, 120 }, { 460, 130 }, border0, border1, 0.f, 1.f, 0.f);
	CohenSutherlandLine({ 120, 40 }, { 110, 460 }, border0, border1, 0.f, 1.f, 0.f);
	CohenSutherlandLine({ 120, 340 }, { 210, 460 }, border0, border1, 0.f, 0.f, 1.f);
	CohenSutherlandLine({ 360, 150 }, { 410, 330 }, border0, border1, 0.f, 0.f, 1.f);
*/

// Aufgabe 1b
	MyPoint border0 = { 100, 100 };
	MyPoint border1 = { 400, 400 };
	std::vector<MyPoint> rectInside = { {120, 120}, {150, 120}, {160, 140}, {130, 140} };
	std::vector<MyPoint> triangleOutside = { {300, 300}, {500, 300}, {350, 500} };
	std::vector<MyPoint> bugDemo = { { 50, 200}, {200, 200}, {200, 500} };
	std::vector<MyPoint> workingPolygonOutside = { {300, 120}, {500, 120}, {480, 140}, {340, 190}, {250, 180}, {230, 80}};
	drawPolygon(triangleOutside, 1.f, 1.f, 0.f);
	drawPolygon(bugDemo, 1.f, 1.f, 0.f);
	drawPolygon(workingPolygonOutside, 1.f, 1.f, 0.f);
	BresenhamGeneric(border0.x, border1.x, border0.y, border0.y, 1.f, 0.f, 0.f);
	BresenhamGeneric(border1.x, border1.x, border0.y, border1.y, 1.f, 0.f, 0.f);
	BresenhamGeneric(border0.x, border1.x, border1.y, border1.y, 1.f, 0.f, 0.f);
	BresenhamGeneric(border0.x, border0.x, border0.y, border1.y, 1.f, 0.f, 0.f);
	SutherlandHodgmanPolygon(rectInside, border0, border1, 0.f, 0.f, 1.f);
	SutherlandHodgmanPolygon(triangleOutside, border0, border1, 0.f, 0.f, 1.f);
	SutherlandHodgmanPolygon(bugDemo, border0, border1, 0.f, 0.f, 1.f);
	SutherlandHodgmanPolygon(workingPolygonOutside, border0, border1, 0.f, 0.f, 1.f);

	// Aufgabe 2
/*
	drawColourTriangle({ 40, 40 }, { 200, 50 }, { 200, 180 }, { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f,  1.f });
	drawColourTriangle({ 300, 40 }, { 500, 40 }, { 400, 140 }, { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f,  1.f });
	drawColourTriangle({ 300, 300 }, { 500, 300 }, { 400, 400 }, { 1.f, 1.f, 1.f }, { 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f });
	drawColourTriangle({ 40, 300 }, { 240, 300 }, { 140, 400 }, { 1.f, 1.f, 0.f }, { 0.f, 1.f, 1.f }, { 1.f, 0.f, 1.f });
*/

/////////////////////////////////
	glutMainLoop();

	return 0;
}

