/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkRayCast3DPicker.cxx,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:49:25 $
  Version:   $Revision: 1.1 $
  Authors:   Michele Diegoli
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "vtkRayCast3DPicker.h"

#include "vtkActor.h"
#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkGenericCell.h"
#include "vtkIdType.h"
#include "vtkImageData.h"
#include "vtkLODProp3D.h"
#include "vtkMapper.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkProp3DCollection.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkVertex.h"
#include "vtkVolume.h"
#include "vtkVolumeMapper.h"

//------------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkRayCast3DPicker, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkRayCast3DPicker);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Construct object with initial tolerance of 1/40th of window. There are no
// pick methods and picking is performed from the renderer's actors.
vtkRayCast3DPicker::vtkRayCast3DPicker()
//------------------------------------------------------------------------------
{
  //this->Tolerance = 0.025; // 1/40th of the renderer window
  this->Tolerance = 0.01; // 1/100th of the renderer window
  PickTolerance = 0; // computed at pick time

  this->MapperPosition[0] = 0.0;
  this->MapperPosition[1] = 0.0;
  this->MapperPosition[2] = 0.0;

  this->Mapper = NULL;
  this->DataSet = NULL;
  this->GlobalTMin = VTK_LARGE_FLOAT;
  this->Actors = vtkActorCollection::New();
  this->Prop3Ds = vtkProp3DCollection::New();
  this->PickedPositions = vtkPoints::New();
  this->Transform = vtkTransform::New();
  this->Cell = vtkGenericCell::New();
}

//------------------------------------------------------------------------------
vtkRayCast3DPicker::~vtkRayCast3DPicker()
//------------------------------------------------------------------------------
{
  this->Actors->Delete();
  this->Prop3Ds->Delete();
  this->PickedPositions->Delete();
  this->Transform->Delete();
  Cell->Delete();
}

//------------------------------------------------------------------------------
// Update state when prop3D is picked.
void vtkRayCast3DPicker::MarkPicked(vtkAssemblyPath *path, vtkProp3D *prop3D, 
                           vtkAbstractMapper3D *m,
                           float tMin, float mapperPos[3])
//------------------------------------------------------------------------------
{
  int i;
  vtkMapper *mapper;
  vtkVolumeMapper *volumeMapper;

  this->SetPath(path);
  this->GlobalTMin = tMin;

  for (i=0; i < 3; i++) 
  {
    this->MapperPosition[i] = mapperPos[i];
  }
  if ( (mapper=vtkMapper::SafeDownCast(m)) != NULL )
  {
    this->DataSet = mapper->GetInput();
    this->Mapper = mapper;
  }
  else if ( (volumeMapper=vtkVolumeMapper::SafeDownCast(m)) != NULL )
  {
    this->DataSet = volumeMapper->GetInput();
    this->Mapper = volumeMapper;
  }
  else
  {
    this->DataSet = NULL;
  }

  // The point has to be transformed back into world coordinates.
  // Note: it is assumed that the transform is in the correct state.
  this->Transform->TransformPoint(mapperPos,this->PickPosition);
  
  // Invoke pick method if one defined - actor goes first
  prop3D->Pick();
  this->InvokeEvent(vtkCommand::PickEvent,NULL);

}

//------------------------------------------------------------------------------
int vtkRayCast3DPicker::Pick(float selectionX, float selectionY, float selectionZ, vtkRenderer *renderer)
//------------------------------------------------------------------------------
{
  return 0;
}

