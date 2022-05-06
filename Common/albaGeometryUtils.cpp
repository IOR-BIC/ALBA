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
double albaGeometryUtils::DistancePointToLine(double * point, double * lineP1, double * lineP2, int plane)
{
	return sqrt(vtkLine::DistanceToLine(point, lineP1, lineP2));
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
		angleToP1 = atan2((point1[Y] - origin[Y]), (point1[Z] - origin[Z]));
		angleToP2 = atan2((point2[Y] - origin[Y]), (point2[Z] - origin[Z]));
	}
	break;
	case XZ:
	{
		angleToP1 = atan2((point1[X] - origin[X]), (point1[Z] - origin[Z]));
		angleToP2 = atan2((point2[X] - origin[X]), (point2[Z] - origin[Z]));
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
void albaGeometryUtils::GetParallelLine(double(&point1)[3], double(&point2)[3], double *linePoint1, double *linePoint2, double distance, int plane)
{
	int A = 0, B = 1, C = 2;

	if (plane == XY) { A = 0; B = 1; C = 2;	};
	if (plane == YZ) { A = 1; B = 2; C = 0; };
	if (plane == XZ) { A = 0; B = 2; C = 1; };

	double L = sqrt(pow((linePoint2[A] - linePoint1[A]), 2) + pow((linePoint2[B] - linePoint1[B]), 2));

	point1[A] = linePoint1[A] + distance * (linePoint2[B] - linePoint1[B]) / L;
	point1[B] = linePoint1[B] + distance * (linePoint1[A] - linePoint2[A]) / L;
	point1[C] = linePoint1[C]; // 0.0;

	point2[A] = linePoint2[A] + distance * (linePoint2[B] - linePoint1[B]) / L;
	point2[B] = linePoint2[B] + distance * (linePoint1[A] - linePoint2[A]) / L;
	point2[C] = linePoint2[C]; // 0.0;
}
//----------------------------------------------------------------------------
bool albaGeometryUtils::FindPointOnLine(double(&point)[3], double *linePoint1, double *linePoint2, double distance, int plane)
{
	int A = 0, B = 1, C = 2;

	if (plane == XY) { A = 0; B = 1; C = 2; };
	if (plane == YZ) { A = 1; B = 2; C = 0; };
	if (plane == XZ) { A = 0; B = 2; C = 1; };

	double L = sqrt(pow((linePoint2[A] - linePoint1[A]), 2) + pow((linePoint2[B] - linePoint1[B]), 2));
	double dist_ratio = distance / L;

	point[A] = (1 - dist_ratio)*linePoint1[A] + dist_ratio * linePoint2[A];
	point[B] = (1 - dist_ratio)*linePoint1[B] + dist_ratio * linePoint2[B];
	point[C] = linePoint1[C];// 0.0;

	return (dist_ratio > 0 && dist_ratio < 1); //the point is on the line.
}

//----------------------------------------------------------------------------
int albaGeometryUtils::PointUpDownLine(double *point, double *lp1, double *lp2, int plane)
{
	int A = 0, B = 1, C = 2;

	if (plane == YZ) { A = 0; B = 1; C = 2; };
	if (plane == YZ) { A = 1; B = 2; C = 0; };
	if (plane == XZ) { A = 0; B = 2; C = 1; };

	double d = (point[A] - lp1[A]) * (lp2[B] - lp1[B]) - (point[B] - lp1[B]) * (lp2[A] - lp1[A]);

	if (d > 0)
		return 1;
	else if (d < 0)
		return -1;
	else
		return 0;
}

