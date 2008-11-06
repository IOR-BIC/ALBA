/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDExtrudeToCircle.cxx,v $
Language:  C++
Date:      $Date: 2008-11-06 10:22:50 $
Version:   $Revision: 1.4.2.1 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2008 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


#include "mafDefines.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkMEDPastValuesList.h"
#include "vtkMEDExtrudeToCircle.h"
#include <assert.h>

#ifndef M_PI
  #define _USE_MATH_DEFINES
#endif

#include <cmath>




//------------------------------------------------------------------------------
// standard macros
vtkCxxRevisionMacro(vtkMEDExtrudeToCircle, "$Revision: 1.4.2.1 $");
vtkStandardNewMacro(vtkMEDExtrudeToCircle);
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Constructor
vtkMEDExtrudeToCircle::vtkMEDExtrudeToCircle() : DefinedLength(false), DefinedDirection(false), DefinedVector(false),
DefinedExtrusionPoint(false), DefinedMinNumEndPts(false), BuiltCells(false),
Mesh(NULL), DefaultDirectionSign(1)
//------------------------------------------------------------------------------
{
}


//------------------------------------------------------------------------------
// Destructor
vtkMEDExtrudeToCircle::~vtkMEDExtrudeToCircle()
//------------------------------------------------------------------------------
{  
  if (Mesh != NULL)
    delete Mesh ;
}



//------------------------------------------------------------------------------
// Initialize the filter
void vtkMEDExtrudeToCircle::Initialize()
//------------------------------------------------------------------------------
{
  if (Mesh != NULL){
    delete Mesh ;
    Mesh = NULL ;
  }

  Output->Initialize() ;
}




//------------------------------------------------------------------------------
// Execute method
void vtkMEDExtrudeToCircle::Execute()
//------------------------------------------------------------------------------
{
  vtkDebugMacro(<< "Executing ExtrudeToCircle Filter") ;

  // pointers to input and output
  Input = this->GetInput() ;
  Output = this->GetOutput() ;

  // Make sure the filter is cleared of previous data before you run it !
  Initialize() ;



  //----------------------------------------------------------------------------
  // Calculate parameters of input hole and extrusion
  //----------------------------------------------------------------------------

  // Calculate the parameters of the input hole (centre, normal and radius)
  CalcHoleParameters(Input) ;

  // Calculate the direction, length etc of the extrusion
  CalculateExtrusionVector() ;

  // Calculate radius of end of extrusion.
  // If the extrusion vector is not normal to the hole, need to correct radius
  // because the hole will be an elliptical slice with a greater radius than the vessel.
  // If you slice a circular tube with radius r, at an angle a to the normal,
  // the rms radius of the elliptical slice will be r * sqrt(1+sec^2(a)) / sqrt(2)
  double cosa = DotProduct(HoleNormal, Direction) ;
  EndRadius = HoleRadius * sqrt(2.0) * fabs(cosa) / sqrt(1.0 + cosa*cosa) ;


  //----------------------------------------------------------------------------
  // Calculate the mesh structure
  //----------------------------------------------------------------------------
  Mesh = new MeshData ;
  CalcExtrusionRings() ;
  CalcExtrusionVertices() ;


  //----------------------------------------------------------------------------
  // Transfer mesh to output polydata
  //----------------------------------------------------------------------------
  vtkPoints *points = vtkPoints::New() ;
  VerticesToVtkPoints(points) ;
  Output->SetPoints(points) ;
  points->Delete() ;

  vtkCellArray *triangles = vtkCellArray::New() ;
  VerticesToVtkTriangles(triangles) ;
  Output->SetPolys(triangles) ;
  triangles->Delete() ;
}



