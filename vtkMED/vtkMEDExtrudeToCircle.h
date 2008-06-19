/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDExtrudeToCircle.h,v $
Language:  C++
Date:      $Date: 2008-06-19 08:46:32 $
Version:   $Revision: 1.4 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2008 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


#ifndef __vtkMEDExtrudeToCircle_h
#define __vtkMEDExtrudeToCircle_h

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkMAFConfigure.h"
#include <iostream>


//------------------------------------------------------------------------------
/** vtkMEDExtrudeToCircle - Extrusion filter based on vtkLinearExtrusionFilter, 
This creates an extrusion terminating in a circle. */
//------------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMEDExtrudeToCircle : public vtkPolyDataToPolyDataFilter
{
public:
  //----------------------------------------------------------------------------
  // Public methods
  //----------------------------------------------------------------------------
  vtkTypeRevisionMacro(vtkMEDExtrudeToCircle, vtkPolyDataToPolyDataFilter);
  static vtkMEDExtrudeToCircle *New();                                                   ///< New() method
  void PrintSelf(ostream& os, vtkIndent indent) const ;                            ///< print self

  void SetLength(double len) ;                        ///< Set the length of the extrusion
  void SetDirection(const double *direc) ;            ///< Set the direction of extrusion (does not have to be normalised)
  void SetDirection(double x, double y, double z) ;   ///< Set the direction of extrusion (does not have to be normalised)
  void SetExtrusionPoint(const double *extrPoint) ;   ///< Set the target point of extrusion
  void SetExtrusionVector(const double *extrVector) ; ///< Set the extrusion vector
  void SetMinNumberofEndPoints(int minNumEndPoints) ; ///< set min no. of vertices at end of extrusion

  /** Reset the extrusion direction to the default, which is normal to the hole.
  Use this to reverse the direction if the extrusion goes the wrong way.
  Argument is +1 or -1 to select forward or reverse direction relative to the hole. */
  void SetDirectionToDefault(int directionSign) ;      

  double GetLength() const {return m_length ; }
  void GetDirection(double *direc) const {CopyVector(m_direction, direc) ;}
  void GetExtrusionPoint(double *extrPt) const {CopyVector(m_extrusionPoint, extrPt) ;}
  void GetExtrusionVector(double *extrVec) const {CopyVector(m_extrusionVector, extrVec) ;}

  void GetHoleCentre(double *centre) const {CopyVector(m_holeCentre, centre) ;}          ///< Get centre of input hole
  void GetHoleNormal(double *normal) const {CopyVector(m_holeNormal, normal) ;}          ///< Get normal of input hole
  double GetHoleRadius() const {return m_holeRadius ;}                                   ///< Get radius of input hole
  int GetHoleNumVerts() const {return m_holeNumVerts ;}                                        ///< Get number of points around hole

  double GetEndRadius() const {return m_endRadius ;}  ///< Get end radius of extrusion

  /** Get matrix reqd to rotate arrow (x axis) to vector direction u */
  void GetMatRotArrowToAxis(vtkMatrix4x4 *mat, const double *u) const ;

protected:
  //----------------------------------------------------------------------------
  // Protected methods
  //----------------------------------------------------------------------------
  vtkMEDExtrudeToCircle() ;   ///< constructor
  ~vtkMEDExtrudeToCircle() ;  ///< deconstructor

  void Execute();       ///< execute method

  void Initialize() ;   ///< initialize filter (clear old data out for clean start)

  /** Calculate Centre, normal and radius of hole, and no. of vertices */
  void CalcHoleParameters(vtkPolyData *hole) ;

  /** Calculate "up" vector which defines phi = 0 in cylindrical coords
  It is the direction from the centre of the hole to the first vertex.
  This is not necessarily normal to the cylinder axis. 
  holepts is the list of point ids on the hole. */
  void CalcUpVector(vtkIdList *holepts) ;

  /** Calculate vertex rings (z position and no. of vertices) along extrusion 
  Allocates memory for mesh structure */
  void CalcExtrusionRings() ;

  /** Calculate cylindrical and cartesian coords of each mesh vertex */
  void CalcExtrusionVertices() ;

  /** Transfer vertices to vtkPoints */
  void VerticesToVtkPoints(vtkPoints *pts) const ;

  /** Sort vertices into vtk triangles */
  void VerticesToVtkTriangles(vtkCellArray *triangles) const ;

  /** Calculate the extrusion vector, once we know the centre and normal of the hole.
  This also calculates the length, direction and extrusion point.
  This pulls together all the possible routes for setting the extrusion vector.
  The default length is 1.0 and the default direction is the hole normal, if they have not been set. */
  void CalculateExtrusionVector() ;

  /** Get cylindrical coords (r, phi, z) of point x.
  The cylinder system is defined by the central axis and an "up" vector, which defines phi = 0
  cylAxis and upVector do not have to be normalised.
  upVector does not have to be exactly normal to cylAxis.
  phi is in the range 0 to 2pi. */
  void CalcCylinderCoords(const double *x, const double *centre, const double *cylAxis, const double *upVector, double *r, double *phi, double *z) const ;

  /** Get cartesian coords x given cylindrical coords (r, phi, z)
  The cylinder system is defined by the central axis and an "up" vector, which defines phi = 0
  cylAxis and upVector do not have to be normalised.
  upVector does not have to be exactly normal to cylAxis. */
  void CalcCartesianCoords(double r, double phi, double z, const double *centre, const double *cylAxis, const double *upVector, double *x) const ;

  /** Get points in order around hole */
  void GetPointsAroundHole(vtkPolyData *hole, vtkIdList *pts) const ;

  /** Checks sense of points as calculated by GetPointsAroundHole()
  Returns 1 if phi increases with index
  Returns -1 if phi decreases with index */
  int CalcSenseOfPointsAroundHole(vtkIdList *pts, const double *centre, const double *cylAxis, const double *upVector) const ;

  /** This makes sure that the values of phi on the first ring
  monotonically increase from near zero
  Otherwise we get can serious problems with 2pi when interpolating. */
  void Remove2PiArtefactsFromFirstRing() ;

  /** Reverse id list */
  void ReverseIdList(vtkIdList *pts) const ;

  /** Circular shift id list */
  void CircularShiftIdList(vtkIdList *pts, int shift) const ;

  /** Modulo operator, same as % but works correctly on negative values of n as well */
  inline int Modulo(int n, int m) const ;

  /** Copy vector b = a */
  void CopyVector(const double *a, double *b) const ;

  /** Set vector */
  void SetVector(double *vec, double x, double y, double z) const ;

  /** Add vector */
  void AddVector(const double *vin, const double *x, double *vout) const ;

  /** Subtract vector */
  void SubtractVector(const double *vin, const double *x, double *vout) const ;

  /** Multiply vector by scalar */
  void MultVectorByScalar(double s, const double *vin, double *vout) const ;

  /** Get norm of vector */
  double GetNorm(const double *vec) const ;

  /** Normalize vector */
  void Normalize(const double *vin, double *vout) const ;

  /** Cross product of vectors a ^ b = c */
  void CrossProduct(const double *a, const double *b, double *c) const ;

  /** Dot product of vectors a.b */
  double DotProduct(const double *a, const double *b) const ;

  /** Inverse solution of equation x' = Mx, where M = row vectors (u, v and w)
  Find x given M and x' */
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
  vtkPolyData *m_input ;
  vtkPolyData *m_output ;

  bool m_definedLength ;          // Is length defined
  bool m_definedDirection ;       // Is direction defined
  bool m_definedVector ;          // Is vector defined
  bool m_definedExtrusionPoint ;  // Is extrusion point defined
  bool m_definedMinNumEndPts ;    // Is no. of end points defined
  bool m_builtCells ;             // Are polydata cell links built

  double m_length ;             // length of extrusion
  double m_direction[3] ;       // direction of extrusion
  double m_extrusionPoint[3] ;  // target point of extrusion
  double m_extrusionVector[3] ; // extrusion vector (length and direction of extrusion)
  int m_minNumEndPts ;          // user requested min no. of vertices at end of extrusion
  int m_defaultDirectionSign ;  // default direction: 1 for forwards, -1 for reverse

  double m_holeCentre[3] ;      // centre of hole
  double m_holeNormal[3] ;      // normal of plane of hole
  double m_holeRadius ;         // r.m.s. radius of hole
  int m_holeNumVerts ;          // no. of vertices around hole
  double m_upVector[3] ;        // direction from centre of hole to first point

  double m_endRadius ;          // end radius of extrusion

  MeshData* m_mesh ; // mesh structure consisting of rings of vertices

} ;



