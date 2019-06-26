/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkALBACollisionDetectionFilter.cxx
  Language:  C++
  RCS:   $Id: vtkALBACollisionDetectionFilter.cxx,v 1.1.2.2 2012-02-22 12:53:16 ior02 Exp $

  Copyright (c) 2003-2004 Goodwin Lawlor
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.

=========================================================================*/
#include "vtkALBACollisionDetectionFilter.h"
#include "vtkObjectFactory.h"
#include "vtkOBBTree.h"
#include "vtkMatrix4x4.h"
#include "vtkIdList.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkCommand.h"
#include "vtkPoints.h"
#include "vtkLookupTable.h"
#include "vtkUnsignedCharArray.h"
#include "vtkIdTypeArray.h"
#include "vtkCellData.h"
#include "vtkBox.h"
#include "vtkTriangle.h"
#include "vtkLine.h"
#include "vtkPlane.h"
#include "vtkMath.h"
#include "vtkCommand.h"
#include "vtkPointData.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkTransform.h"
#include "vtkALBASmartPointer.h"
#include "vtkCellArray.h"

vtkCxxRevisionMacro(vtkALBACollisionDetectionFilter, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkALBACollisionDetectionFilter);

// Constructs with initial 0 values.
vtkALBACollisionDetectionFilter::vtkALBACollisionDetectionFilter()
{
  vtkDebugMacro(<< "Initializing object");

  // Ask the superclass to set the number of connections.
  this->SetNumberOfInputs(2);
  this->SetNumberOfOutputs(3);
  this->vtkSource::SetNthOutput(1, vtkPolyData::New());
  // Releasing data for pipeline parallism.
  // Filters will know it is empty. 
  this->Outputs[1]->ReleaseData();
  this->Outputs[1]->Delete();
  this->vtkSource::SetNthOutput(2, vtkPolyData::New());
  // Releasing data for pipeline parallism.
  // Filters will know it is empty. 
  this->Outputs[2]->ReleaseData();
  this->Outputs[2]->Delete();

  this->Transform[0] = NULL;
  this->Transform[1] = NULL;
  this->Matrix[0] = NULL;
  this->Matrix[1] = NULL;
  this->NumberOfBoxTests = 0;
  this->BoxTolerance = 0.0;
  this->CellTolerance = 0.0;
  this->NumberOfCellsPerBucket = 10;
  this->GenerateScalars = 0;
  this->CollisionMode = VTK_ALL_CONTACTS;
  this->Opacity = 1.0;
  this->Tree[0] = NULL;
  this->Tree[1] = NULL;

  this->Tree[0] = vtkOBBTree::New();
  this->Tree[1] = vtkOBBTree::New();
}

// Destroy any allocated memory.
vtkALBACollisionDetectionFilter::~vtkALBACollisionDetectionFilter()
{

//   if (this->Matrix[0])
//     {
//     this->Matrix[0]->UnRegister(this);
//     this->Matrix[0] = NULL;
//     }
//   if (this->Matrix[1])
//     {
//     this->Matrix[1]->UnRegister(this);
//     this->Matrix[1] = NULL;
//     }
  if (this->Transform[0])
    {
    this->Transform[0]->UnRegister(this);
    this->Transform[0] = NULL;
    }
  if (this->Transform[1])
    {
    this->Transform[1]->UnRegister(this);
    this->Transform[1] = NULL;
    }

  Tree[0]->Delete();
  Tree[1]->Delete();

}


// Description:
// Set and Get the input data...
void vtkALBACollisionDetectionFilter::SetInput(int idx, vtkPolyData *input)
{

  if (2 <= idx || idx < 0)
    {
    vtkErrorMacro(<< "Index " << idx 
      << " is out of range in SetInput. Only two inputs allowed!");
    }
    
  // Ask the superclass to connect the input.
  this->SetNthInput(idx, input);

  Tree[idx]->SetDataSet(input);
  Tree[idx]->AutomaticOn();
  Tree[idx]->SetNumberOfCellsPerBucket(this->NumberOfCellsPerBucket);
  Tree[idx]->BuildLocator();
  Tree[idx]->SetTolerance(this->BoxTolerance);
}




//----------------------------------------------------------------------------
vtkPolyData *vtkALBACollisionDetectionFilter::GetInput(int idx)
{
  if (2 <= idx || idx < 0)
    {
    vtkErrorMacro(<< "Index " << idx 
      << " is out of range in GetInput. Only two inputs allowed!");
    return NULL;
    }
  
  return vtkPolyData::SafeDownCast(this->Inputs[idx]);
}

