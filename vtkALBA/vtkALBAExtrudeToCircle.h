/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAExtrudeToCircle
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __vtkALBAExtrudeToCircle_h
#define __vtkALBAExtrudeToCircle_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "albaConfigure.h"
#include <iostream>


//------------------------------------------------------------------------------
/// vtkALBAExtrudeToCircle. \n
/// Extrusion filter based on vtkLinearExtrusionFilter. \n
/// This creates an extrusion terminating in a circle.
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAExtrudeToCircle : public vtkPolyDataAlgorithm
{
public:
  //----------------------------------------------------------------------------
  // Public methods
  //----------------------------------------------------------------------------
  vtkTypeMacro(vtkALBAExtrudeToCircle, vtkPolyDataAlgorithm);
  static vtkALBAExtrudeToCircle *New();                  ///< New() method
  void PrintSelf(ostream& os, vtkIndent indent) const ; ///< print self

  void SetLength(double len) ;                        ///< Set the length of the extrusion
  void SetDirection(const double *direc) ;            ///< Set the direction of extrusion (does not have to be normalised)
  void SetDirection(double x, double y, double z) ;   ///< Set the direction of extrusion (does not have to be normalised)
  void SetExtrusionPoint(const double *extrPoint) ;   ///< Set the target point of extrusion
  void SetExtrusionVector(const double *extrVector) ; ///< Set the extrusion vector
  void SetMinNumberofEndPoints(int minNumEndPoints) ; ///< set min no. of vertices at end of extrusion

  /// Reset the extrusion direction to the default, which is normal to the hole. \n
  /// Use this to reverse the direction if the extrusion goes the wrong way. \n
  /// Argument is +1 or -1 to select forward or reverse direction relative to the hole.
  void SetDirectionToDefault(int directionSign) ;      

  double GetLength() const {return m_Length ; }
  void GetDirection(double *direc) const {CopyVector(m_Direction, direc) ;}
  void GetExtrusionPoint(double *extrPt) const {CopyVector(m_ExtrusionPoint, extrPt) ;}
  void GetExtrusionVector(double *extrVec) const {CopyVector(m_ExtrusionVector, extrVec) ;}

  void GetHoleCentre(double *centre) const {CopyVector(m_HoleCentre, centre) ;}          ///< Get centre of input hole
  void GetHoleNormal(double *normal) const {CopyVector(m_HoleNormal, normal) ;}          ///< Get normal of input hole
  double GetHoleRadius() const {return m_HoleRadius ;}                                   ///< Get radius of input hole
  int GetHoleNumVerts() const {return m_HoleNumVerts ;}                                  ///< Get number of points around hole

  double GetEndRadius() const {return m_EndRadius ;}  ///< Get end radius of extrusion

  /// Get matrix reqd to rotate arrow (x axis) to vector direction u
  void GetMatRotArrowToAxis(vtkMatrix4x4 *mat, const double *u) const ;

protected:
  //----------------------------------------------------------------------------
  // Protected methods
  //----------------------------------------------------------------------------
  vtkALBAExtrudeToCircle() ;   ///< constructor
  ~vtkALBAExtrudeToCircle() ;  ///< deconstructor

	/** Execute method */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  void Initialize() ;   ///< initialize filter (clear old data out for clean start)

  /// Calculate Centre, normal and radius of hole, and no. of vertices
  void CalcHoleParameters(vtkPolyData *hole) ;

  /// Calculate "up" vector which defines phi = 0 in cylindrical coords. \n
  /// It is the direction from the centre of the hole to the first vertex. \n
  /// This is not necessarily normal to the cylinder axis.  \n
  /// holepts is the list of point ids on the hole. \n
  void CalcUpVector(vtkIdList *holepts) ;

  /// Calculate vertex rings (z position and no. of vertices) along extrusion.  \n
  /// Allocates memory for mesh structure
  void CalcExtrusionRings() ;

  /// Calculate cylindrical and cartesian coords of each mesh vertex
  void CalcExtrusionVertices() ;

  /// Transfer vertices to vtkPoints
  void VerticesToVtkPoints(vtkPoints *pts) const ;

  /// Sort vertices into vtk triangles
  void VerticesToVtkTriangles(vtkCellArray *triangles) const ;

  /// Calculate the extrusion vector, once we know the centre and normal of the hole. \n
  /// This also calculates the length, direction and extrusion point. \n
  /// This pulls together all the possible routes for setting the extrusion vector. \n
  /// The default length is 1.0 and the default direction is the hole normal, if they have not been set.
  void CalculateExtrusionVector() ;

  /// Get cylindrical coords (r, phi, z) of point x. \n
  /// The cylinder system is defined by the central axis and an "up" vector, which defines phi = 0. \n
  /// cylAxis and upVector do not have to be normalised. \n
  /// upVector does not have to be exactly normal to cylAxis. \n
  /// phi is in the range 0 to 2pi.
  void CalcCylinderCoords(const double *x, const double *centre, const double *cylAxis, const double *upVector, double *r, double *phi, double *z) const ;

  /// Get cartesian coords x given cylindrical coords (r, phi, z). \n
  /// The cylinder system is defined by the central axis and an "up" vector, which defines phi = 0. \n
  /// cylAxis and upVector do not have to be normalised. \n
  /// upVector does not have to be exactly normal to cylAxis.
  void CalcCartesianCoords(double r, double phi, double z, const double *centre, const double *cylAxis, const double *upVector, double *x) const ;

  /// Get points in order around hole
  void GetPointsAroundHole(vtkPolyData *hole, vtkIdList *pts) const ;

  /// Checks sense of points as calculated by GetPointsAroundHole(). \n
  /// Returns 1 if phi increases with index. \n
  /// Returns -1 if phi decreases with index.
  int CalcSenseOfPointsAroundHole(vtkIdList *pts, const double *centre, const double *cylAxis, const double *upVector) const ;

  /// This makes sure that the values of phi on the first ring \n
  /// monotonically increase from near zero, \n
  /// otherwise we get can serious problems with 2pi when interpolating.
  void Remove2PiArtefactsFromFirstRing() ;

  /// Reverse id list
  void ReverseIdList(vtkIdList *pts) const ;

  /// Circular shift id list
  void CircularShiftIdList(vtkIdList *pts, int shift) const ;

  /// Modulo operator, same as % but works correctly on negative values of n as well
  inline int Modulo(int n, int m) const ;

  /// Copy vector b = a
  void CopyVector(const double *a, double *b) const ;

  /// Set vector
  void SetVector(double *vec, double x, double y, double z) const ;

  /// Add vector
  void AddVector(const double *vin, const double *x, double *vout) const ;

  /// Subtract vector
  void SubtractVector(const double *vin, const double *x, double *vout) const ;

  /// Multiply vector by scalar
  void MultVectorByScalar(double s, const double *vin, double *vout) const ;

  /// Get norm of vector
  double GetNorm(const double *vec) const ;

  /// Normalize vector
  void Normalize(const double *vin, double *vout) const ;

  /// Cross product of vectors a ^ b = c
  void CrossProduct(const double *a, const double *b, double *c) const ;

  /// Dot product of vectors a.b
  double DotProduct(const double *a, const double *b) const ;

  /// Inverse solution of equation x' = Mx, where M = row vectors (u, v and w).  \n
  /// Find x given M and x'.
  void InverseSolveTransform(const double *u, const double *v, const double *w, const double *y, double *x) const ;


  //----------------------------------------------------------------------------
  // Nested classes for mesh structure
  //----------------------------------------------------------------------------
  class VertexData ;
  class RingData ;
  class MeshData ;


  //----------------------------------------------------------------------------
  // Member variables
  //----------------------------------------------------------------------------
  vtkPolyData *m_Input ;
  vtkPolyData *m_Output ;

  bool m_DefinedLength ;          // Is length defined
  bool m_DefinedDirection ;       // Is direction defined
  bool m_DefinedVector ;          // Is vector defined
  bool m_DefinedExtrusionPoint ;  // Is extrusion point defined
  bool m_DefinedMinNumEndPts ;    // Is no. of end points defined
  bool m_BuiltCells ;             // Are polydata cell links built

  double m_Length ;             // length of extrusion
  double m_Direction[3] ;       // direction of extrusion
  double m_ExtrusionPoint[3] ;  // target point of extrusion
  double m_ExtrusionVector[3] ; // extrusion vector (length and direction of extrusion)
  int m_MinNumEndPts ;          // user requested min no. of vertices at end of extrusion
  int m_DefaultDirectionSign ;  // default direction: 1 for forwards, -1 for reverse

  double m_HoleCentre[3] ;      // centre of hole
  double m_HoleNormal[3] ;      // normal of plane of hole
  double m_HoleRadius ;         // r.m.s. radius of hole
  int m_HoleNumVerts ;          // no. of vertices around hole
  double m_UpVector[3] ;        // direction from centre of hole to first point

  double m_EndRadius ;          // end radius of extrusion

  MeshData* m_Mesh ; // mesh structure consisting of rings of vertices

  //Test classes of subclasses
  friend class RingDataTest;
  friend class MeshDataTest;
  friend class VertexDataTest;

  // Private subclass test friend
  friend class vtkALBAExtrudeToCircleVertexDataTest;

} ;