//------------------------------------------------------------------------------
// subclass for vertex position and index in polydata
//------------------------------------------------------------------------------
class vtkMEDExtrudeToCircle::VertexData{
public:
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  void SetCylCoords(const double *cyl) {for (int i=0 ; i<3 ; i++){m_cylcoord[i]=cyl[i] ;}}
  void SetCylCoords(double r, double phi, double z) {m_cylcoord[0]=r; m_cylcoord[1]=phi; m_cylcoord[2]=z;}
  void SetCylR(double r) {m_cylcoord[0]=r;}
  void SetCylPhi(double phi) {m_cylcoord[1]=phi;}
  void SetCylZ(double z) {m_cylcoord[2]=z;}
  void SetCartCoords(const double *x) {for (int i=0 ; i<3 ; i++){m_cartcoord[i]=x[i];}}
  void SetCartCoords(double x, double y, double z) {m_cartcoord[0]=x; m_cartcoord[1]=y; m_cartcoord[2]=z;}
  void SetId(vtkIdType id) {m_polydataId = id;}

  void GetCylCoords(double *cyl) const {for (int i=0 ; i<3 ; i++){cyl[i]=m_cylcoord[i];}}
  void GetCylCoords(double *r, double *phi, double *z) const {*r=m_cylcoord[0]; *phi=m_cylcoord[1]; *z=m_cylcoord[2];}
  double GetCylR() const {return m_cylcoord[0];}
  double GetCylPhi() const {return m_cylcoord[1];}
  double GetCylZ() const {return m_cylcoord[2];}
  void GetCartCoords(double *x) const {for (int i=0 ; i<3 ; i++){x[i]=m_cartcoord[i] ;}}
  vtkIdType GetId() const {return m_polydataId;}

private:
  double m_cylcoord[3] ;
  double m_cartcoord[3] ;
  vtkIdType m_polydataId ;
} ;


//------------------------------------------------------------------------------
// subclass for ring of vertices
//------------------------------------------------------------------------------
class vtkMEDExtrudeToCircle::RingData{
public:
  ~RingData() {delete [] vertex ;}
  void AllocateVertices(int n) {numVerts = n ;  vertex = new VertexData[n] ;}
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  double z ;                        // position in cylindrical coords
  int numVerts ;                    // no. of vertices in ring
  VertexData *vertex ;              // array of vertices
} ;


//------------------------------------------------------------------------------
// subclass for extrusion mesh
//------------------------------------------------------------------------------
class vtkMEDExtrudeToCircle::MeshData{
public:
  ~MeshData() {delete [] ring ;}
  void AllocateRings(int n) {numRings = n ;  ring = new RingData[n] ;}
  void PrintSelf(ostream& os, vtkIndent indent) const ;

  int numRings ;              // no. of rings in mesh
  RingData *ring ;            // array of rings
} ;

#endif