vtkIdTypeArray *vtkALBACollisionDetectionFilter::GetContactCells(int i)
{
  if (2 <= i || i < 0)
    {
    vtkErrorMacro(<< "Index " << i 
      << " is out of range in GetContactCells. There are only two contact cells arrays!");
    return NULL;
    }

  return vtkIdTypeArray::SafeDownCast(
    this->GetOutput(i)->GetFieldData()->GetArray("ContactCells"));
  
}

void vtkALBACollisionDetectionFilter::SetTransform(int i, vtkLinearTransform *transform)
{
  if (i > 1 || i < 0)
    {
    vtkErrorMacro(<< "Index " << i 
    << " is out of range in SetTransform. Only two transforms allowed!");
    }

  if (transform == this->Transform[i])
    {
    return;
    }
    
  if(this->Transform[i]) 
    {
    this->Transform[i]->Delete();
    this->Transform[i] = NULL;
    }
//     
//   if (this->Matrix[i])
//     {
//     this->Matrix[i]->Delete();
//     this->Matrix[i] = NULL;
//     } 
  
  if(transform)
    {
    this->Transform[i] = transform;
    this->Transform[i]->Register(this);
    this->Matrix[i] = transform->GetMatrix();
    this->Matrix[i]->Register(this);
    }

  this->Modified();
}



void vtkALBACollisionDetectionFilter::SetMatrix(int i, vtkMatrix4x4 *matrix)
{
  if (i > 1 || i < 0)
    {
    vtkErrorMacro(<< "Index " << i 
      << " is out of range in SetMatrix. Only two matrices allowed!");
    }

  if (matrix == this->Matrix[i])
    {
    return;
    }

  if (this->Transform[i])
    {
    this->Transform[i]->Delete();
    this->Transform[i] = NULL;
    }

//   if(this->Matrix[i]) 
//     {
//     this->Matrix[i]->Delete();
//     this->Matrix[i] = NULL;
//     }
    
  vtkDebugMacro(<< "Setting matrix: " << i << " to point to " << matrix << endl);

  if(matrix)
    {
    this->Matrix[i] = matrix;
    vtkMatrixToLinearTransform *transform = vtkMatrixToLinearTransform::New();
    // Consistent Register and UnRegisters.
//     transform->Register(this);
//     transform->Delete();
    transform->SetInput(matrix);

    this->Transform[i] = transform;
    vtkDebugMacro(<< "Setting Transform " << i << " to points to: " << transform << endl);
    }
  this->Modified();
}

vtkMatrix4x4* vtkALBACollisionDetectionFilter::GetMatrix(int i)
{
  if (this->Transform[i]) 
    { 
    this->Transform[i]->Update(); 
    }
  return this->Matrix[i]; 
}

