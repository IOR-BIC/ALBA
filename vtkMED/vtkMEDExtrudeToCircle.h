/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDExtrudeToCircle.h,v $
Language:  C++
Date:      $Date: 2008-04-09 09:14:17 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/


#ifndef __vtkMEDExtrudeToCircle_h
#define __vtkMEDExtrudeToCircle_h

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkMAFConfigure.h"



//------------------------------------------------------------------------------
/** vtkMEDExtrudeToCircle - Extrusion filter based on vtkLinearExtrusionFilter, 
This creates an extrusion terminating in a circle. */
//------------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMEDExtrudeToCircle : public vtkPolyDataToPolyDataFilter
{
public:
  vtkTypeRevisionMacro(vtkMEDExtrudeToCircle, vtkPolyDataToPolyDataFilter);
  static vtkMEDExtrudeToCircle *New();                                                   ///< New() method
  void PrintSelf(ostream& os, vtkIndent indent) const ;                                ///< print self

  void SetLength(double len) ;                        ///< Set the length of the extrusion
  void SetDirection(const double *direc) ;            ///< Set the direction of extrusion (does not set length)
  void SetExtrusionPoint(const double *extrPoint) ;   ///< Set the target point of extrusion (sets length and direction)
  void SetExtrusionVector(const double *extrVector) ; ///< Set the extrusion vector (length and direction)

  double GetLength() const {return m_length ; }
  void GetDirection(double *direc) const {CopyVector(m_direction, direc) ;}
  void GetExtrusionPoint(double *extrPt) const {CopyVector(m_extrusionPoint, extrPt) ;}
  void GetExtrusionVector(double *extrVec) const {CopyVector(m_extrusionVector, extrVec) ;}

  void GetHoleCentre(double *centre) const {CopyVector(m_holeCentre, centre) ;}          ///< Get centre of input hole
  void GetHoleNormal(double *normal) const {CopyVector(m_holeNormal, normal) ;}          ///< Get normal of input hole
  double GetHoleRadius() const {return m_holeRadius ;}                                   ///< Get radius of input hole

  double GetEndRadius() const {return m_endRadius ;}  ///< Get end radius of extrusion

  /** Get matrix to rotate arrow (x axis) to vector direction u */
  void GetMatRotArrowToAxis(vtkMatrix4x4 *mat, const double *u) const ;

protected:
  vtkMEDExtrudeToCircle() ;   ///< constructor
  ~vtkMEDExtrudeToCircle() ;  ///< deconstructor

  void Execute();       ///< execute method

  void Initialize() ;   ///< initialize filter (clear old data out for clean start)

  /** Calculate Centre, normal and radius of hole */
  void CalcHoleParameters() ;

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

   /** Calculate the target phi for an arbitrary (r, phi)
   Given the original hole points (r_hole, phi_hole) and their target phi's. */
  double CalculateTargetPhi(double *r, double *phi, double *r_hole, double *phi_hole, double *phi_target, int npts) const ;

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


  vtkPolyData *m_input ;
  vtkPolyData *m_output ;

  bool m_definedLength ;
  bool m_definedDirection ;
  bool m_definedVector ;
  bool m_definedExtrusionPoint ;
  double m_length ;             ///< length of extrusion
  double m_direction[3] ;       ///< direction of extrusion
  double m_extrusionPoint[3] ;  ///< target point of extrusion
  double m_extrusionVector[3] ; ///< extrusion vector (length and direction of extrusion)

  double m_holeCentre[3] ;      ///< centre of hole
  double m_holeNormal[3] ;      ///< normal of plane of hole
  double m_holeRadius ;         ///< r.m.s. radius of hole

  double m_endRadius ;          ///< end radius of extrusion

  vtkLinearExtrusionFilter *m_extrude ;

} ;

#endif
