/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaGeometryUtils.cpp
Language:  C++
Date:      $Date: 2022-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGeometryUtils.h"
#include "vtkMath.h"
#include "vtkLine.h"

#define VTK_NO_INTERSECTION 0
#define VTK_YES_INTERSECTION  2
#define EPSILON 1e-5;

/// Points

//---------------------------------------------------------------------------
double albaGeometryUtils::Dot(double *point1, double *point2)
{
	return vtkMath::Dot(point1, point2);
	//return point1[X] * point2[X] + point1[Y] * point2[Y] + point1[Z] * point2[Z];
}
//---------------------------------------------------------------------------
double albaGeometryUtils::Mag(double *point1)
{
	return std::sqrt(point1[X] * point1[X] + point1[Y] * point1[Y] + point1[Z] * point1[Z]);
}
//---------------------------------------------------------------------------
float albaGeometryUtils::Norm2(double *point1)
{
	//return vtkMath::Norm2D(point1);
	return point1[X] * point1[X] + point1[Y] * point1[Y] + point1[Z] * point1[Z];
}
//---------------------------------------------------------------------------
double albaGeometryUtils::Norm(double *point1)
{
	return vtkMath::Norm(point1);
	//return sqrt(Norm2(point1));
}
//---------------------------------------------------------------------------
void albaGeometryUtils::Cross(double *point1, double *point2, double *cross)
{
	vtkMath::Cross(point1, point2, cross);

// 	double cross[3];
// 	cross[X] = point1[Y] * point2[Z] - point1[Z] * point2[Y];
// 	cross[Y] = point1[Z] * point2[X] - point1[X] * point2[Z];
// 	cross[Z] = point1[X] * point2[Y] - point1[Y] * point2[X];
}

/// Points Utils

//---------------------------------------------------------------------------
bool albaGeometryUtils::Equal(double *point1, double *point2)
{
	return (point1[X] == point2[X]) && (point1[Y] == point2[Y]) && (point1[Z] == point2[Z]);
}

//----------------------------------------------------------------------------
double albaGeometryUtils::DistanceBetweenPoints(double *point1, double *point2)
{
	return sqrt(vtkMath::Distance2BetweenPoints(point1, point2));
	//return sqrt(pow(point1[X] - point2[X], 2) + pow(point1[Y] - point2[Y], 2) + pow(point1[Z] - point2[Z], 2));
}

//---------------------------------------------------------------------------
void albaGeometryUtils::GetMidPoint(double(&midPoint)[3], double *point1, double *point2)
{
	midPoint[X] = (point1[X] + point2[X]) / 2;
	midPoint[Y] = (point1[Y] + point2[Y]) / 2;
	midPoint[Z] = (point1[Z] + point2[Z]) / 2;
}

//----------------------------------------------------------------------------
double albaGeometryUtils::GetAngle(double point1[3], double point2[3]) // Degree
{
	return std::acos(Dot(point1, point2) / (Mag(point1)*Mag(point2))) * 180.0 / vtkMath::Pi();
}
//----------------------------------------------------------------------------
double albaGeometryUtils::GetAngle(double* point1, double* point2, double* origin) // Degree
{
	double ab[3] = { origin[X] - point1[X], origin[Y] - point1[Y], origin[Z] - point1[Z] };
	double bc[3] = { point2[X] - origin[X], point2[Y] - origin[Y], point2[Z] - origin[Z] };

	double abVec = sqrt(ab[X] * ab[X] + ab[Y] * ab[Y] + ab[Z] * ab[Z]);
	double bcVec = sqrt(bc[X] * bc[X] + bc[Y] * bc[Y] + bc[Z] * bc[Z]);

	double abNorm[3] = { ab[X] / abVec, ab[Y] / abVec, ab[Z] / abVec };
	double bcNorm[3] = { bc[X] / bcVec, bc[Y] / bcVec, bc[Z] / bcVec };

	double res = abNorm[X] * bcNorm[X] + abNorm[Y] * bcNorm[Y] + abNorm[Z] * bcNorm[Z];

	return 180.0 - (acos(res) * 180.0 / vtkMath::Pi());
}