static int ComputeCollisions(vtkOBBNode *nodeA, vtkOBBNode *nodeB, vtkMatrix4x4 *Xform, void *clientdata)
{
  // This is hard-coded for triangles but could be easily changed to allow for allow n-sided polygons
  int numIdsA, numIdsB;
  vtkIdList *IdsA, *IdsB, *pointIdsA, *pointIdsB;
  vtkCellArray *cells;
  vtkIdType cellPtIds[2];
  vtkIdTypeArray *contactcells1, *contactcells2;
  vtkPoints *contactpoints;
  IdsA  = nodeA->Cells;
  IdsB = nodeB->Cells;
  numIdsA = IdsA->GetNumberOfIds();
  numIdsB = IdsB->GetNumberOfIds();

  // clientdata is a pointer to this object... need to cast it as such
  vtkALBACollisionDetectionFilter* self = reinterpret_cast<vtkALBACollisionDetectionFilter *>( clientdata );
  
  // Turn off debugging here if its on... otherwise there's squawks every update/box test
  int DebugWasOn = 0;
  int FirstContact = 0;
  if (self->GetDebug())
    {
    self->DebugOff();
    DebugWasOn = 1;
    }    
  vtkPolyData *inputA = vtkPolyData::SafeDownCast(self->GetInput(0));
  vtkPolyData *inputB = vtkPolyData::SafeDownCast(self->GetInput(1));
  contactcells1 = self->GetContactCells(0);
  contactcells2 = self->GetContactCells(1);
  contactpoints = self->GetOutput(2)->GetPoints();

  if (self->GetCollisionMode() == vtkALBACollisionDetectionFilter::VTK_ALL_CONTACTS)
    {
    cells = self->GetOutput(2)->GetLines();
    }
  else
    {
    cells = self->GetOutput(2)->GetVerts();
    }

  float Tolerance = self->GetCellTolerance();
  if (self->GetCollisionMode() == vtkALBACollisionDetectionFilter::VTK_FIRST_CONTACT) 
    {
    FirstContact = 1;
    }
  
  
  vtkIdType cellIdA, cellIdB;
  
  double x1[4], x2[4], xnew[4];
  double ptsA[9], ptsB[9];
  double boundsA[6], boundsB[6];
  vtkIdType i,j,k,m,n,p,v;
  double *point, in[4], out[4];

  // Loop thru the cells/points in IdsA
  for (i = 0; i < numIdsA; i++) 
   {
    cellIdA = IdsA->GetId(i);
    pointIdsA = inputA->GetCell(cellIdA)->GetPointIds();
    inputA->GetCellBounds(cellIdA, boundsA);

    // Initialize ptsA
    // It might speed things up if ptsA and ptsB only had to be an
    // array of pointers to the cell vertices, rather than allocating here.
    for (j=0; j<3; j++)
      {
      for (k=0; k<3; k++)
        {
        ptsA[j*3+k] = inputA->GetPoints()->GetPoint(pointIdsA->GetId(j))[k];
        }
      }

    // Loop thru each cell IdsB and test for collision  
    for (m = 0; m < numIdsB; m++)
      {
      cellIdB = IdsB->GetId(m);
      pointIdsB = inputB->GetCell(cellIdB)->GetPointIds();
      inputB->GetCellBounds(cellIdB, boundsB);
      
      // Initialize ptsB
      for (n=0; n<3; n++)
        {
        point = inputB->GetPoints()->GetPoint(pointIdsB->GetId(n));
        // transform the vertex
        in[0] = point[0]; in[1] = point[1]; in[2] = point[2]; in[3] = 1.0;
        Xform->MultiplyPoint( in, out );
        out[0] = out[0]/out[3];
        out[1] = out[1]/out[3];
        out[2] = out[2]/out[3];
        for (p=0; p<3; p++) 
          {
          ptsB[n*3+p] = out[p];
          }
        }
        
      //Calculate the bounds for the xformed cell
      boundsB[0] = boundsB[2] = boundsB[4] =  VTK_LARGE_FLOAT;
      boundsB[1] = boundsB[3] = boundsB[5] = -VTK_LARGE_FLOAT;
      for (v=0; v < 9; v=v+3)
        {
        if (ptsB[v] < boundsB[0]) boundsB[0] = ptsB[v];
        if (ptsB[v] > boundsB[1]) boundsB[1] = ptsB[v];
        if (ptsB[v+1] < boundsB[2]) boundsB[2] = ptsB[v+1];
        if (ptsB[v+1] > boundsB[3]) boundsB[3] = ptsB[v+1];
        if (ptsB[v+2] < boundsB[4]) boundsB[4] = ptsB[v+2];
        if (ptsB[v+2] > boundsB[5]) boundsB[5] = ptsB[v+2];
        }
      
      // Test for intersection     
      if (self->IntersectPolygonWithPolygon(3, ptsA, boundsA, 3, ptsB, boundsB, 
        Tolerance, x1, x2, self->GetCollisionMode()))
        {
        contactcells1->InsertNextValue(cellIdA);
        contactcells2->InsertNextValue(cellIdB);
        //transform x back to "world space"
        // could speed this up by testing for identity matrix
        // and skipping the next transform.
        x1[3] = x2[3] = 1.0;
        self->GetMatrix(0)->MultiplyPoint(x1,xnew);
        xnew[0] = xnew[0]/xnew[3];
        xnew[1] = xnew[1]/xnew[3];
        xnew[2] = xnew[2]/xnew[3];
        cellPtIds[0] = contactpoints->InsertNextPoint(xnew);
        if (self->GetCollisionMode() == vtkALBACollisionDetectionFilter::VTK_ALL_CONTACTS)
          {
          self->GetMatrix(0)->MultiplyPoint(x2,xnew);
          xnew[0] = xnew[0]/xnew[3];
          xnew[1] = xnew[1]/xnew[3];
          xnew[2] = xnew[2]/xnew[3];
          cellPtIds[1] = contactpoints->InsertNextPoint(xnew);
          // insert a new line
          cells->InsertNextCell(2, cellPtIds);
          }
        else
          {
          // insert a new vert
          cells->InsertNextCell(1, cellPtIds);
          }


        if (FirstContact)
          {
          // return the negative of the number of box tests to find first contact
          // this will call a halt to the proceedings
          if (DebugWasOn) self->DebugOn();
          return (-1 - self->GetNumberOfBoxTests());
          }
        }
        
      }
    }
  if (DebugWasOn) self->DebugOn(); 
  return 1;
}

