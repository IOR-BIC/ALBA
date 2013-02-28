#pragma once


/*=========================================================================
  Program:   MAF2Medical
  Module:    $RCSfile: StentModelSource.h,v $
  Language:  C++
  Date:      $Date: 2012-Nov 10:15:31 $
  Version:   $Revision: 1.1.2.7 $
  Authors:   Xiangyin Ma, Hui Wei
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include <iostream>

#include "itkSimplexMesh.h"
#include "itkDefaultDynamicMeshTraits.h"

#include "itkSimplexMeshToTriangleMeshFilter.h"

#include "vtkMEDConfigure.h"

#ifndef VECTORITEM_H
#define VECTORITEM_H
#include <vector>




using std::vector;
  // #define Pi 3.14159265
#endif

/**----------------------------------------------------------------------------*/
// forward declarations :
/**----------------------------------------------------------------------------*/
class vtkPolyData;

/**
*---------------------------------------
* Stent Model Creation
*--------------------------------------
* A stent is a series of crowns joined by links. 
* use a standard simplex as control mesh, 
* and use a subset of simplex vertices to serve as extremities
* of struts and links by connecting them 
* to include geometric information 
*---
*1: vertices are sampled along circles with diameters equal to stent diameter.  
*   These circles are perpendicular to the centreline
*   and their centres are located on the vessel centre line 
*   with intervals successively equal to crown length and length between crowns. 
*2: To sample the vertices, the density is double of  strut number per crown.
*3: All these vertices all connected following the simplex constraints, 
*   thus the control mesh is available.
*4: connect a subset of vertices to form the struts and links
*---
* To adpat different configuration of stent, 
* the control mesh have two more circles of vertices  in each side than the stent.
*---------------------------------------
*/
class VTK_vtkMED_EXPORT vtkMEDStentModelSource
{

public:
	/**----------------------------------------------------------------------------*/
	//--------typedef----------
	/**----------------------------------------------------------------------------*/
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
	
	/**----------------------------------------------------------------------------*/
	//-------enum---------
	/**----------------------------------------------------------------------------*/
	enum LinkConnectionType {peak2valley, valley2peak, peak2peak, valley2valley};
	enum StentConfigurationType {InPhase, OutOfPhase};
	enum LinkOrientationType {None, PositiveOne, NegativeOne};

	vtkMEDStentModelSource(void);
	~vtkMEDStentModelSource(void);
	/**----------------------------------------------------------------------------*/
	//--------setters----------
	/**----------------------------------------------------------------------------*/
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
	void setCenterLine (vector<vector<double>>::const_iterator centerLineStart,
						vector<vector<double>>::const_iterator centerLindEnd, int pointNumber);
	void setCenterLineFromPolyData(vtkPolyData *polyData);
	void setTestValue(){};
	
	/**----------------------------------------------------------------------------*/
	//--------getters----------
	/**----------------------------------------------------------------------------*/
	double getStrutLength();
	double getLinkLength();
	double getRadius();
	double getInitialStentLength();
	int getCrownNumber();
	int getStrutsNumber();

	/**-------create a stent from exist parameters ------------*/
	void createStent();

	SimplexMeshType::Pointer simplexMesh; 
	struct Strut{
		PointIdentifier startVertex;
		PointIdentifier endVertex;
	};
	vector<Strut> strutsList;
	vector<Strut> linkList;
	
	 /************************************************************************/
	 /* for every simplex vertex, centerLocationIdx stores the corresponding centerline position*/
	 /* this information is recorded during the simplex mesh & stent model creation*/
	 /************************************************************************/
	vector<int> centerLocationIndex;
	int computeCrownNumberAfterSetCenterLine();


private:
	/* stent parameters */
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

	/* create simplex mesh*/
	void createStentSimplexMesh();
	/* create struts*/
	void createStruts();
	/* create links between crown*/
	void createLinks();
	/* prepare a default center line */
	void init();


	double (*centerLine)[3];
	int nCenterVertex;
	int nCrownSimplex;

	double initialStentLength;	
	double strutLength;
	double centerLineLength;
	void setStrutAngle(double value); //not needed



	//bool crownAdded;

	void calculateSamplingPoint(double *preSamplePoint, double* samplePoint, 
								double distance, double* left, double* right);

	/** used to calculate stent length */
	double calculateDistanceBetweenCrown();
};