//----------------------------------------------------------------------------
double albaGeometryUtils::DistancePointToLine(double * point, double * lineP1, double * lineP2)
{
	return vtkLine::DistanceToLine(point, lineP1, lineP2);

// 	double ab[3] = { lineP2[X] - lineP1[X], lineP2[Y] - lineP1[Y], lineP2[Z] - lineP1[Z] };
// 	double ac[3] = { point[X] - lineP1[X], point[Y] - lineP1[Y], point[Z] - lineP1[Z] };
// 
// 	double cross[3];
// 	Cross(ab, ac, cross);
// 	
// 	double area = Mag(cross);
// 	double cd = area / Mag(ab);
// 
// 	return cd;
}
//----------------------------------------------------------------------------
// double albaGeometryUtils::GetPointToLineDistance(double *point, double *linePoint1, double *linePoint2)
// {
// 	double a = linePoint1[Y] - linePoint2[Y]; // Note: this was incorrectly "y2 - y1" in the original answer
// 	double b = linePoint2[X] - linePoint1[X];
// 	double c = linePoint1[X] * linePoint2[Y] - linePoint2[X] * linePoint1[Y];
// 
// 	return abs(a * point[X] + b * point[Y] + c) / sqrt(a * a + b * b);
// }
//----------------------------------------------------------------------------
// float albaGeometryUtils::DistancePointToLine(double * point, double * lineP1, double * lineP2)
// {
// 	return sqrt(vtkLine::DistanceToLine(point, lineP1, lineP2));

	// 	double point_x = point[X];
	// 	double point_y = point[Y];
	// 
	// 	double line_x1 = lineP1[X];
	// 	double line_y1 = lineP1[Y];
	// 	double line_x2 = lineP2[X];
	// 	double line_y2 = lineP2[Y];
	// 
	// 	double diffX = line_x2 - line_x1;
	// 	double diffY = line_y2 - line_y1;
	// 
	// 	if ((diffX == 0) && (diffY == 0))
	// 	{
	// 		diffX = point_x - line_x1;
	// 		diffY = point_y - line_y1;
	// 		return sqrt(diffX * diffX + diffY * diffY);
	// 	}
	// 
	// 	float t = ((point_x - line_x1) * diffX + (point_y - line_y1) * diffY) / (diffX * diffX + diffY * diffY);
	// 
	// 	if (t < 0)
	// 	{
	// 		// Point is nearest to the first point i.e x1 and y1
	// 		diffX = point_x - line_x1;
	// 		diffY = point_y - line_y1;
	// 	}
	// 	else if (t > 1)
	// 	{
	// 		// Point is nearest to the end point i.e x2 and y2
	// 		diffX = point_x - line_x2;
	// 		diffY = point_y - line_y2;
	// 	}
	// 	else
	// 	{
	// 		// If perpendicular line intersect the line segment.
	// 		diffX = point_x - (line_x1 + t * diffX);
	// 		diffY = point_y - (line_y1 + t * diffY);
	// 	}
	// 
	// 	// Returning shortest distance
	// 	return sqrt(diffX * diffX + diffY * diffY);
//}

//----------------------------------------------------------------------------
bool albaGeometryUtils::GetLineLineIntersection(double(&point)[3], double *line1Point1, double *line1Point2, double *line2Point1, double *line2Point2)
{
// 	double u, v;
// 	int val = vtkLine::Intersection(line1Point1, line1Point2, line2Point1, line2Point2, u, v);
// 	vtkLine::IntersectWithLine()

	double da[3] = { line1Point2[X] - line1Point1[X], line1Point2[Y] - line1Point1[Y], line1Point2[Z] - line1Point1[Z] };
	double db[3] = { line2Point2[X] - line2Point1[X], line2Point2[Y] - line2Point1[Y], line2Point2[Z] - line2Point1[Z] };
	double dc[3] = { line2Point1[X] - line1Point1[X], line2Point1[Y] - line1Point1[Y], line2Point1[Z] - line1Point1[Z] };

	double cross_da_db[3];
	Cross(da, db, cross_da_db);
	if (Dot(dc, cross_da_db) != 0.0) // Lines are not coplanar
		return false;

	double cross_dc_db[3];
	Cross(da, db, cross_dc_db);

	double s = Dot(cross_dc_db, cross_da_db) / Norm2(cross_da_db);
	if (s >= 0.0 && s <= 1.0)
	{
		point[X] = line1Point1[X] + da[X] * s;
		point[Y] = line1Point1[Y] + da[Y] * s;
		point[Z] = line1Point1[Z] + da[Z] * s;
		return true;
	}

	return false;
}