// Description:
// Perform a collision detection
void vtkALBACollisionDetectionFilter::Execute()
{
  // get the info objects
  vtkDebugMacro(<< "Beginning execution...");

  // inputs and outputs
  vtkPolyData *input[2];
  vtkPolyData *output[3];

  // copy inputs to outputs
  for (int i=0; i<2; i++)
  {
    input[i] = vtkPolyData::SafeDownCast(Inputs[i]);
    if (input[i])
    {
	    output[i] = vtkPolyData::SafeDownCast(this->GetOutput(i));
	    output[i]->CopyStructure(input[i]);
	    output[i]->GetPointData()->PassData(input[i]->GetPointData());
	    output[i]->GetCellData()->PassData(input[i]->GetCellData());
	    output[i]->GetFieldData()->PassData(input[i]->GetFieldData());
    }
    else
    {
      return;
    }
  }
  
  // set up the contacts polydata output on port index 2
  output[2] = vtkPolyData::SafeDownCast(this->Outputs[2]);
  vtkPoints *contactsPoints = vtkPoints::New();
  output[2]->SetPoints(contactsPoints);
  contactsPoints->Delete();

  if (this->CollisionMode == vtkALBACollisionDetectionFilter::VTK_ALL_CONTACTS)
    {//then create a lines cell array
    vtkCellArray *lines = vtkCellArray::New();
    output[2]->SetLines(lines);
    lines->Delete();
    }
  else
    {//else create a verts cell array
    vtkCellArray *verts = vtkCellArray::New();
    output[2]->SetVerts(verts);
    verts->Delete();
    }

  //Allocate arrays for the contact cells lists
  vtkIdTypeArray *contactcells0 = vtkIdTypeArray::New();
  contactcells0->SetName("ContactCells");
  output[0]->GetFieldData()->AddArray(contactcells0);
  contactcells0->Delete();

  vtkIdTypeArray *contactcells1 = vtkIdTypeArray::New();
  contactcells1->SetName("ContactCells");
  output[1]->GetFieldData()->AddArray(contactcells1);
  contactcells1->Delete();

  // make sure input is available
  if ( ! input[0] )
    {
    vtkWarningMacro(<< "Input 1 hasn't been added... can't execute!");
    return;
    }

  // make sure input is available
  if ( ! input[1] )
    {
    vtkWarningMacro(<< "Input 2 hasn't been added... can't execute!");
    return;
    }
    
  // The transformations...
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  vtkMatrix4x4 *tmpMatrix = vtkMatrix4x4::New();

  if (this->Transform[0] != NULL || this->Transform[1] != NULL)
    {
    vtkMatrix4x4::Invert(this->Transform[0]->GetMatrix(), tmpMatrix);
    // the sequence of multiplication is significant
    vtkMatrix4x4::Multiply4x4(tmpMatrix, this->Transform[1]->GetMatrix(), matrix);
    }
   else
    {
     vtkWarningMacro(<< "Set two transforms or two matrices");
     return;
    }

  this->InvokeEvent(vtkCommand::StartEvent, NULL);
  

  // rebuild the obb trees... they do their own mtime checking with input data
//   vtkOBBTree *Tree0 = vtkOBBTree::New();
//   vtkOBBTree *Tree1 = vtkOBBTree::New();
//   Tree0->SetDataSet(input[0]);
//   Tree0->AutomaticOn();
//   Tree0->SetNumberOfCellsPerBucket(this->NumberOfCellsPerBucket);
//   Tree0->BuildLocator();
// 
//   Tree1->SetDataSet(input[1]);
//   Tree1->AutomaticOn();
//   Tree1->SetNumberOfCellsPerBucket(this->NumberOfCellsPerBucket);
//   Tree1->BuildLocator();
//     
//   // Set the Box Tolerance
//   Tree0->SetTolerance(this->BoxTolerance);
//   Tree1->SetTolerance(this->BoxTolerance);

  // Do the collision detection...
  vtkIdType BoxTests = Tree[0]->IntersectWithOBBTree(Tree[1],  matrix, ComputeCollisions, this);

//   Tree0->Delete();
//   Tree1->Delete();

  matrix->Delete();
  tmpMatrix->Delete();

  vtkDebugMacro(<< "Collision detection finished");
  this->NumberOfBoxTests = abs(BoxTests);
  
  // Generate the scalars if needed
  if (GenerateScalars)
    {

    for (int idx =0; idx < 2; idx++)
      {

      vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
      output[idx]->GetCellData()->SetScalars(scalars);
      vtkIdType numCells = input[idx]->GetNumberOfCells();
      scalars->SetNumberOfComponents(4);
      scalars->SetNumberOfTuples(numCells);
      vtkIdTypeArray *contactcells = this->GetContactCells(idx);
      vtkIdType numContacts = this->GetNumberOfContacts();
      
      // Fill the array with blanks...
      // Maybe this should change, to alpha set to Opacity
      // regardless if there are contact or not.
      float alpha;
      if (numContacts > 0)
        {
        alpha = this->Opacity*255.0;
        }
      else
        {
        alpha = 255.0;
        }
      float blank[4] = {255.0,255.0,255.0,alpha};

      for (vtkIdType i = 0; i < numCells; i++)
        {
        scalars->SetTuple(i, blank);
        }

      // Now color the intersecting cells 
      vtkLookupTable *lut = vtkLookupTable::New();
      if (numContacts>0)
        {
        if (this->CollisionMode == VTK_ALL_CONTACTS)
          {
          lut->SetTableRange(0, numContacts-1);
          lut->SetNumberOfTableValues(numContacts);
          }
        else // VTK_FIRST_CONTACT
          {
          lut->SetTableRange(0, 1);
          lut->SetNumberOfTableValues(numContacts+1);
          }
        lut->Build();
        }
      double *RGBA;
      float RGB[4];

      for (vtkIdType id, i = 0; i < numContacts; i++)
        {
        id = contactcells->GetValue(i);
        RGBA = lut->GetTableValue(i);
        RGB[0] = 255.0*RGBA[0];
        RGB[1] = 255.0*RGBA[1];
        RGB[2] = 255.0*RGBA[2];
        RGB[3] = 255.0;
        scalars->SetTuple(id, RGB);
        }

      lut->Delete();
      scalars->Delete();
      vtkDebugMacro(<< "Created scalars on output " << idx);
      }
    }
    
  this->InvokeEvent(vtkCommand::EndEvent, NULL);

  return;

}

