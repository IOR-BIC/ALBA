/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDExtrudeToCircle.cxx,v $
Language:  C++
Date:      $Date: 2008-04-09 08:21:17 $
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


#include "mafDefines.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkObjectFactory.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkIdList.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkMEDPastValuesList.h"
#include "vtkMEDExtrudeToCircle.h"
#include <assert.h>




//------------------------------------------------------------------------------
// standard macros
vtkCxxRevisionMacro(vtkMEDExtrudeToCircle, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMEDExtrudeToCircle);
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Constructor
vtkMEDExtrudeToCircle::vtkMEDExtrudeToCircle() : m_definedLength(false), m_definedDirection(false), m_definedVector(false),
m_definedExtrusionPoint(false)
//------------------------------------------------------------------------------
{
  m_extrude = vtkLinearExtrusionFilter::New() ;

}


//------------------------------------------------------------------------------
// Destructor
vtkMEDExtrudeToCircle::~vtkMEDExtrudeToCircle()
//------------------------------------------------------------------------------
{
  m_extrude->Delete() ;
}



//------------------------------------------------------------------------------
// Initialize the filter
void vtkMEDExtrudeToCircle::Initialize()
//------------------------------------------------------------------------------
{
}




//------------------------------------------------------------------------------
// Execute method
void vtkMEDExtrudeToCircle::Execute()
//------------------------------------------------------------------------------
{
  vtkDebugMacro(<< "Executing vtkMEDExtrudeToCircle Filter") ;

  // pointers to input and output
  m_input = this->GetInput() ;
  m_output = this->GetOutput() ;

  // Make sure the filter is cleared of previous data before you run it !
  Initialize() ;



  //----------------------------------------------------------------------------
  // Calculate parameters of input hole and extrusion
  //----------------------------------------------------------------------------

  // Calculate the parameters of the input hole (centre, normal and radius)
  CalcHoleParameters() ;

  // Calculate the direction, length etc of the extrusion
  CalculateExtrusionVector() ;

  // Calculate radius of end of extrusion.
  // If the extrusion vector is not normal to the hole, need to correct radius
  // because the hole will be an elliptical slice with a greater radius than the vessel.
  // If you slice a circular tube with radius r, at an angle a to the normal,
  // the rms radius of the elliptical slice will be r * sqrt(1+sec^2(a)) / sqrt(2)
  double cosa = DotProduct(m_holeNormal, m_direction) ;
  m_endRadius = m_holeRadius * sqrt(2.0) * fabs(cosa) / sqrt(1.0 + cosa*cosa) ;



  //----------------------------------------------------------------------------
  // Create the extrusion mesh with vtkLinearExtrusionFilter
  //----------------------------------------------------------------------------
  m_extrude->SetInput(m_input) ;
  m_extrude->SetCapping(0) ;
  m_extrude->SetExtrusionTypeToVectorExtrusion() ;
  m_extrude->SetVector(m_extrusionVector) ;         // The extrusion is controlled by the length and direction of the vector.
  m_extrude->GetOutput()->Update() ;                // update required when Execute() contains a pipeline



  //----------------------------------------------------------------------------
  // Morph the mesh so that end is circular
  //----------------------------------------------------------------------------

  // Get no. of points in extruded tube
  m_extrude->GetOutput()->Update() ;
  vtkPolyData *tube = m_extrude->GetOutput() ;
  int numPtsTube = tube->GetNumberOfPoints() ;


  // Get the points around the hole in order
  int numPts = m_input->GetNumberOfPoints() ;
  vtkIdList *pts = vtkIdList::New() ;
  GetPointsAroundHole(m_input, pts) ;
  if (pts->GetNumberOfIds() != numPts){
    std::cout << "no. of points not the same " << pts->GetNumberOfIds() << " " << numPts << std::endl ;
    assert(false) ;
  }


  // define an arbitrary "up" vector from the centre to the first point
  double x[3], upVector[3] ;
  int ptId ;
  ptId = pts->GetId(0) ;
  m_input->GetPoint(ptId, x) ;
  SubtractVector(x, m_holeCentre, upVector) ;


  // Give each point round the hole a target value of phi.
  // This is the phi which the points would have if they were arranged uniformly around a circle.
  // We don't know which direction the hole goes, so we try i = 0, 1... n-1 and j = 0, n-1, n-2... 1
  // Note that the first point is zero phi in both cases.
  int i, j ;
  double *phi_tar = new double[numPts] ;
  double *phi_tar_f = new double[numPts] ;
  double *phi_tar_b = new double[numPts] ;
  double ainc = 2.0 * 3.14159 / (double)numPts ;
  for (i = 0, j = numPts ;  i < numPts ;  i++, j--){
    ptId = pts->GetId(i) ;
    phi_tar_f[ptId] = (double)i * ainc ;
    phi_tar_b[ptId] = (double)(j % numPts)  * ainc ;
  }

  // choose the sequence which gives increasing phi
  double sumsqf, sumsqb ;
  double rh, phih, zh ;
  for (i = 0, sumsqf = 0.0, sumsqb = 0.0 ;  i < numPts ;  i++){
    m_input->GetPoint(i, x) ;
    CalcCylinderCoords(x, m_holeCentre, m_direction, upVector, &rh, &phih, &zh) ;
    sumsqf += (phi_tar_f[i] - phih) * (phi_tar_f[i] - phih) ;
    sumsqb += (phi_tar_b[i] - phih) * (phi_tar_b[i] - phih) ;
  }
  if (sumsqf < sumsqb){
    for (i = 0 ;  i < numPts ;  i++)
      phi_tar[i] = phi_tar_f[i] ;
  }
  else{
    for (i = 0 ;  i < numPts ;  i++)
      phi_tar[i] = phi_tar_b[i] ;
  }



  // The standard vtk extrusion consists of npts around the hole, and another npts around the far end.
  // The cylindrical coords of the hole and the far end are identical, except for the z coord.
  // So r[j] = r[i], 
  // phi[j] = phi[i]
  // z[j] = zlen, z[i] = 0
  // where j = i+npts, 0 <= i < npts 
  if (numPtsTube != 2*numPts){
    std::cout << "extrusion is not standard vtk extrusion" << std::endl ;
    assert(false) ;
  }



  // Get each point on the extrusion and work out where to move it to
  double r, phi, z ;
  double xi[3], xj[3] ;
  for (i = 0 ;  i < numPts ;  i++){
    int ptId_hole = pts->GetId(i) ;
    int ptId_end = ptId_hole + numPts ;

    // get current position of points in cylinder coords
    tube->GetPoint(ptId_hole, xi) ;
    CalcCylinderCoords(xi, m_holeCentre, m_direction, upVector, &rh, &phih, &zh) ;
    tube->GetPoint(ptId_end, xj) ;
    CalcCylinderCoords(xj, m_holeCentre, m_direction, upVector, &r, &phi, &z) ;

    // set the z coord to the vector length
    // This is so that we end up with a circle normal to the axis, even if the axis is not normal to the start hole.
    double zpos = m_length ;

    // set the radius to that of the hole
    double rpos = m_endRadius ;

    // set the angle to the target phi of the corresponding point on the hole
    double phipos = phi_tar[ptId_hole] ;

    // convert to cartesian coords and move point
    CalcCartesianCoords(rpos, phipos, zpos, m_holeCentre, m_direction, upVector, x) ;
    tube->GetPoints()->SetPoint(ptId_end, x) ;
  }

  delete [] phi_tar ;
  delete [] phi_tar_f ;
  delete [] phi_tar_b ;



  //----------------------------------------------------------------------------
  // Copy result to output data
  //----------------------------------------------------------------------------
  m_output->DeepCopy(tube) ;
}