//----------------------------------------------------------------------------
void albaGeometryUtils::RotatePoint(double *point, double *origin, double angle)
{
	double s = sin(angle);
	double c = cos(angle);

	double d = Dot(origin, point);
	double cr[3];
	Cross(cr, origin, point);

	//glm::dvec3 rotated = (v * c) + (glm::cross(k, v) * s) + (k * glm::dot(k, v)) * (1 - c);

	double pnt1[3]{ point[X] * c, point[Y] * c, point[Z] * c };

	cr[X] *= s;
	cr[Y] *= s;
	cr[Z] *= s;

	double kdot[3]{ origin[X] * d, origin[Y] * d, origin[Z] * d };
	kdot[X] *= (1 - c);
	kdot[Y] *= (1 - c);
	kdot[Z] *= (1 - c);

	point[X] = pnt1[X] + cr[X] + kdot[X];
	point[Y] = pnt1[Y] + cr[Y] + kdot[Y];
	point[Z] = pnt1[Z] + cr[Z] + kdot[Z];

	return;
	// Translate point back to origin:
	point[X] -= origin[X];
	point[Y] -= origin[Y];
	point[Z] -= origin[Z];

	double RotatedPoint[3];

	//First rotate the Z:
	RotatedPoint[X] = point[X] * c - point[Y] * s;
	RotatedPoint[Y] = point[X] * s + point[Y] * c;
	RotatedPoint[Z] = point[Z];

	//Second rotate the Y:
	RotatedPoint[X] = RotatedPoint[X] * c + RotatedPoint[Z] * s;
	RotatedPoint[Y] = RotatedPoint[Y];
	RotatedPoint[Z] = RotatedPoint[Y] * s + RotatedPoint[Z] * c;

	//Third rotate the X:
	RotatedPoint[X] = RotatedPoint[X];
	RotatedPoint[Y] = RotatedPoint[Y] * c - RotatedPoint[Z] * s;
	RotatedPoint[Z] = RotatedPoint[Y] * s + RotatedPoint[Z] * c;

	// Translate point back:
	point[X] = RotatedPoint[X] + origin[X];
	point[Y] = RotatedPoint[Y] + origin[Y];
	point[Z] = RotatedPoint[Z] + origin[Z];
}


/// Lines

//----------------------------------------------------------------------------
int albaGeometryUtils::IntersectLineLine(double *l1p1, double *l1p2, double *l2p1, double *l2p2, double &perc)
{
	double x[3];
	double projXYZ[3];
	int i;
	double l2Perc;

	double tol = EPSILON;

	if (vtkLine::Intersection(l1p1, l1p2, l2p1, l2p2, perc, l2Perc) == VTK_YES_INTERSECTION)
	{
		// make sure we are within tolerance
		for (i = 0; i < 3; i++)
		{
			x[i] = l2p1[i] + l2Perc * (l2p2[i] - l2p1[i]);
			projXYZ[i] = l1p1[i] + perc*(l1p2[i] - l1p1[i]);
		}
		return vtkMath::Distance2BetweenPoints(x, projXYZ) <= tol*tol;
	}
	else return false;

}