//------------------------------------------------------------------------------
// Perform pick operation with a segment provided! Points are expressed in the 
// world coordinate system. Return non-zero if something was successfully picked.
int vtkRayCast3DPicker::Pick(float *p1, float *p2, vtkRenderer *renderer)
//------------------------------------------------------------------------------
{
  int i;
  vtkProp *prop;
  //vtkCamera *camera;
  vtkAbstractMapper3D *mapper = NULL;
  float p1World[4], p2World[4], p1Mapper[4], p2Mapper[4];
  int picked=0;
  int *winSize;
  float x, y, t;
  float *viewport;
  float ray[3]/*, rayLength*/;
  int pickable;
  int LODId;
  float windowLowerLeft[4], windowUpperRight[4];
  float bounds[6], tol, hitPosition[3];

  float selectionPoint[3]; // selection point in display coordinates

   //  Initialize picking process
  this->Initialize();
  this->Renderer = renderer;

   if ( renderer == NULL )
   {
    vtkErrorMacro(<<"Must specify renderer!");
    return 0;
   }

  // Invoke start pick method if defined
  this->InvokeEvent(vtkCommand::StartPickEvent,NULL);

  for (i=0; i<3; i++) 
  {
    p1World[i] = p1[i];
    p2World[i] = p2[i];
  }
  
  // Homogeneous coordinates
  p1World[3] = p2World[3] = 1.0;

  // take the middle point as a reference for computing
  // the tolerance
  renderer->SetWorldPoint((p1World[0]+p2World[0])/2.0,
    (p1World[1]+p2World[1])/2.0,(p1World[2]+p2World[2])/2.0,1);
  renderer->WorldToDisplay();
  renderer->GetDisplayPoint(selectionPoint);

  // Compute the tolerance in world coordinates.  Do this by
  // determining the world coordinates of the diagonal points of the
  // window, computing the width of the window in world coordinates, and 
  // multiplying by the tolerance.
  viewport = renderer->GetViewport();
  winSize = renderer->GetRenderWindow()->GetSize();
  x = winSize[0] * viewport[0];
  y = winSize[1] * viewport[1];
  renderer->SetDisplayPoint(x, y, selectionPoint[2]);
  renderer->DisplayToWorld();
  renderer->GetWorldPoint(windowLowerLeft);
  
  x = winSize[0] * viewport[2];
  y = winSize[1] * viewport[3];
  renderer->SetDisplayPoint(x, y, selectionPoint[2]);
  renderer->DisplayToWorld();
  renderer->GetWorldPoint(windowUpperRight);
  
  for (tol=0.0,i=0; i<3; i++) 
  {
    tol += (windowUpperRight[i] - windowLowerLeft[i]) *
      (windowUpperRight[i] - windowLowerLeft[i]);
  }
  
  tol = sqrt (tol) * this->Tolerance;
  PickTolerance = tol;

  //  Loop over all props.  Transform ray (defined from position of
  //  camera to selection point) into coordinates of mapper (not
  //  transformed to actors coordinates!  Reduces overall computation!!!).
  //  Note that only vtkProp3D's can be picked by vtkRayCast3DPicker.
  //
  vtkPropCollection *props;
  vtkProp *propCandidate;
  if ( this->PickFromList ) 
  {
    props = this->GetPickList();
  }
  else 
  {
    props = renderer->GetProps();
  }

  vtkActor *actor;
  vtkLODProp3D *prop3D;
  vtkVolume *volume;
  vtkAssemblyPath *path;
  vtkProperty *tempProperty;
  this->Transform->PostMultiply();
  for ( props->InitTraversal(); (prop=props->GetNextProp()); )
  {
    for ( prop->InitPathTraversal(); (path=prop->GetNextPath()); )
    {
      pickable = 0;
      actor = NULL;
      propCandidate = path->GetLastNode()->GetProp();
      if ( propCandidate->GetPickable() && propCandidate->GetVisibility() )
      {
        pickable = 1;
        if ( (actor=vtkActor::SafeDownCast(propCandidate)) != NULL )
        {
          mapper = actor->GetMapper();
          if ( actor->GetProperty()->GetOpacity() <= 0.0 )
          {
            pickable = 0;
          }
        }
        else if ( (prop3D=vtkLODProp3D::SafeDownCast(propCandidate)) != NULL )
        {
          LODId = prop3D->GetPickLODID();
          mapper = prop3D->GetLODMapper(LODId);

          // if the mapper is a vtkMapper (as opposed to a vtkVolumeMapper), 
          // then check the transparency to see if the object is pickable
          if ( vtkMapper::SafeDownCast(mapper) != NULL)
          {
            prop3D->GetLODProperty(LODId, &tempProperty);
            if ( tempProperty->GetOpacity() <= 0.0 )
            {
              pickable = 0;
            }
          }
        }
        else if ( (volume=vtkVolume::SafeDownCast(propCandidate)) != NULL )
        {
          mapper = volume->GetMapper();
        }
        else
        {
          pickable = 0; //only vtkProp3D's (actors and volumes) can be picked
         }
      }
      //  If actor can be picked, get its composite matrix, invert it, and
      //  use the inverted matrix to transform the ray points into mapper
      //  coordinates. 
      if ( pickable  &&  mapper != NULL )
      {
        vtkMatrix4x4 *LastMatrix = path->GetLastNode()->GetMatrix();
        if (LastMatrix == NULL)
        {
          vtkErrorMacro (<< "Pick: Null matrix.");
          return 0;
        }
        this->Transform->SetMatrix(LastMatrix);
        this->Transform->Push();
        this->Transform->Inverse();

        this->Transform->TransformPoint(p1World,p1Mapper);
        this->Transform->TransformPoint(p2World,p2Mapper);

        for (i=0; i<3; i++) 
        {
          ray[i] = p2Mapper[i] - p1Mapper[i];
        }

        this->Transform->Pop();

        //  Have the ray endpoints in mapper space, now need to compare this
        //  with the mapper bounds to see whether intersection is possible.
        //
        //  Get the bounding box of the modeller.  Note that the tolerance is
        //  added to the bounding box to make sure things on the edge of the
        //  bounding box are picked correctly.
        mapper->GetBounds(bounds);
        bounds[0] -= tol; bounds[1] += tol; 
        bounds[2] -= tol; bounds[3] += tol; 
        bounds[4] -= tol; bounds[5] += tol;
        if ( vtkCell::HitBBox(bounds, (float *)p1Mapper, ray, hitPosition, t) )
        {
          t = this->IntersectWithLine((float *)p1Mapper, 
                                      (float *)p2Mapper, tol, path, 
                                      (vtkProp3D *)propCandidate, mapper);
          if ( t < VTK_LARGE_FLOAT )
          {
            picked = 1;
            this->Prop3Ds->AddItem((vtkProp3D *)prop);
            this->PickedPositions->InsertNextPoint
              ((1.0 - t)*p1World[0] + t*p2World[0],
               (1.0 - t)*p1World[1] + t*p2World[1],
               (1.0 - t)*p1World[2] + t*p2World[2]);

            // backwards compatibility: also add to this->Actors
            if (actor)
            {
              this->Actors->AddItem(actor);
            }
          }
        }//if HitBBox
      }//if visible and pickable not transparent and has mapper
    }//for all parts
  }//for all actors

  // Invoke end pick method if defined
  this->InvokeEvent(vtkCommand::EndPickEvent,NULL);

  return picked;
}