//------------------------------------------------------------------------------
// Set the length of the extrusion
void vtkMEDExtrudeToCircle::SetLength(double len)
//------------------------------------------------------------------------------
{
  // set the length
  m_length = len ;
  m_definedLength = true ;

  // extrusion vector and extrusion point are no longer defined
  m_definedVector = false ;
  m_definedExtrusionPoint = false ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set the direction of extrusion (does not set length)
void vtkMEDExtrudeToCircle::SetDirection(const double *direc)       
//------------------------------------------------------------------------------
{
  // copy vector and normalize
  Normalize(direc, m_direction) ;
  m_definedDirection = true ;

  // extrusion vector and extrusion point are no longer defined
  m_definedVector = false ;
  m_definedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the target point of extrusion
void vtkMEDExtrudeToCircle::SetExtrusionPoint(const double *extrPoint)
//------------------------------------------------------------------------------
{
  CopyVector(extrPoint, m_extrusionPoint) ;
  m_definedExtrusionPoint = true ;

  // everything else is undefined until we know the centre of the hole
  m_definedLength = false ;
  m_definedDirection = false ;
  m_definedVector = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the extrusion vector (length and direction)
void vtkMEDExtrudeToCircle::SetExtrusionVector(const double *extrVector)
//------------------------------------------------------------------------------
{
  CopyVector(extrVector, m_extrusionVector) ;
  m_definedVector = true ;

  // need to calculate length and direction later
  m_definedLength = false ;
  m_definedDirection = false ;

  // extrusion point is no longer defined
  m_definedExtrusionPoint = false ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Calculate the extrusion vector, once we know the centre and normal of the hole.
// This also calculates the length, direction and extrusion point.
// This pulls together all the possible routes for setting the extrusion vector.
// The default length is 5 * hole radius and the default direction is the hole normal, if they have not been set.
void vtkMEDExtrudeToCircle::CalculateExtrusionVector()
//------------------------------------------------------------------------------
{
  if (m_definedVector){
    // calculate length, direction and extrusion point from vector
    if (!m_definedLength){
      // back calculate length
      m_length = GetNorm(m_extrusionVector) ;
      m_definedLength = true ;
    }

    if (!m_definedDirection){
      // back-calculate direction
      Normalize(m_extrusionVector, m_direction) ;
      m_definedDirection = true ;
    }

    if (!m_definedExtrusionPoint){
      // calculate extrusion point
      AddVector(m_holeCentre, m_extrusionVector, m_extrusionPoint) ;
      m_definedExtrusionPoint = true ;
    }
  }
  else if (m_definedExtrusionPoint){
    // calculate vector, length and direction from extrusion point
    SubtractVector(m_extrusionPoint, m_holeCentre, m_extrusionVector) ;
    m_length = GetNorm(m_extrusionVector) ;
    Normalize(m_extrusionVector, m_direction) ;
    m_definedLength = true ;
    m_definedDirection = true ;
    m_definedVector = true ;
  }
  else{
    if (!m_definedLength){
      // if no length defined, set to 5 * hole radius
      m_length = 5.0 * m_holeRadius ;
      m_definedLength = true ;
    }

    if (!m_definedDirection){
      // if no direction defined, set to normal of hole
      Normalize(m_holeNormal, m_direction) ;
      m_definedDirection = true ;
    }

    // calculate vector from length and direction
    MultVectorByScalar(m_length, m_direction, m_extrusionVector) ;
    m_definedVector = true ;

    // calculate extrusion point
    AddVector(m_holeCentre, m_extrusionVector, m_extrusionPoint) ;
    m_definedExtrusionPoint = true ;
  }
}



//------------------------------------------------------------------------------
// Calculate Centre, normal and radius of hole
// The normal is normalised.
void vtkMEDExtrudeToCircle::CalcHoleParameters()
//------------------------------------------------------------------------------
{
  int i, j ;
  double sumsq, x[3] ;
  vtkPoints *pts = m_input->GetPoints() ;
  int numPts = m_input->GetNumberOfPoints() ;

  //----------------------------------------------------------------------------
  // get the centre
  //----------------------------------------------------------------------------

  // get the mean centre
  SetVector(m_holeCentre, 0, 0, 0) ;
  for (i = 0 ;  i < numPts ; i++){
    pts->GetPoint(i, x) ;
    AddVector(m_holeCentre, x, m_holeCentre) ;
  }
  m_holeCentre[0] /= (double)numPts ;
  m_holeCentre[1] /= (double)numPts ;
  m_holeCentre[2] /= (double)numPts ;


  //----------------------------------------------------------------------------
  // get the radius
  //----------------------------------------------------------------------------

  // get points relative to centre
  double **dx = new double*[numPts] ;

  for (i = 0 ;  i < numPts ; i++){
    dx[i] = new double[3] ;
    pts->GetPoint(i, x) ;
    SubtractVector(x, m_holeCentre, dx[i]) ;
  }

  // get the rms radius
  for (i = 0, sumsq = 0.0 ;  i < numPts ; i++){
    for (j = 0 ;  j < 3 ;  j++)
      sumsq += dx[i][j] * dx[i][j] ;
  }
  sumsq /= (double)numPts ;
  m_holeRadius = sqrt(sumsq) ;

  // free allocated memory
  for (i = 0 ;  i < numPts ; i++)
    delete [] dx[i] ;
  delete [] dx ;


  //----------------------------------------------------------------------------
  // get the normal
  //----------------------------------------------------------------------------

  // Find the normal to the plane of the hole.
  // In this example data, the polydata circulates around the hole centre c, such that for each polyline cell,
  // with endpoints p0 and p1, the cross product (p1-c)^(p0-c) points in the direction of the outward normal.
  // Therefore we use the median cross product to define the normal.
  int numPtsInCell ;
  vtkIdType *ptlist ;
  double pt0[3], pt1[3] ;
  vtkMEDPastValuesList vecProd0(1000), vecProd1(1000), vecProd2(1000) ;
  m_input->BuildCells() ;
  for (int i = 0 ;  i < m_input->GetNumberOfCells() ;  i++){
    m_input->GetCellPoints(i, numPtsInCell, ptlist) ;
    if (numPtsInCell == 2){
      m_input->GetPoint(ptlist[0], pt0) ;
      m_input->GetPoint(ptlist[1], pt1) ;
      SubtractVector(pt0, m_holeCentre, pt0) ;
      SubtractVector(pt1, m_holeCentre, pt1) ;

      // get cross product and add to list
      double cp[3] ;
      CrossProduct(pt1, pt0, cp) ;
      vecProd0.AddNewValue(cp[0]) ;
      vecProd1.AddNewValue(cp[1]) ;
      vecProd2.AddNewValue(cp[2]) ;
    }
  }

  // set normal to median cross product
  m_holeNormal[0] = vecProd0.GetMedian() ;
  m_holeNormal[1] = vecProd1.GetMedian() ;
  m_holeNormal[2] = vecProd2.GetMedian() ;
  Normalize(m_holeNormal, m_holeNormal) ;
}





//------------------------------------------------------------------------------
// Get cylindrical coords (r, phi, z) of point x.
// The cylinder system is defined by the central axis and an "up" vector, which defines phi = 0
// cylAxis and upVector do not have to be normalised.
// upVector does not have to be exactly normal to cylAxis.
// phi is in the range 0 to 2pi.
void vtkMEDExtrudeToCircle::CalcCylinderCoords(const double *x, const double *centre, const double *cylAxis, const double *upVector, 
                                         double *r, double *phi, double *z) const 
//------------------------------------------------------------------------------
{
  // Get normalised central axis
  double u[3], v[3], w[3] ;
  Normalize(cylAxis, w) ;

  // Get orthonormal up vector: u = upvec - (upvec.w)w
  double dp = DotProduct(upVector, cylAxis) ;
  u[0] = upVector[0] - dp*w[0] ;
  u[1] = upVector[1] - dp*w[1] ;
  u[2] = upVector[2] - dp*w[2] ;
  Normalize(u, u) ;

  // Get third axis v = w^u
  CrossProduct(w,u,v) ;

  // Get x vector relative to centre
  double dx[3] ;
  SetVector(dx, x[0]-centre[0], x[1]-centre[1], x[2]-centre[2]) ;

  // Get cartesian coords in cylinder system
  double ucoord = DotProduct(dx, u) ;
  double vcoord = DotProduct(dx, v) ;
  double wcoord = DotProduct(dx, w) ;

  // calculate cylindrical coords
  *z = wcoord ;
  *r = sqrt(ucoord*ucoord + vcoord*vcoord) ;
  *phi = atan2(vcoord, ucoord) ;

  // put phi range 0 to 2pi
  if (*phi < 0.0)
    *phi += 2.0 * 3.14159 ;
}




//------------------------------------------------------------------------------
// Get cartesian coords x given cylindrical coords (r, phi, z)
// The cylinder system is defined by the central axis and an "up" vector.
// cylAxis and upVector do not have to be normalised.
// upVector does not have to be exactly normal to cylAxis.
void vtkMEDExtrudeToCircle::CalcCartesianCoords(double r, double phi, double z, const double *centre, const double *cylAxis, const double *upVector, double *x) const 
//------------------------------------------------------------------------------
{
  // Get normalised central axis
  double u[3], v[3], w[3] ;
  Normalize(cylAxis, w) ;

  // Get orthonormal up vector: u = upvec - (upvec.w)w
  double dp = DotProduct(upVector, cylAxis) ;
  u[0] = upVector[0] - dp*w[0] ;
  u[1] = upVector[1] - dp*w[1] ;
  u[2] = upVector[2] - dp*w[2] ;
  Normalize(u, u) ;

  // Get third axis v = w^u
  CrossProduct(w,u,v) ;

  // convert cylinder coords to cartesian coords (ucoord,vcoord,wcoord) in cylinder frame
  double ucoord = r*cos(phi) ;
  double vcoord = r*sin(phi) ;
  double wcoord = z ;

  // Get x vector relative to centre
  double dx[3], ucoords[3] ;
  ucoords[0] = ucoord ;
  ucoords[1] = vcoord ;
  ucoords[2] = wcoord ;
  InverseSolveTransform(u, v, w, ucoords, dx) ;

  // add centre to get world x
  SetVector(x, dx[0]+centre[0], dx[1]+centre[1], dx[2]+centre[2]) ;
}


//------------------------------------------------------------------------------
// Get matrix to rotate arrow (x axis) to vector direction u
void vtkMEDExtrudeToCircle::GetMatRotArrowToAxis(vtkMatrix4x4 *mat, const double *u) const 
//------------------------------------------------------------------------------
{
  double xnew[3], ynew[3], znew[3] ;

  // calculate new x axis to be normal along u
  Normalize(u, xnew) ;


  // calculate new y and z axes normal to u
  double a[3], b[3], c[3] ; // 3 possible vectors, at least 2 of which must be non-zero
  SetVector(a, u[2], 0.0, -u[0]) ;
  SetVector(b, 0.0, u[2], -u[1]) ;
  SetVector(c, -u[1], u[0], 0.0) ;

  // y = weighted sum of vectors
  double anorm = GetNorm(a) ;
  double bnorm = GetNorm(b) ;
  double cnorm = GetNorm(c) ;
  for (int i = 0 ;  i < 3 ;  i++)
    ynew[i] = anorm*a[i] + bnorm*b[i] + cnorm*c[i] ;
  Normalize(ynew, ynew) ;

  // point ynew in direction of y if possible
  if (ynew[1] < 0){
    ynew[0] *= -1.0 ;
    ynew[1] *= -1.0 ;
    ynew[2] *= -1.0 ;
  }


  // calculate new z axis z = x ^ y
  CrossProduct(xnew, ynew, znew) ;
  Normalize(znew, znew) ;


  // The rotation matrix should be R = xnew0 ynew0 znew0
  //                                   xnew1 ynew1 znew1
  //                                   xnew2 ynew2 znew2
  mat->Identity() ;
  for (int i = 0 ;  i < 3 ;  i++){
    mat->SetElement(i,0,xnew[i]) ;
    mat->SetElement(i,1,ynew[i]) ;
    mat->SetElement(i,2,znew[i]) ;
  }
}


//------------------------------------------------------------------------------
// List points around hole in order
void vtkMEDExtrudeToCircle::GetPointsAroundHole(vtkPolyData *hole, vtkIdList *pts) const 
//------------------------------------------------------------------------------
{
  int i ;
  vtkIdType numPtsInCell ;
  unsigned short numCellsOnPt ;
  vtkIdType *ptlist ;
  vtkIdType *cellList ;

  // check that all the cells have two points
  hole->BuildCells() ;
  hole->BuildLinks() ;
  for (i = 0 ;  i < hole->GetNumberOfCells() ;  i++){
    hole->GetCellPoints(i, numPtsInCell, ptlist) ;
    if (numPtsInCell != 2){
      std::cout << "cell " << i << " found with " << numPtsInCell << " pts" << std::endl ;
      assert(false) ;
    }
  }

  // check that all the points have two cells
  for (i = 0 ;  i < hole->GetNumberOfPoints() ;  i++){
    hole->GetPointCells(i, numCellsOnPt, cellList) ;
    if (numCellsOnPt != 2){
      std::cout << "point " << i << " found with " << numCellsOnPt << " cells" << std::endl ;
      assert(false) ;
    }
  }

  // trace points in order around the hole
  int icell = 0 ;
  int lastPt = -1 ;
  bool istop = false ;
  int ilo ;
  int ihi ;
  pts->Initialize() ;
  while (pts->GetNumberOfIds() < hole->GetNumberOfPoints() && !istop){
    // get points in icell
    hole->GetCellPoints(icell, numPtsInCell, ptlist) ;

    if (lastPt == -1){
      // first cell so put both points in list
      ilo = 0 ;
      ihi = 1 ;
      pts->InsertNextId(ptlist[ilo]) ;
      pts->InsertNextId(ptlist[ihi]) ;
      lastPt = ptlist[ihi] ;
    }
    else{
      // set ihi such that ptlist[ilo] is the previous point and ptlist[ihi] is the new point
      if ((ptlist[0] == lastPt) && (ptlist[1] != lastPt)){
        ilo = 0 ;
        ihi = 1 ;
      }
      else if ((ptlist[1] == lastPt) && (ptlist[0] != lastPt)){
        ilo = 1 ;
        ihi = 0 ;
      }
      else{
        std::cout << "problem with pt indices in GetPointsAroundHole()" << std::endl ;
        assert(false) ;
      }

      if (ptlist[ihi] != pts->GetId(0)){
        // add point to list
        pts->InsertNextId(ptlist[ihi]) ;
        lastPt = ptlist[ihi] ;
      }
      else{
        // stop when we get back to the first point
        istop = true ;
      }
    }


    // get cells attached to new point
    hole->GetPointCells(ptlist[ihi], numCellsOnPt, cellList) ;

    // set next cell to the one which is not the current cell
    if ((cellList[0] == icell) && (cellList[1] != icell))
      icell = cellList[1] ;
    else if ((cellList[1] == icell) && (cellList[0] != icell))
      icell = cellList[0] ;
    else{
      std::cout << "problem with cell indices in GetPointsAroundHole()" << std::endl ;
      assert(false) ;
    }
  }
}




//------------------------------------------------------------------------------
// Calculate the target phi for an arbitrary (r, phi)
// Given the original hole points (r_hole, phi_hole) and their target phi's.
double vtkMEDExtrudeToCircle::CalculateTargetPhi(double *r, double *phi, double *r_hole, double *phi_hole, double *phi_target, int npts) const 
//------------------------------------------------------------------------------
{
  int i, imin1, imin2 ;
  double mindist = 0 ;
  double phi_tar ;
  double *dist = new double[npts] ;

  // Find the nearest point to (r,phi) on the original hole
  for (i = 0 ;  i < npts ;  i++){
    double dx = r[i]*cos(phi[i]) - r_hole[i]*cos(phi_hole[i]) ;
    double dy = r[i]*sin(phi[i]) - r_hole[i]*sin(phi_hole[i]) ;
    dist[i] = dx*dx + dy*dy ;
    if ((dist[i] < mindist) || (i == 0)){
      mindist = dist[i] ;
      imin1 = i ;
    }
  }

  // Find out which of neighbours imin+1 or imin-1 is nearest to the test point
  int ibk = (imin1 - 1) % npts ;
  int ifw = (imin1 + 1) % npts ;
  if (dist[ibk] < dist[ifw])
    imin2 = ibk ;
  else
    imin2 = ifw ;

  // interpolate target values of phi between the two points imin1 and imin2
  phi_tar = (dist[imin1] * phi_target[imin1] + dist[imin2] * phi_target[imin2]) / (dist[imin1] + dist[imin2]) ;


  delete [] dist ;

  return phi_tar ;
}




//------------------------------------------------------------------------------
// Copy vector
void vtkMEDExtrudeToCircle::CopyVector(const double *a, double *b) const
//------------------------------------------------------------------------------
{
  b[0] = a[0] ;
  b[1] = a[1] ;
  b[2] = a[2] ;
}


//------------------------------------------------------------------------------
// Set vector
void vtkMEDExtrudeToCircle::SetVector(double *vec, double x, double y, double z) const
//------------------------------------------------------------------------------
{
  vec[0] = x ;
  vec[1] = y ;
  vec[2] = z ;
}

//------------------------------------------------------------------------------
// Add vector
void vtkMEDExtrudeToCircle::AddVector(const double *vin, const double *x, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = vin[0] + x[0] ;
  vout[1] = vin[1] + x[1] ;
  vout[2] = vin[2] + x[2] ;

}


//------------------------------------------------------------------------------
// Subtract vector
void vtkMEDExtrudeToCircle::SubtractVector(const double *vin, const double *x, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = vin[0] - x[0] ;
  vout[1] = vin[1] - x[1] ;
  vout[2] = vin[2] - x[2] ;

}

//------------------------------------------------------------------------------
// Multiply vector by scalar
void vtkMEDExtrudeToCircle::MultVectorByScalar(double s, const double *vin, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = s*vin[0] ;
  vout[1] = s*vin[1] ;
  vout[2] = s*vin[2] ;
}



//------------------------------------------------------------------------------
// Get norm of vector
double vtkMEDExtrudeToCircle::GetNorm(const double *vec) const
//------------------------------------------------------------------------------
{
  return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]) ;
}

//------------------------------------------------------------------------------
// Normalize vector
void vtkMEDExtrudeToCircle::Normalize(const double *vin, double *vout) const
//------------------------------------------------------------------------------
{
  double norm = GetNorm(vin) ;
  if (norm == 0.0){
    std::cout << "vector has zero magnitude" << std::endl ;
    assert(false) ;
  }

  vout[0] = vin[0] / norm ;
  vout[1] = vin[1] / norm ;
  vout[2] = vin[2] / norm ;
}

//------------------------------------------------------------------------------
// Cross product of vectors a ^ b = c
void vtkMEDExtrudeToCircle::CrossProduct(const double *a, const double *b, double *c) const
//------------------------------------------------------------------------------
{
  c[0] =   a[1]*b[2] - a[2]*b[1] ;
  c[1] = -(a[0]*b[2] - a[2]*b[0]) ;
  c[2] =   a[0]*b[1] - a[1]*b[0] ;
}


//------------------------------------------------------------------------------
// Dot product of vectors a.b
double vtkMEDExtrudeToCircle::DotProduct(const double *a, const double *b) const
//------------------------------------------------------------------------------
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] ;
}