//------------------------------------------------------------------------------
/// vtkALBAExtrudeToCircle::VertexData. \n
/// Private subclass for vertex position and index in polydata.
//------------------------------------------------------------------------------
class vtkALBAExtrudeToCircle::VertexData{
public:
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  void SetCylCoords(const double *cyl) {for (int i=0 ; i<3 ; i++){Cylcoord[i]=cyl[i] ;}}
  void SetCylCoords(double r, double phi, double z) {Cylcoord[0]=r; Cylcoord[1]=phi; Cylcoord[2]=z;}
  void SetCylR(double r) {Cylcoord[0]=r;}
  void SetCylPhi(double phi) {Cylcoord[1]=phi;}
  void SetCylZ(double z) {Cylcoord[2]=z;}
  void SetCartCoords(const double *x) {for (int i=0 ; i<3 ; i++){Cartcoord[i]=x[i];}}
  void SetCartCoords(double x, double y, double z) {Cartcoord[0]=x; Cartcoord[1]=y; Cartcoord[2]=z;}
  void SetId(vtkIdType id) {PolydataId = id;}

  void GetCylCoords(double *cyl) const {for (int i=0 ; i<3 ; i++){cyl[i]=Cylcoord[i];}}
  void GetCylCoords(double *r, double *phi, double *z) const {*r=Cylcoord[0]; *phi=Cylcoord[1]; *z=Cylcoord[2];}
  double GetCylR() const {return Cylcoord[0];}
  double GetCylPhi() const {return Cylcoord[1];}
  double GetCylZ() const {return Cylcoord[2];}
  void GetCartCoords(double *x) const {for (int i=0 ; i<3 ; i++){x[i]=Cartcoord[i] ;}}
  vtkIdType GetId() const {return PolydataId;}

private:
  double Cylcoord[3] ;
  double Cartcoord[3] ;
  vtkIdType PolydataId ;
} ;


//------------------------------------------------------------------------------
/// vtkALBAExtrudeToCircle::RingData.
/// Private subclass for ring of vertices.
//------------------------------------------------------------------------------
class vtkALBAExtrudeToCircle::RingData{
public:
  ~RingData() {delete [] Vertex ;}
  void AllocateVertices(int n) {NumVerts = n ;  Vertex = new VertexData[n] ;}
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  double Z ;                        ///< position in cylindrical coords
  int NumVerts ;                    ///< no. of vertices in ring
  VertexData *Vertex ;              ///< array of vertices
} ;


//------------------------------------------------------------------------------
/// vtkALBAExtrudeToCircle::MeshData.
/// Private subclass for extrusion mesh
//------------------------------------------------------------------------------
class vtkALBAExtrudeToCircle::MeshData{
public:
  ~MeshData() {delete [] Ring ;}
  void AllocateRings(int n) {NumRings = n ;  Ring = new RingData[n] ;}
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  int NumRings ;              ///< no. of rings in mesh
  RingData *Ring ;            ///< array of rings
} ;



#endif