//----------------------------------------------------------------------------
void albaGeometryUtils::GetParallelLine(double(&point1)[3], double(&point2)[3], double *linePoint1, double *linePoint2, double distance)
{
	double L = sqrt(pow((linePoint2[X] - linePoint1[X]), 2) + pow((linePoint2[Y] - linePoint1[Y]), 2) + pow((linePoint2[Z] - linePoint1[Z]), 2));

	point1[X] = linePoint1[X] + distance * (linePoint2[Y] - linePoint1[Y]) / L;
	point1[Y] = linePoint1[Y] + distance * (linePoint1[X] - linePoint2[X]) / L;
	point1[Z] = linePoint1[Z] + distance * (linePoint2[Z] - linePoint1[Z]) / L;

	point2[X] = linePoint2[X] + distance * (linePoint2[Y] - linePoint1[Y]) / L;
	point2[Y] = linePoint2[Y] + distance * (linePoint1[X] - linePoint2[X]) / L;
	point2[Z] = linePoint2[Z] + distance * (linePoint2[Z] - linePoint1[Z]) / L;
}
//----------------------------------------------------------------------------
bool albaGeometryUtils::FindPointOnLine(double(&point)[3], double *linePoint1, double *linePoint2, double distance)
{
	double L = sqrt(pow((linePoint2[X] - linePoint1[X]), 2) + pow((linePoint2[Y] - linePoint1[Y]), 2));
	double dist_ratio = distance / L;

	point[X] = (1 - dist_ratio)*linePoint1[X] + dist_ratio * linePoint2[X];
	point[Y] = (1 - dist_ratio)*linePoint1[Y] + dist_ratio * linePoint2[Y];
	//point[Z] = 0.0;

	return (dist_ratio > 0 && dist_ratio < 1); //the point is on the line.
}

//----------------------------------------------------------------------------
int albaGeometryUtils::PointUpDownLine(double *point, double *lp1, double *lp2)
{
	double d = (point[X] - lp1[X]) * (lp2[Y] - lp1[Y]) - (point[Y] - lp1[Y]) * (lp2[X] - lp1[X]);

	if (d > 0)
		return 1;
	else if (d < 0)
		return -1;
	else
		return 0;
}

void albaGeometryUtils::rotAroundZ(double *point, float degree)
{
	double n_point[3];

	n_point[X] = (point[X] * cos(degree * vtkMath::Pi() / 180.0)) - (point[Y] * sin(degree * vtkMath::Pi() / 180.0));
	n_point[Y] = (point[X] * sin(degree * vtkMath::Pi() / 180.0)) + (point[Y] * cos(degree * vtkMath::Pi() / 180.0));
	n_point[Z] = point[Z];

	point[X] = n_point[X];
	point[Y] = n_point[Y];
	point[Z] = n_point[Z];
}

void albaGeometryUtils::rotAroundY(double *point, float degree)
{
	double n_point[3];

	n_point[X] = (point[X] * cos(degree * vtkMath::Pi() / 180.0)) + (point[Z] * sin(degree * vtkMath::Pi() / 180.0));
	n_point[Y] = point[Y];
	n_point[Z] = ((point[X] * -1.0f) * sin(degree * vtkMath::Pi() / 180.0)) + (point[Z] * cos(degree * vtkMath::Pi() / 180.0));;

	point[X] = n_point[X];
	point[Y] = n_point[Y];
	point[Z] = n_point[Z];
}

void albaGeometryUtils::rotAroundA(double *point, double *axis, float zdegree)
{
	double v1[3] = { 1.0f, 0.0f, 0.0f };
	double v2[3] = { 0.0f, 1.0f, 0.0f };

	float xdegree = GetAngle(axis, v1);
	float ydegree = GetAngle(axis, v2);

	rotAroundZ(point, xdegree);
	rotAroundY(point, ydegree);
	rotAroundZ(point, zdegree);
	rotAroundY(point, -ydegree);
	rotAroundZ(point, -xdegree);
}

/*
void albaGeometryUtils::rotAObject(Object& obj, double *axis, float degree)
{
axis = glm::normalize(axis);
translate(axis, glm::vec3{ axis[X], axis[Y], axis[Z] });
for (int i = 0; i < obj.vertices.size(); i++)
{
rotAroundA(obj.vertices[i], axis, degree);
}
rotAroundA(obj.mp, axis, degree);
translate(axis, glm::vec3{ axis[X] * -1.0f, axis[Y] * -1.0f, axis[Z] * -1.0f });
}*/