//------------------------------------------------------------------------------
// Inverse solution of equation y = Mx, where M = row vectors (u, v and w)
// Find x given M and y
void vtkMEDExtrudeToCircle::InverseSolveTransform(const double *u, const double *v, const double *w, const double *y, double *x) const
//------------------------------------------------------------------------------
{
  double det, det0, det1, det2 ;

  det = u[0]*(v[1]*w[2] - v[2]*w[1]) - u[1]*(v[0]*w[2] - v[2]*w[0]) + u[2]*(v[0]*w[1] - v[1]*w[0]) ;
  det0 = y[0]*(v[1]*w[2] - v[2]*w[1]) - u[1]*(y[1]*w[2] - v[2]*y[2]) + u[2]*(y[1]*w[1] - v[1]*y[2]) ;
  det1 = u[0]*(y[1]*w[2] - v[2]*y[2]) - y[0]*(v[0]*w[2] - v[2]*w[0]) + u[2]*(v[0]*y[2] - y[1]*w[0]) ;
  det2 = u[0]*(v[1]*y[2] - y[1]*w[1]) - u[1]*(v[0]*y[2] - y[1]*w[0]) + y[0]*(v[0]*w[1] - v[1]*w[0]) ;

  x[0] = det0 / det ;
  x[1] = det1 / det ;
  x[2] = det2 / det ;
}


