/*=========================================================================
Program:   MAF2Medical
Module:    $RCSfile: StentModelSource.h,v $
Language:  C++
Date:      $Date: 2012-Nov 10:15:31 $
Version:   $Revision: 1.1.2.7 $
Authors:   Xiangyin Ma, Hui Wei
==========================================================================
Copyright (c) 2013 
University of Bedfordshire, UK
=========================================================================*/

#ifndef __medVMEStentModelSource_h 
#define __medVMEStentModelSource_h


#include "itkSimplexMesh.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkSimplexMeshToTriangleMeshFilter.h"
#include "vtkPolyData.h"
#include <vector>
#include <iostream>

// #define Pi 3.14159265



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
* To adapt different configuration of stent, 
* the control mesh have two more circles of vertices  in each side than the stent.
*---------------------------------------
*/
class vtkMEDStentModelSource
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

  vtkMEDStentModelSource(void); ///< Constructor
  ~vtkMEDStentModelSource(void); ///< Destructor

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
  void setLinkInterlace(int value);
  void setCenterLine (std::vector<std::vector<double> >::const_iterator m_CenterLineStart,
    std::vector<std::vector<double> >::const_iterator centerLindEnd, int pointNumber);
  void setCenterLineForInphaseShort(std::vector<std::vector<double> >::const_iterator m_CenterLineStart,
    std::vector<std::vector<double> >::const_iterator centerLindEnd, int pointNumber);
  void setCenterLineForBardHelical(std::vector<std::vector<double> >::const_iterator m_CenterLineStart,
    std::vector<std::vector<double> >::const_iterator centerLindEnd);
  /// Set the center line of the stent from the given line.
  void setCenterLineFromPolyData(vtkPolyData *polyData);
  void setTestValue(){};
  void setStentType(int value);

  /**----------------------------------------------------------------------------*/
  //--------getters----------
  /**----------------------------------------------------------------------------*/
  double getStrutLength();
  double getLinkLength();
  double getRadius();
  double getInitialStentLength();
  int getCrownNumber();
  int getStrutsNumber();
  int getLinkInterlace();
  int getInphaseShort();
  int getStentType();

  /**-------create a stent from exist parameters ------------*/
  void createStent();
  void createStentInphaseShort();
  void createStentBard();
  void createStentBardHelical();

  inline SimplexMeshType::Pointer GetSimplexMesh() {return m_SimplexMesh ;}

  struct Strut{
    PointIdentifier startVertex;
    PointIdentifier endVertex;
  };

  inline std::vector<Strut>& GetStrutsList() {return m_StrutsList ;}
  inline std::vector<Strut>& GetLinksList() {return m_LinksList ;}
  inline std::vector<Strut>& GetMeshList() {return m_MeshList ;}


  /************************************************************************/
  /* for every simplex vertex, centerLocationIdx stores the corresponding centerline position*/
  /* this information is recorded during the simplex mesh & stent model creation*/
  /************************************************************************/
  std::vector<int> centerLocationIndex;
  //int computeCrownNumberAfterSetCenterLine();
  //void getBardNormalCircle( double theta);
  void getShortStrutsLines(int* tindices,double middlePoints[][3]);//double  (&middlePoints)[4][3] );
  //void caculateShortStrutsLines(int* tindices, double middlePoints[][3] );

private:
  /// Allocate or reallocate centerline. \n
  /// This may destroy any existing data.
  void AllocateCentreLine(int n) ;

  /* create simplex mesh*/
  void createStentSimplexMesh();
  /* create struts*/
  void createStruts();
  /* create links between crown*/
  void createLinks();
  /* prepare a default center line */
  void init();
  //---------short begin--------------
  /*methods for in phase link length< crown length*/
  /* create simplex mesh*/
  //void createStentSimplexMeshShort();
  /* create struts*/
  void createStrutsShort();
  /* create links between crown*/
  void createLinksShort();
  /* prepare a default center line */
  void initShort();

  //----------short over--------------
  //----------Bard stent----------#
  void createBardStentSimplexMesh();
  /* create struts*/
  void createBardStruts();

  void getUVVector( double * UVector, double * normalCircle, double * VVector );

  void getBardNormalCircle( double *startPoint, double *endPoint, double *samplePoint,double theta);
  void initBard();


  //---------Bard helical----------

  void initBardHelical();
  void createBardHelicalStentSimplexMesh();
  void createBardHelicalStentSimplexMesh2();
  void createBardStruts2();
  void createBardLinks();

  void NeighborLRDown( int i, int sampleNumberPerCircle, int j );

  void NeighborUpRL( int i, int sampleNumberPerCircle, int j );

  void createBardHelicalStruts();

  //---------Bard helical-----------


  void setStrutAngle(double value); //not needed



  //bool crownAdded;

  void calculateSamplingPoint(double *preSamplePoint, double* samplePoint, 
    double distance, double* left, double* right);

  /** used to calculate stent length */
  double calculateDistanceBetweenCrown();

  void getMiddlePoint(double *start, double *end,double *middle);
  void getOffsetVectorWithScale(const double *start, const double *end, const double scale, const double *org, double offsetVec[3]) const;


  /* stent parameters */
  double m_StentDiameter;
  double m_CrownLength;
  double m_StrutLength;
  double m_StrutAngle;
  double m_LinkLength;
  int m_strutsNumber;
  int m_CrownNumber;
  int m_LinkNumber;
  int m_LinkInterlace;
  int m_StentType;
  LinkConnectionType m_LinkConnection;
  StentConfigurationType m_StentConfiguration;
  LinkOrientationType m_LinkOrientation;
  unsigned int m_LinkAlignment;

  double (*m_CenterLine)[3];
  int m_NumberOfPointsAllocatedToCenterLine ;
  int m_NumCenterVertex;
  int m_NumCrownSimplex;

  double m_InitialStentLength;	
  double m_CenterLineLength;

  double (*m_SampleArray)[3];

  SimplexMeshType::Pointer m_SimplexMesh; 
  std::vector<Strut> m_StrutsList;
  std::vector<Strut> m_LinksList;
  std::vector<Strut> m_MeshList;//used for visualize m_SimplexMesh;
};


#endif