#pragma once

#include <iostream>

#include "itkSimplexMesh.h"
#include "itkDefaultDynamicMeshTraits.h"

#include "itkSimplexMeshToTriangleMeshFilter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"

#ifndef VECTORITEM_H
#define VECTORITEM_H
#include <vector>
using std::vector;
#define Pi 3.14159265
#endif

class StentModelSource
{

public:
	typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double, double> MeshTraits;
	typedef itk::SimplexMesh<double,3,MeshTraits>           SimplexMeshType;
	typedef itk::SimplexMeshGeometry                        SimplexMeshGeometryType;
	typedef SimplexMeshType::CellType       CellType;
	typedef CellType::CellAutoPointer       CellAutoPointer;
	typedef SimplexMeshType::PointType      PointType;
	typedef SimplexMeshType::CellIdentifier        CellIdentifier;
	typedef SimplexMeshType::PointIdentifier       PointIdentifier;
	typedef SimplexMeshType::CellFeatureIdentifier CellFeatureIdentifier;
	typedef SimplexMeshType::CellAutoPointer       OutputCellAutoPointer;
	typedef SimplexMeshType::CellType              OutputCellType;
	typedef itk::PolygonCell< OutputCellType >     OutputPolygonType;
	

	enum LinkConnectionType {peak2valley, valley2peak, peak2peak, valley2valley};
	enum StentConfigurationType {InPhase, OutOfPhase};
	enum LinkOrientationType {None, PositiveOne, NegativeOne};

	StentModelSource(void);
	~StentModelSource(void);
	
	void setCenterLinePolydata(vtkPolyData *line);

	void setStentDiameter(double value);
	void setCrownLength(double value);
	//void setStrutAngle(double value); //move to private section
	void setLinkLength(double value);
	void setStrutsNumber(int value);
	void setLinkConnection(LinkConnectionType value);
	void setStentConfiguration(StentConfigurationType value);
	void setLinkOrientation(LinkOrientationType value);
	void setLinkAlignment(unsigned int value);
	void setCrownNumber(int value);
	void setLinkNumber(int value);

	void setStartPosition(double *value);
	void setDirection(double *value);

	void setCenterLine (double value[][3]);
	double getStrutLength();
	double getLinkLength();

	void createStent();

	SimplexMeshType::Pointer simplexMesh; 
	struct Strut{
		PointIdentifier startVertex;
		PointIdentifier endVertex;
	};
	vector<Strut> strutsList;
	vector<Strut> linkList;

	/*struct Point3D{
		double x;
		double y;
		double z;
	};
	vector<Point3D> pointList;*/
	
private:
	//stent parameters
	double stentDiameter;
	double crownLength;
	double strutAngle;
    double linkLength;
	int strutsNumber;
	int crownNumber;
	int linkNumber;
	LinkConnectionType linkConnection;
	StentConfigurationType stentConfiguration;
	LinkOrientationType linkOrientation;
	unsigned int linkAlignment;

	//stent deplyment position
	double startPosition[3];
	double direction[3];

	double (*centerLine)[3];
	int nCenterVertex;
	int nCrownSimplex;
	
	double strutLength;
	vtkPolyData *m_centerline;
	//void setStrutAngle(double value);
	void setStrutAngle(double value);


	void init();

	bool crownAdded;

	void createStentSimplexMesh();
	void createStruts();
	void createLinks();
};