// Method intersects two polygons. You must supply the number of points and
// point coordinates (npts, *pts) and the bounding box (bounds) of the two
// polygons. Also supply a tolerance squared for controlling
// error. The method returns 1 if there is an intersection, and 0 if
// not. A single point of intersection x[3] is also returned if there
// is an intersection.
int vtkALBACollisionDetectionFilter::IntersectPolygonWithPolygon(int npts, double *pts, double bounds[6],
                                            int npts2, double *pts2, 
                                            double bounds2[6], double tol2,
                                            double x1[3], double x2[3], int CollisionMode)
{
  double n[3], n2[3], coords[3];
  int i, j;
  double *p1, *p2, *q1, ray[3], ray2[3];
  double t,u,v;
  double *x[2];
  int Num = 0;
  x[0] = x1;
  x[1] = x2;

  //  Intersect each edge of first polygon against second
  //
  vtkPolygon::ComputeNormal(npts2, pts2, n2);
  vtkPolygon::ComputeNormal(npts, pts, n);

  int parallel_edges=0;
  for (i=0; i<npts; i++) 
    {
    p1 = pts + 3*i;
    p2 = pts + 3*((i+1)%npts);

    for (j=0; j<3; j++)
      {
      ray[j] = p2[j] - p1[j];
      }
    if ( ! vtkBox::IntersectBox(bounds2, p1, ray, coords, t) )
      {
      continue;
      }

    if ( (vtkPlane::IntersectWithLine(p1,p2,n2,pts2,t,x[Num])) == 1 ) 
      {
      if ( (npts2==3
            && vtkTriangle::PointInTriangle(x[Num],pts2,pts2+3,pts2+6,tol2))
           || (npts2>3
               && vtkPolygon::PointInPolygon(x[Num],npts2,pts2,bounds2,n2)
               == 1))
        {
        Num++;
        if (CollisionMode != vtkALBACollisionDetectionFilter::VTK_ALL_CONTACTS ||
          Num == 2)
          {
          return 1;
          }
        }
      } 
    else
      {
       // cout << "Test for overlapping" << endl;
       // test to see if cells are coplanar and overlapping...
       parallel_edges++;
       if (parallel_edges >1) // cells are parallel then...
        {
        //cout << "cells are parallel" << endl;
        // test to see if they are coplanar
        q1 = pts2;
        for (j=0; j<3; j++)
          {
          ray2[j] = p1[j] - q1[j];
          }
        if (vtkMath::Dot(n,ray2) == 0.0) // cells are coplanar
          {
          //cout << "cells are coplanar" << endl;
          // test to see if coplanar cells overlap
          // ie, if one of the tris has a vertex in the other
          for (int ii=0; ii < npts; ii++)
            {
            for (int jj=0; jj < npts2; jj++)
              {
              if (vtkLine::Intersection(pts+3*ii,pts+3*((ii+1)%npts),
              pts2+3*jj,pts2+3*((jj+1)%npts2),u,v) > 0)
                {
                //cout << "Found an overlapping one!!!" << endl;
                for (int k=0;k<3;k++)
                  {
                  x[Num][k] = pts[k+3*ii] + u*(pts[k+(3*((ii+1)%npts))]-pts[k+3*ii]);
                  }
                Num++;
                if (CollisionMode != vtkALBACollisionDetectionFilter::VTK_ALL_CONTACTS ||
                  Num == 2)
                  {
                  return 1;
                  }
                }            
              }
             }
             
           } // end if cells are coplanar
         } // end if cells are parallel
        } // end else
     }
       
       
  //  Intersect each edge of second polygon against first
  //

  for (i=0; i<npts2; i++) 
    {
    p1 = pts2 + 3*i;
    p2 = pts2 + 3*((i+1)%npts2);

    for (j=0; j<3; j++)
      {
      ray[j] = p2[j] - p1[j];
      }

    if ( ! vtkBox::IntersectBox(bounds, p1, ray, coords, t) )
      {
      continue;
      }

    if ( (vtkPlane::IntersectWithLine(p1,p2,n,pts,t,x[Num])) == 1 ) 
      {
      if ( (npts==3 && vtkTriangle::PointInTriangle(x[Num],pts,pts+3,pts+6,tol2))
        || (npts>3 && vtkPolygon::PointInPolygon(x[Num],npts,pts,bounds,n)
               == 1))
        {
        Num++;
        if (CollisionMode != vtkALBACollisionDetectionFilter::VTK_ALL_CONTACTS ||
          Num == 2)
          {
          return 1;
          }
        }
      } 
    }
  
  //if we get through to here then there's no collision.
  return 0;
}