//------------------------------------------------------------------------------
//from vtkCellPicker: now I can select an object when I pick a cell(on the surface)
float vtkRayCast3DPicker::IntersectWithLine(float p1[3], float p2[3], float tol, 
                                       vtkAssemblyPath *path, 
                                       vtkProp3D *prop3D, 
                                       vtkAbstractMapper3D *m)
//------------------------------------------------------------------------------
{
  vtkIdType numCells, cellId, minCellId;
  int i, minSubId, subId;
  float tMin, t, pcoords[3], minXYZ[3], minPcoords[3];
  vtkDataSet *input;
  vtkMapper *mapper;
  vtkVolumeMapper *volumeMapper;

  // Get the underlying dataset
  if ( (mapper=vtkMapper::SafeDownCast(m)) != NULL )
  {
    input = mapper->GetInput();
  }
  else if ( (volumeMapper=vtkVolumeMapper::SafeDownCast(m)) != NULL )
  {
    input = volumeMapper->GetInput();
  }
  else
  {
    return VTK_LARGE_FLOAT;
  }

  if ( (numCells = input->GetNumberOfCells()) < 1 )
  {
    return 2.0;
  }

  //  Intersect each cell with ray.  Keep track of one closest to 
  //  the eye (and within the clipping range).
  //
  minCellId = -1;
  minSubId = -1;
  pcoords[0] = pcoords[1] = pcoords[2] = 0;
  for (tMin=VTK_LARGE_FLOAT,cellId=0; cellId<numCells; cellId++) 
  {
    input->GetCell(cellId, this->Cell);

    if ( this->Cell->IntersectWithLine(p1, p2, tol, t, this->x, pcoords, subId) //le coordinate del punto sul triangolo sono in x
    && t < tMin )
    {
      minCellId = cellId;
      minSubId = subId;
      for (i=0; i<3; i++)
      {
        minXYZ[i] = this->x[i];
        minPcoords[i] = pcoords[i];
      }
      tMin = t;
    }
  }
  //  Now compare this against other actors.
  //
  if ( minCellId>(-1) && tMin < this->GlobalTMin ) 
  {
    this->MarkPicked(path, prop3D, m, tMin, minXYZ);
    this->CellId = minCellId;
    this->SubId = minSubId;
    for (i=0; i<3; i++)
    {
      this->PCoords[i] = minPcoords[i];
    }
    vtkDebugMacro("Picked cell id= " << minCellId);
  }
  return tMin;
}

//------------------------------------------------------------------------------
// Initialize the picking process.
void vtkRayCast3DPicker::Initialize()
//------------------------------------------------------------------------------
{
  this->CellId = (-1);
  this->SubId = (-1);
  /*for (int i=0; i<3; i++)  //modified by Marco. 29-6-2004 Mhmmm???????
    {
    this->PCoords[i] = 0.0;
    }*/

  this->vtkAbstractPropPicker::Initialize();

  this->Actors->RemoveAllItems();
  this->Prop3Ds->RemoveAllItems();
  this->PickedPositions->Reset();
  
  this->MapperPosition[0] = 0.0;
  this->MapperPosition[1] = 0.0;
  this->MapperPosition[2] = 0.0;

  this->Mapper = NULL;
  this->GlobalTMin = VTK_LARGE_FLOAT;
}

//------------------------------------------------------------------------------
vtkActorCollection *vtkRayCast3DPicker::GetActors()
//------------------------------------------------------------------------------
{
  if (this->Actors->GetNumberOfItems() != 
        this->PickedPositions->GetNumberOfPoints()) 
  {
    vtkWarningMacro(<<"Not all Prop3Ds are actors, use GetProp3Ds instead");
  }
  return this->Actors; 
}

//------------------------------------------------------------------------------
void vtkRayCast3DPicker::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->DataSet )
  {
    os << indent << "DataSet: " << this->DataSet << "\n";
  }
  else
  {
    os << indent << "DataSet: (none)";
  }

  os << indent << "Mapper: " << this->Mapper << "\n";

  os << indent << "Tolerance: " << this->Tolerance << "\n";

  os << indent << "Mapper Position: (" <<  this->MapperPosition[0] << ","
     << this->MapperPosition[1] << ","
     << this->MapperPosition[2] << ")\n";
}
