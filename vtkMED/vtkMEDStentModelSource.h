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
  void setLinkLength(double value);
  void setStrutsNumber(int value);
  void setLinkConnection(LinkConnectionType value);
  void setStentConfiguration(StentConfigurationType value);
  void setLinkOrientation(LinkOrientationType value);
  void setLinkAlignment(unsigned int value);
  void setCrownNumber(int value);
  void setLinkNumber(int value);
  void setLinkInterlace(int value);
  void setCenterLine (std::vector<std::vector<double> >::const_iterator centerLineStart,
    std::vector<std::vector<double> >::const_iterator centerLineEnd, int pointNumber);
  void setCenterLineForInphaseShort(std::vector<std::vector<double> >::const_iterator centerLineStart,
    std::vector<std::vector<double> >::const_iterator centerLineEnd, int pointNumber);
  void setCenterLineForBardHelical(std::vector<std::vector<double> >::const_iterator centerLineStart,
    std::vector<std::vector<double> >::const_iterator centerLineEnd);
  /// Set the center line of the stent from the given line.
  void setCenterLineFromPolyData(vtkPolyData *polyData);
  void setTestValue(){};
  void setStentType(int value);

  /// Set angle and calc strut length. Argument is radians. \n
  /// Must set crown length first.
  void setStrutAngle(double theta); 


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

  inline SimplexMeshType::Pointer GetSimplexMesh() {return m_SimplexMesh ;}


  //-----------------------
  // Create stents
  //-----------------------
  void createStent(); ///< Beds original example stent
  void createStentInphaseShort();  ///< Abbott inphase, short links
  void createStentBard(); ///< No use!
  void createStentBardHelical(); ///< Bard helical


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

  //-----------------------
  // Beds example stent
  //-----------------------
  void createStentSimplexMesh();
  void createStruts();
  void createLinks();
  void init();  ///< prepare a default center line


  //------------------------------
  // Abbott in-phase, short links
  // link < crown length
  //------------------------------
  void createStrutsShort();
  void createLinksShort();
  void initShort();


  //-----------------------
  // Bard stent
  //-----------------------
  void createBardStentSimplexMesh();
  void createBardStruts();
  void getUVVector( double * UVector, double * normalCircle, double * VVector );
  void getBardNormalCircle( double *startPoint, double *endPoint, double *samplePoint,double theta);
  void initBard();


  //-----------------------
  // Bard helical stent
  //-----------------------
  void initBardHelical();
  void createBardHelicalStentSimplexMesh();
  void createBardHelicalStentSimplexMesh2();
  void createBardStruts2();
  void createBardLinks();
  void NeighborLRDown( int i, int sampleNumberPerCircle, int j );
  void NeighborUpRL( int i, int sampleNumberPerCircle, int j );
  void createBardHelicalStruts();





  //bool crownAdded;

  /// Find sampling point on segment of center line. \n
  /// Original recursive version (deprecated).
  void calculateSamplingPoint(double *preSamplePoint, double* samplePoint, 
    double distance, double* left, double* right);

  /// Find sampling point on segment of center line. \n
  /// Direct calculation. \n
  /// Returns false if intersection point is outside the line segment.
  bool calculateSamplingPoint2(double *preSamplePoint, double* samplePoint, 
    double distance, double* left, double* right);

  /** used to calculate stent length */
  double calculateDistanceBetweenCrown();

  void getMiddlePoint(double *start, double *end,double *middle);
  void getOffsetVectorWithScale(const double *start, const double *end, const double scale, const double *org, double offsetVec[3]) const;


  /* stent parameters */
  double m_StentDiameter;
  double m_CrownLength;
  double m_StrutLength;
  double m_StrutAngle;  ///< angle in radians
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