// Description:
// Make sure filter executes if transform are changed
unsigned long vtkALBACollisionDetectionFilter::GetMTime()
{
  unsigned long mTime=this->MTime.GetMTime();
  unsigned long transMTime, matrixMTime;

  if ( this->Transform[0] )
    {
    transMTime = this->Transform[0]->GetMTime();
    mTime = ( transMTime > mTime ? transMTime : mTime );
    }
    
  if ( this->Transform[1] )
    {
    transMTime = this->Transform[1]->GetMTime();
    mTime = ( transMTime > mTime ? transMTime : mTime );
    }
    
  if ( this->Matrix[0] )
    {
    matrixMTime = this->Matrix[0]->GetMTime();
    mTime = ( matrixMTime > mTime ? matrixMTime : mTime );
    }
    
  if ( this->Matrix[1] )
    {
    matrixMTime = this->Matrix[1]->GetMTime();
    mTime = ( matrixMTime > mTime ? matrixMTime : mTime );
    }
    
  return mTime;
}


void vtkALBACollisionDetectionFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "Box Tolerance: " << this->BoxTolerance << "\n";
  os << indent << "Cell Tolerance: " << this->CellTolerance << "\n";
  os << indent << "Number of cells per bucket: " << this->NumberOfCellsPerBucket << "\n";

}

int vtkALBACollisionDetectionFilter::GetNumberOfContacts()
{
  return this->GetOutput(0)->GetFieldData()->GetArray("ContactCells")->GetNumberOfTuples();
}