//------------------------------------------------------------------------------
// Set the length of the extrusion
void vtkMEDExtrudeToCircle::SetLength(double len)
//------------------------------------------------------------------------------
{
  // set the length
  Length = len ;
  DefinedLength = true ;

  // extrusion vector and extrusion point are no longer defined
  DefinedVector = false ;
  DefinedExtrusionPoint = false ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set the direction of extrusion.
// The direction does not have to be normalised.
void vtkMEDExtrudeToCircle::SetDirection(const double *direc)       
//------------------------------------------------------------------------------
{
  // copy vector and normalize
  Normalize(direc, Direction) ;
  DefinedDirection = true ;

  // extrusion vector and extrusion point are no longer defined
  DefinedVector = false ;
  DefinedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the direction of extrusion.
// The direction does not have to be normalised.
void vtkMEDExtrudeToCircle::SetDirection(double x, double y, double z)       
//------------------------------------------------------------------------------
{
  // copy vector and normalize
  Direction[0] = x ;
  Direction[1] = y ;
  Direction[2] = z ;

  Normalize(Direction, Direction) ;
  DefinedDirection = true ;

  // extrusion vector and extrusion point are no longer defined
  DefinedVector = false ;
  DefinedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the target point of extrusion.
void vtkMEDExtrudeToCircle::SetExtrusionPoint(const double *extrPoint)
//------------------------------------------------------------------------------
{
  CopyVector(extrPoint, ExtrusionPoint) ;
  DefinedExtrusionPoint = true ;

  // everything else is undefined until we know the centre of the hole
  DefinedLength = false ;
  DefinedDirection = false ;
  DefinedVector = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the extrusion vector.
void vtkMEDExtrudeToCircle::SetExtrusionVector(const double *extrVector)
//------------------------------------------------------------------------------
{
  CopyVector(extrVector, ExtrusionVector) ;
  DefinedVector = true ;

  // need to calculate length and direction later
  DefinedLength = false ;
  DefinedDirection = false ;

  // extrusion point is no longer defined
  DefinedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// set min no. of vertices at end of extrusion
void vtkMEDExtrudeToCircle::SetMinNumberofEndPoints(int minNumEndPoints)
//------------------------------------------------------------------------------
{
  MinNumEndPts = minNumEndPoints ;
  DefinedMinNumEndPts = true ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Reset the extrusion direction to the default, which is normal to the hole.
// Use this to reverse the direction if the extrusion goes the wrong way.
// Argument is +1 or -1 to select forward or reverse direction relative to the hole.
void vtkMEDExtrudeToCircle::SetDirectionToDefault(int directionSign)      
//------------------------------------------------------------------------------
{
  // set any existing vector data to undefined
  DefinedDirection = false ;
  DefinedVector = false ;
  DefinedExtrusionPoint = false ;

  // set the flag for the sign of the direction
  if (directionSign == -1)
    DefaultDirectionSign = -1 ;
  else
    DefaultDirectionSign = 1 ;

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
  if (DefinedVector){
    // calculate length, direction and extrusion point from vector
    if (!DefinedLength){
      // back calculate length
      Length = GetNorm(ExtrusionVector) ;
      DefinedLength = true ;
    }

    if (!DefinedDirection){
      // back-calculate direction
      Normalize(ExtrusionVector, Direction) ;
      DefinedDirection = true ;
    }

    if (!DefinedExtrusionPoint){
      // calculate extrusion point
      AddVector(HoleCentre, ExtrusionVector, ExtrusionPoint) ;
      DefinedExtrusionPoint = true ;
    }
  }
  else if (DefinedExtrusionPoint){
    // calculate vector, length and direction from extrusion point
    SubtractVector(ExtrusionPoint, HoleCentre, ExtrusionVector) ;
    Length = GetNorm(ExtrusionVector) ;
    Normalize(ExtrusionVector, Direction) ;
    DefinedLength = true ;
    DefinedDirection = true ;
    DefinedVector = true ;
  }
  else{
    if (!DefinedLength){
      // if no length defined, set to 5 * hole radius
      Length = 5.0 * HoleRadius ;
      DefinedLength = true ;
    }

    if (!DefinedDirection){
      // if no direction defined, set to normal of hole
      Normalize(HoleNormal, Direction) ;

      // check if the direction needs to be reversed
      if (DefaultDirectionSign == -1){
        MultVectorByScalar(-1.0, Direction, Direction) ;
      }

      DefinedDirection = true ;
    }

    // calculate vector from length and direction
    MultVectorByScalar(Length, Direction, ExtrusionVector) ;
    DefinedVector = true ;

    // calculate extrusion point
    AddVector(HoleCentre, ExtrusionVector, ExtrusionPoint) ;
    DefinedExtrusionPoint = true ;
  }
}



//------------------------------------------------------------------------------
// Calculate Centre, normal and radius of hole, and no. of vertices
// The normal is normalised.
void vtkMEDExtrudeToCircle::CalcHoleParameters(vtkPolyData *hole)
//------------------------------------------------------------------------------
{
  int i, j ;
  double sumsq, x[3] ;
  vtkPoints *pts = hole->GetPoints() ;
  int numPts = Input->GetNumberOfPoints() ;

  //----------------------------------------------------------------------------
  // get the centre
  //----------------------------------------------------------------------------

  // get the mean centre
  SetVector(HoleCentre, 0, 0, 0) ;
  for (i = 0 ;  i < numPts ; i++){
    pts->GetPoint(i, x) ;
    AddVector(HoleCentre, x, HoleCentre) ;
  }
  HoleCentre[0] /= (double)numPts ;
  HoleCentre[1] /= (double)numPts ;
  HoleCentre[2] /= (double)numPts ;


  //----------------------------------------------------------------------------
  // get the radius
  //----------------------------------------------------------------------------

  double dx[3] ;
  for (i = 0, sumsq = 0.0 ;  i < numPts ; i++){
    // get point relative to hole centre
    pts->GetPoint(i, x) ;
    SubtractVector(x, HoleCentre, dx) ;

    // add to sum squared radius
    for (j = 0 ;  j < 3 ;  j++)
      sumsq += dx[j] * dx[j] ;
  }

  // get the rms radius
  sumsq /= (double)numPts ;
  HoleRadius = sqrt(sumsq) ;


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

  // Build cell links if not already done
  if (!BuiltCells){
    Input->BuildCells() ;
    BuiltCells = true ;
  }

  for (int i = 0 ;  i < Input->GetNumberOfCells() ;  i++){
    Input->GetCellPoints(i, numPtsInCell, ptlist) ;
    if (numPtsInCell == 2){
      Input->GetPoint(ptlist[0], pt0) ;
      Input->GetPoint(ptlist[1], pt1) ;
      SubtractVector(pt0, HoleCentre, pt0) ;
      SubtractVector(pt1, HoleCentre, pt1) ;

      // get cross product and add to list
      double cp[3] ;
      CrossProduct(pt1, pt0, cp) ;
      vecProd0.AddNewValue(cp[0]) ;
      vecProd1.AddNewValue(cp[1]) ;
      vecProd2.AddNewValue(cp[2]) ;
    }
  }

  // set normal to median cross product
  HoleNormal[0] = vecProd0.GetMedian() ;
  HoleNormal[1] = vecProd1.GetMedian() ;
  HoleNormal[2] = vecProd2.GetMedian() ;
  Normalize(HoleNormal, HoleNormal) ;



  //----------------------------------------------------------------------------
  // get the number of points around the hole
  //----------------------------------------------------------------------------
  HoleNumVerts = pts->GetNumberOfPoints() ;
}




//------------------------------------------------------------------------------
// Calculate "up" vector which defines phi = 0 in cylindrical coords
// It is the direction from the centre of the hole to the first vertex.
// This is not necessarily normal to the cylinder axis.
void vtkMEDExtrudeToCircle::CalcUpVector(vtkIdList *holepts)
//------------------------------------------------------------------------------
{
  double x[3] ;
  int ptId ;
  ptId = holepts->GetId(0) ;
  Input->GetPoint(ptId, x) ;
  SubtractVector(x, HoleCentre, UpVector) ;
}



//------------------------------------------------------------------------------
// Calculate no. of rings required on extrusion, their positions and no. of points in each
// Needs length of extrusion and corrected end radius
// Allocates memory for the rings and the vertices in the mesh structure.
void vtkMEDExtrudeToCircle::CalcExtrusionRings()
//------------------------------------------------------------------------------
{
  int i, k ;

  // set the requested min. value for the no. of end points to something sensible.
  // default is the same as the no. of vertices on the hole.
  if (!DefinedMinNumEndPts || (MinNumEndPts < HoleNumVerts)){
    // set min no. of end vertices to default
    MinNumEndPts = HoleNumVerts ;
    DefinedMinNumEndPts = true ;
  }

  // calculate circumference of extrusion
  double circumf = 2*M_PI * EndRadius ;

  // try increasing the no. of rings k until we get a number which gives the right length.
  k = 2 ;
  double m, sumL;
  while (k < 1000){
    // calculate slope m where no. of vertices ni in ring i is given by ni = n0 + m*i
    m = (double)(MinNumEndPts - HoleNumVerts) / (double)(k-1) ;

    // calculate the total length of the extrusion
    for (i = 1, sumL = 0.0 ;  i < k ;  i++){
      int ni = HoleNumVerts + (int)(m*(double)i) ; 
      double Li = circumf / (double)ni ;
      sumL += Li ;
    }

    // stop when the length reaches the desired length
    if (sumL >= Length)
      break ;

    k++ ;
  }

  // tweak the circumference so that the total length will be the same as the requested length
  circumf *= Length / sumL ;


  //----------------------------------------------------------------------------
  // set the positions and no. of vertices in each ring
  //----------------------------------------------------------------------------

  // allocate the rings
  Mesh->AllocateRings(k) ;

  // first ring is same as start hole
  Mesh->Ring[0].AllocateVertices(HoleNumVerts) ;
  Mesh->Ring[0].Z = 0.0 ;

  // allocate the number of vertices in each ring
  double z = 0.0 ;
  for (i = 1 ;  i < k-1 ;  i++){
    int ni = HoleNumVerts + (int)(m*(double)i) ; 
    z += circumf / (double)ni ;

    Mesh->Ring[i].AllocateVertices(ni) ;
    Mesh->Ring[i].Z = z ;
  }

  // last ring as requested
  Mesh->Ring[k-1].AllocateVertices(MinNumEndPts) ;
  Mesh->Ring[k-1].Z = Length ;
}





//------------------------------------------------------------------------------
// Calculate cylindrical and cartesian coords of each mesh vertex
void vtkMEDExtrudeToCircle::CalcExtrusionVertices()
//------------------------------------------------------------------------------
{
  int i, j ;

  // get indices of points around hole, in correct order
  vtkIdList *holepts = vtkIdList::New() ;
  GetPointsAroundHole(Input, holepts) ;

  // Calculate the up vector for the cylindrical coord system
  CalcUpVector(holepts) ;

  // Reverse order of points if the list of points contains decreasing phi
  int sense = CalcSenseOfPointsAroundHole(holepts, HoleCentre, ExtrusionVector, UpVector) ;
  if (sense < 0){
    ReverseIdList(holepts) ;
    CircularShiftIdList(holepts, 1) ; // rotate list to put zero phi back at first entry
  }

  // Calculate the up vector for the cylindrical coord system
  CalcUpVector(holepts) ;

  // get the cylindrical coords of the hole points and copy them to the first ring
  double r, phi, z ;
  int ptId ;
  double x[3] ;
  for (j = 0 ;  j < holepts->GetNumberOfIds() ;  j++){
    ptId = holepts->GetId(j) ;
    Input->GetPoint(ptId, x) ;  // cartesian coords of hole point
    CalcCylinderCoords(x, HoleCentre, ExtrusionVector, UpVector, &r, &phi, &z) ;
    Mesh->Ring[0].Vertex[j].SetCylCoords(r, phi, z) ;
  }

  // Remove unwanted jumps of 2pi from the data
  Remove2PiArtefactsFromFirstRing() ;

  // calculate the cylindrical coords of the end ring, which is of course a 
  // perfect circle, normal to the z axis
  int iend = Mesh->NumRings - 1 ;
  double dphi = 2*M_PI / (double)Mesh->Ring[iend].NumVerts ;
  for (j = 0 ;  j < Mesh->Ring[iend].NumVerts ;  j++){
    r = EndRadius ;
    phi = (double)j * dphi ;
    z = Mesh->Ring[iend].Z ;
    Mesh->Ring[iend].Vertex[j].SetCylCoords(r, phi, z) ;
  }

  // Tricky bit: now we have to interpolate the vertices on the middle rings
  for (i = 1 ;  i < Mesh->NumRings-1 ;  i++){
    for (j = 0 ;  j < Mesh->Ring[i].NumVerts ;  j++){
      // find the point on the end ring which corresponds to point j
      double factor1 = (double)Mesh->Ring[iend].NumVerts / (double)Mesh->Ring[i].NumVerts ;
      int jend = (int)((double)j * factor1 + 0.5) ;

      // find the points on the start ring which correspond to point j
      // and the relative weighting for each
      double factor2 = (double)Mesh->Ring[0].NumVerts / (double)Mesh->Ring[i].NumVerts ;
      double jdouble = (double)j * (double)factor2 ;
      int jstart0 = (int)jdouble ;
      int jstart1 = (jstart0 + 1) ;
      double rmdr = jdouble - (double)jstart0 ;
      double w0 = 1.0 - rmdr ;
      double w1 = rmdr ;

      // make sure indices are in correct range
      jend = jend % Mesh->Ring[iend].NumVerts ;
      jstart0 = jstart0 % Mesh->Ring[0].NumVerts ;
      jstart1 = jstart1 % Mesh->Ring[0].NumVerts ;

      // get the total weights for the three points
      double lambda = Mesh->Ring[i].Z / Mesh->Ring[iend].Z ;
      double w_end = lambda ;
      double w_start0 = (1.0 - lambda)*w0 ;
      double w_start1 = (1.0 - lambda)*w1 ;

      // get the cylindrical coords of the three points
      double r_start0, phi_start0, z_start0 ;
      double r_start1, phi_start1, z_start1 ;
      double r_end, phi_end, z_end ;
      Mesh->Ring[0].Vertex[jstart0].GetCylCoords(&r_start0, &phi_start0, &z_start0) ;
      Mesh->Ring[0].Vertex[jstart1].GetCylCoords(&r_start1, &phi_start1, &z_start1) ;
      Mesh->Ring[iend].Vertex[jend].GetCylCoords(&r_end, &phi_end, &z_end) ;
      if (jstart1 == 0){
        // special case: deal with phi wrapping round from 2pi to zero
        phi_start1 += + 2.0*M_PI ;
      }

      // interpolate the three points
      r = w_start0*r_start0 + w_start1*r_start1 + w_end*r_end ;
      phi = w_start0*phi_start0 + w_start1*phi_start1 + w_end*phi_end ;
      z = w_start0*z_start0 + w_start1*z_start1 + w_end*z_end ;
      Mesh->Ring[i].Vertex[j].SetCylCoords(r, phi, z) ;    
    }
  }

  // finally calculate the cartesian coords of each vertex
  for (i = 0 ;  i < Mesh->NumRings ;  i++){
    for (j = 0 ;  j < Mesh->Ring[i].NumVerts ;  j++){
      Mesh->Ring[i].Vertex[j].GetCylCoords(&r, &phi, &z) ;
      CalcCartesianCoords(r, phi, z, HoleCentre, ExtrusionVector, UpVector, x) ;
      Mesh->Ring[i].Vertex[j].SetCartCoords(x) ;
    }
  }

  holepts->Delete() ;
}




//------------------------------------------------------------------------------
// Transfer vertices to vtkPoints
// This allocates memory for points.
void vtkMEDExtrudeToCircle::VerticesToVtkPoints(vtkPoints *points) const
//------------------------------------------------------------------------------
{
  int i, j, k, totalpts ;

  // pre-allocate memory for total no. of vertices
  for (i = 0, totalpts = 0 ;  i < Mesh->NumRings ;  i++)
    totalpts += Mesh->Ring[i].NumVerts ;
  points->Allocate(totalpts) ;       // allocate space for n points (ie 3 * n floats)

  for (i = 0, k = 0 ;  i < Mesh->NumRings ;  i++){
    for (j = 0 ;  j < Mesh->Ring[i].NumVerts ;  j++){
      // insert vertex into points array
      double x[3] ;
      Mesh->Ring[i].Vertex[j].GetCartCoords(x) ;
      points->InsertPoint(k, x) ; 

      // make a note of the polydata index so we can find it again
      Mesh->Ring[i].Vertex[j].SetId(k++) ;     
    }
  }

  points->Squeeze() ;
}



//------------------------------------------------------------------------------
//Sort vertices into vtk triangles
//This allocates memory for triangles
void vtkMEDExtrudeToCircle::VerticesToVtkTriangles(vtkCellArray *triangles) const
//------------------------------------------------------------------------------
{
  int i, j, totalTriangles ;
  vtkIdType id0, id1, id2, thisTriangle[3] ;

  // pre-allocate memory for total no. of vertices
  for (i = 0, totalTriangles = 0 ;  i < Mesh->NumRings-1 ;  i++)
    totalTriangles += Mesh->Ring[i].NumVerts + Mesh->Ring[i+1].NumVerts ;
  triangles->Allocate(3*totalTriangles) ;       // allocate space for indices 

  for (i = 0 ;  i < Mesh->NumRings-1 ;  i++){
    int nthis = Mesh->Ring[i].NumVerts ;      // no. vertices in this ring
    int nnext = Mesh->Ring[i+1].NumVerts ;    // no. vertices in previous ring
    double nratio = (double)nnext / (double)nthis ;

    // loop through the vertices
    for (j = 0 ;  j < Mesh->Ring[i].NumVerts ;  j++){
      // get the range of vertices which will be connected to this one on the next ring
      // n.b. need to use floor() because (int) moves negative numbers towards zero.
      int jjminus = floor(nratio * (double)(j-0.5) + 0.5) ;
      int jjplus = floor(nratio * (double)(j+0.5) + 0.5) ;

      // spray triangles forward from {ring i, vertex j} to {ring i+1, vertices jjminus to jjplus}
      for (int jj = jjminus ;  jj < jjplus ;  jj++){
        int jj0 = Modulo(jj, nnext) ;
        int jj1 = Modulo(jj+1, nnext) ;

        // construct triangle.
        // this goes anticlock if ring index goes left to right and j axis points down.
        id0 = Mesh->Ring[i].Vertex[j].GetId() ;
        id1 = Mesh->Ring[i+1].Vertex[jj1].GetId() ;
        id2 = Mesh->Ring[i+1].Vertex[jj0].GetId() ;

        thisTriangle[0] = id0 ; thisTriangle[1] = id1 ; thisTriangle[2] = id2 ;
        triangles->InsertNextCell(3, thisTriangle) ;
      }

      // add the triangle which has its baseline on the current ring
      int j1 = Modulo(j+1, nthis) ;
      int jjp = Modulo(jjplus, nnext) ;

      id0 = Mesh->Ring[i].Vertex[j].GetId() ;
      id1 = Mesh->Ring[i].Vertex[j1].GetId() ;
      id2 = Mesh->Ring[i+1].Vertex[jjp].GetId() ;

      thisTriangle[0] = id0 ; thisTriangle[1] = id1 ; thisTriangle[2] = id2 ;
      triangles->InsertNextCell(3, thisTriangle) ;
    }
  }

  triangles->Squeeze() ;
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
  double dp = DotProduct(upVector, w) ;
  u[0] = upVector[0] - dp*w[0] ;
  u[1] = upVector[1] - dp*w[1] ;
  u[2] = upVector[2] - dp*w[2] ;
  Normalize(u, u) ;

  // make sure u has same sense as input up vector
  dp = DotProduct(u, upVector) ;
  if (dp < 0)
    MultVectorByScalar(-1.0, u, u) ;

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

  // put phi into range 0 to 2pi
  if (*phi < 0.0)
    *phi += 2*M_PI ;

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
  double dp = DotProduct(upVector, w) ;
  u[0] = upVector[0] - dp*w[0] ;
  u[1] = upVector[1] - dp*w[1] ;
  u[2] = upVector[2] - dp*w[2] ;
  Normalize(u, u) ;

  // make sure u has same sense as input up vector
  dp = DotProduct(u, upVector) ;
  if (dp < 0)
    MultVectorByScalar(-1.0, u, u) ;

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
// Checks sense of points as calculated by GetPointsAroundHole()
// Returns 1 if phi increases with index
// Returns -1 if phi decreases with index
int vtkMEDExtrudeToCircle::CalcSenseOfPointsAroundHole(vtkIdList *holepts, const double *centre, const double *cylAxis, const double *upVector) const
//------------------------------------------------------------------------------
{
  int j ;
  double r, phi, z ;
  double *x ;

  int n ;
  double mplus, mneg, Eplus, Eneg ;
  n = holepts->GetNumberOfIds() ;

  // We define two models of the line phi = m*j = c
  // with m = +/- 2pi/n 
  // and c = 0
  mplus = 2.0*M_PI / (double)n ;
  mneg = -mplus ;

  for (j = 0, Eplus = 0.0, Eneg = 0.0 ;  j < n ;  j++){
    vtkIdType ptId = holepts->GetId(j) ;
    x = Input->GetPoint(ptId) ;  // cartesian coords of hole point
    CalcCylinderCoords(x, HoleCentre, ExtrusionVector, UpVector, &r, &phi, &z) ;

    // Get calculated phi for each line
    double phicalc_plus = mplus*(double)j ;
    double phicalc_neg = mneg*(double)j ;

    // Make sure the calculated phi is in the range 0-2pi, the same as the input data.
    // NB if we got the slope of the line by least squares, it would be hard to account for this.
    if (phicalc_neg < 0.0)
      phicalc_neg += 2.0*M_PI ;

    // calculate deviations from lines
    double dphi_plus = phi - phicalc_plus ;
    double dphi_neg = phi - phicalc_neg ;

    // add to sum squared deviations for each line
    Eplus += dphi_plus*dphi_plus ;
    Eneg += dphi_neg*dphi_neg ;
  }

  if (Eplus < Eneg)
    return 1 ;
  else
    return -1 ;

}



//------------------------------------------------------------------------------
// This removes wrap around jumps of 2pi from phi in the first ring
void vtkMEDExtrudeToCircle::Remove2PiArtefactsFromFirstRing()
//------------------------------------------------------------------------------
{
  int j ;
  int n = Mesh->Ring[0].NumVerts ;

  // slope of model line phi = m*j + c where c = 0
  double m = 2.0*M_PI / (double)n ;

  for (j = 0 ;  j < n ;  j++){
    double phi = Mesh->Ring[0].Vertex[j].GetCylPhi() ;
    double phicalc = m * (double)j ;
    double dphi = phi - phicalc ;

    // add or subtract 2pi to improve value
    if (dphi > M_PI)
      phi -= 2.0*M_PI ;
    else if (dphi < -M_PI)
      phi += 2.0*M_PI ;

    Mesh->Ring[0].Vertex[j].SetCylPhi(phi) ;
  }
}



//------------------------------------------------------------------------------
// Reverse id list
void vtkMEDExtrudeToCircle::ReverseIdList(vtkIdList *pts) const
//------------------------------------------------------------------------------
{
  int j, jj ;
  int n = pts->GetNumberOfIds() ;

  for (j = 0, jj = n-1 ;  j < n/2 ;  j++, jj--){
    vtkIdType id1 = pts->GetId(j) ;
    vtkIdType id2 = pts->GetId(jj) ;
    pts->InsertId(j, id2) ;
    pts->InsertId(jj, id1) ;
  }
}


//------------------------------------------------------------------------------
// Circular shift id list
void vtkMEDExtrudeToCircle::CircularShiftIdList(vtkIdList *pts, int shift) const
//------------------------------------------------------------------------------
{
  int j, jj ;
  int n = pts->GetNumberOfIds() ;

  // copy to temp list
  vtkIdList *temp = vtkIdList::New() ;
  temp->Initialize() ;
  for (j = 0 ;  j < n ;  j++)
    temp->InsertNextId(pts->GetId(j)) ;

  for (j = 0 ;  j < n ;  j++){
    jj = Modulo(j-shift, n) ;
    pts->InsertId(j, temp->GetId(jj)) ;
  }

  temp->Delete() ;
}


//------------------------------------------------------------------------------
// Modulo operator, same as % but works correctly on negative values of n as well
inline int vtkMEDExtrudeToCircle::Modulo(int n, int m) const
//------------------------------------------------------------------------------
{
  if (n >= 0)
    return n % m ;
  else
    return m - (-n % m) ;
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
  os << indent << "no. pts" << GetHoleNumVerts() << std::endl ;
  os << std::endl ;


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

  os << indent << "mesh structure..." << std::endl ;
  os << indent << "requested end pts: " << MinNumEndPts << std::endl ; 
  Mesh->PrintSelf(os, indent) ;
  os << std::endl ;
}



//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::MeshData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "no. rings = " << NumRings << std::endl ;

  for (int i = 0 ;  i < NumRings ;  i++){
    os << indent << "ring " << i << "\t" ;
    Ring[i].PrintSelf(os, 0) ;
    os << std::endl ;
  }
}


//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::RingData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "no. vertices = " << NumVerts << "\t" << "z = " << Z << std::endl ;
  for (int j = 0 ;  j < NumVerts ;  j++){
    os << indent << "vertex " << j << "\t" ;
    Vertex[j].PrintSelf(os, 0) ;
  }
}


//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::VertexData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "cyl " << Cylcoord[0] << "\t" << Cylcoord[1] << "\t" << Cylcoord[2] << "\t"
    << "cart " << Cartcoord[0] << "\t" << Cartcoord[1] << "\t" << Cartcoord[2] << std::endl ;
}
