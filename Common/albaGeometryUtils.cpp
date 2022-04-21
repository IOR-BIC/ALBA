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
#include "vtkTransform.h"
#include "albaVect3d.h"

#define VTK_NO_INTERSECTION 0
#define VTK_YES_INTERSECTION  2
#define EPSILON 1e-5;

/// Points

//---------------------------------------------------------------------------
double albaGeometryUtils::Dot(double *point1, double *point2)
{
	return vtkMath::Dot(point1, point2);
}
//---------------------------------------------------------------------------
double albaGeometryUtils::Mag(double *point1)
{
	return std::sqrt(point1[X] * point1[X] + point1[Y] * point1[Y] + point1[Z] * point1[Z]);
}
//---------------------------------------------------------------------------
float albaGeometryUtils::Norm2(double *point1)
{
	return point1[X] * point1[X] + point1[Y] * point1[Y] + point1[Z] * point1[Z];
}
//---------------------------------------------------------------------------
double albaGeometryUtils::Norm(double *point1)
{
	return vtkMath::Norm(point1);
}
//---------------------------------------------------------------------------
void albaGeometryUtils::Cross(double *point1, double *point2, double *cross)
{
	vtkMath::Cross(point1, point2, cross);
}

//---------------------------------------------------------------------------
bool albaGeometryUtils::Equal(double *point1, double *point2)
{
	return (point1[X] == point2[X]) && (point1[Y] == point2[Y]) && (point1[Z] == point2[Z]);
}
//----------------------------------------------------------------------------
double albaGeometryUtils::DistanceBetweenPoints(double *point1, double *point2)
{
	return sqrt(vtkMath::Distance2BetweenPoints(point1, point2));
}
//---------------------------------------------------------------------------
void albaGeometryUtils::GetMidPoint(double(&midPoint)[3], double *point1, double *point2)
{
	midPoint[X] = (point1[X] + point2[X]) / 2;
	midPoint[Y] = (point1[Y] + point2[Y]) / 2;
	midPoint[Z] = (point1[Z] + point2[Z]) / 2;
}
//----------------------------------------------------------------------------
double albaGeometryUtils::DistancePointToLine(double * point, double * lineP1, double * lineP2)
{
	return vtkLine::DistanceToLine(point, lineP1, lineP2);
}

//----------------------------------------------------------------------------
double albaGeometryUtils::GetAngle(double* point1, double* point2, double* origin, int plane) // Degree
{
	double angle = 0;
	double angleToP1, angleToP2;

	switch (plane)
	{
	case XY:
	{
		angleToP1 = atan2((point1[X] - origin[X]), (point1[Y] - origin[Y]));
		angleToP2 = atan2((point2[X] - origin[X]), (point2[Y] - origin[Y]));
	}
	break;
	case YZ:
	{
		angleToP1 = atan2((point1[Z] - origin[Z]), (point1[Y] - origin[Y]));
		angleToP2 = atan2((point2[Z] - origin[Z]), (point2[Y] - origin[Y]));
	}
	break;
	case XZ:
	{
		angleToP1 = atan2((point1[X] - origin[X]), (point1[Y] - origin[Y]));
		angleToP2 = atan2((point2[X] - origin[X]), (point2[Y] - origin[Y]));
	}
	break;
	}

	angle = angleToP2 - angleToP1;
	if (angle < 0) angle += (2 * vtkMath::Pi());

	return angle;
}
//----------------------------------------------------------------------------
double albaGeometryUtils::GetAngle(double point1[3], double point2[3]) // Degree
{
	//return std::acos(Dot(point1, point2) / (Mag(point1)*Mag(point2))) * 180.0 / vtkMath::Pi();

	double magnVect1 = (double)sqrt((point1[X] * point1[X]) + (point1[Y] * point1[Y]) + (point1[Z] * point1[Z]));
	double magnVect2 = (double)sqrt((point2[X] * point2[X]) + (point2[Y] * point2[Y]) + (point2[Z] * point2[Z]));
	double vectorsMagnitude = magnVect1 * magnVect2;
	double dotVect = point1[X] * point2[X] + point1[Y] * point2[Y] + point1[Z] * point2[Z];
	double angleVect = acos(dotVect / vectorsMagnitude);

	return angleVect * 180.0 / vtkMath::Pi();
}

//----------------------------------------------------------------------------
void albaGeometryUtils::RotatePoint(double *point, double *origin, double angle, int plane)
{
	double s = sin(angle);
	double c = cos(angle);

	switch (plane)
	{
	case XY:
	{
		// Translate point back to origin:
		point[X] -= origin[X];
		point[Y] -= origin[Y];

		// Rotate point
		double xnew = point[X] * c - point[Y] * s;
		double ynew = point[X] * s + point[Y] * c;

		// Translate point back:
		point[X] = xnew + origin[X];
		point[Y] = ynew + origin[Y];
		point[Z] = 0.0;
	}
	break;

	case YZ:
	{
		// Translate point back to origin:
		point[Z] -= origin[Z];
		point[Y] -= origin[Y];

		// Rotate point
		double znew = point[Z] * c - point[Y] * s;
		double ynew = point[Z] * s + point[Y] * c;

		// Translate point back:
		point[X] = 0.0;
		point[Y] = ynew + origin[Y];
		point[Z] = znew + origin[Z];
	}
	break;

	case XZ:
	{
		// Translate point back to origin:
		point[X] -= origin[X];
		point[Z] -= origin[Z];

		// Rotate point
		double xnew = point[X] * c - point[Z] * s;
		double znew = point[X] * s + point[Z] * c;

		// Translate point back:
		point[X] = xnew + origin[X];
		point[Y] = 0.0;
		point[Z] = znew + origin[Z];
	}
	break;
	}
}

/// Lines

//----------------------------------------------------------------------------
bool albaGeometryUtils::GetLineLineIntersection(double(&point)[3], double *line1Point1, double *line1Point2, double *line2Point1, double *line2Point2)
{
	// 	double u, v;
	// 	int val = vtkLine::Intersection(line1Point1, line1Point2, line2Point1, line2Point2, u, v);
	//vtkLine::IntersectWithLine()

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
int albaGeometryUtils::IntersectLineLine(double *l1p1, double *l1p2, double *l2p1, double *l2p2, double &perc)
{
	double x[3];
	double projXYZ[3];
	double l2Perc;

	double tol = EPSILON;

	if (vtkLine::Intersection(l1p1, l1p2, l2p1, l2p2, perc, l2Perc) == VTK_YES_INTERSECTION)
	{
		// make sure we are within tolerance
		for (int i = 0; i < 3; i++)
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


//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void albaGeometryUtils::rotAroundA(double *point, double *axis, float zdegree)
{
// 	double v1[3] = { 1.0f, 0.0f, 0.0f };
// 	double v2[3] = { 0.0f, 1.0f, 0.0f };
// 
// 	float xdegree = GetAngle(axis, v1);
// 	float ydegree = GetAngle(axis, v2);
// 
// 	rotAroundZ(point, xdegree);
// 	rotAroundY(point, ydegree);
// 	rotAroundZ(point, zdegree);
// 	rotAroundY(point, -ydegree);
// 	rotAroundZ(point, -xdegree);
}

/*
//----------------------------------------------------------------------------
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
