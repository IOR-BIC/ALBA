/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaGeometryUtils.h
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

#ifndef __albaGeometryUtils_h
#define __albaGeometryUtils_h

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------



/**
  Class Name: albaWizard.
  Class for wizard creation each wizard must extend this block
*/
class albaGeometryUtils
{
protected:
	enum { X, Y, Z, };

public:
	
	static double Dot(double *point1, double *point2);
	static double Mag(double *point1);

	static float Norm2(double *point1);
	static double Norm(double *point1);

	static void Cross(double *point1, double *point2, double* cross);


	static bool Equal(double *point1, double *point2);

	static double DistanceBetweenPoints(double *point1, double *point2);

	static void GetMidPoint(double(&midPoint)[3], double *point1, double *point2);

	static double GetAngle(double point1[3], double point2[3]);
	static double GetAngle(double* point1, double* point2, double* origin);


	static double DistancePointToLine(double * point, double * lineP1, double * lineP2);
	//static double GetPointToLineDistance(double *point, double *linePoint1, double *linePoint2);
	//static float DistancePointToLine(double *point, double *lineP1, double *lineP2);

	static void RotatePoint(double *point, double *origin, double angle);


	static bool GetLineLineIntersection(double(&point)[3], double *line1Point1, double *line1Point2, double *line2Point1, double *line2Point2);


	/// Lines Utils

	//static bool FindIntersectionLines(double(&point)[3], double *line1Point1, double *line1Point2, double *line2Point1, double *line2Point2);
	static int IntersectLineLine(double *l1p1, double *l1p2, double *l2p1, double *l2p2, double &perc);




	static void GetParallelLine(double(&point1)[3], double(&point2)[3], double *linePoint1, double *linePoint2, double distance);

	static bool FindPointOnLine(double(&point)[3], double *linePoint1, double *linePoint2, double distance);

	//Returns 1 if the point is up, -1 if is down and 0 if the point is in the line
	static int PointUpDownLine(double *point, double *lp1, double *lp2);


	static void rotAroundZ(double *point, float degree);
	static void rotAroundY(double *point, float degree);
	static void rotAroundA(double *point, double *axis, float zdegree);

};
#endif