//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  double holeCentre[3], holeNormal[3] ;

  GetHoleCentre(holeCentre) ;
  GetHoleNormal(holeNormal) ;

  os << indent << "hole parameters..." << std::endl ;
  os << indent << "centre: " << holeCentre[0] << " " << holeCentre[1] << " " << holeCentre[2] << std::endl ;
  os << indent << "normal: " << holeNormal[0] << " " << holeNormal[1] << " " << holeNormal[2] << std::endl ;
  os << indent << "radius: " << GetHoleRadius() << std::endl ;


  double dir[3], vec[3], extrPt[3] ;

  GetDirection(dir) ;
  GetExtrusionVector(vec) ;
  GetExtrusionPoint(extrPt) ;

  os << indent << "extrusion parameters..." << std::endl ;
  os << indent << "direction: " << dir[0] << " " << dir[1] << " " << dir[2] << std::endl ;
  os << indent << "vector: " << vec[0] << " " << vec[1] << " " << vec[2] << std::endl ;
  os << indent << "extrusion point: " << extrPt[0] << " " << extrPt[1] << " " << extrPt[2] << std::endl ;
  os << indent << "length: " << GetLength() << std::endl ;
  os << indent << "end radius: " << GetEndRadius() << std::endl ;

  os << std::endl ;

}
