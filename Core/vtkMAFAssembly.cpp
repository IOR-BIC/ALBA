/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkMAFAssembly.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-31 15:58:06 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "vtkMAFAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkObjectFactory.h"
#include "vtkAssemblyPaths.h"
#include "vtkAssemblyNode.h"
#include "vtkProp3DCollection.h"
#include "vtkActor.h"
#include "vtkVolume.h"

vtkCxxRevisionMacro(vtkMAFAssembly, "$Revision: 1.5 $");
vtkStandardNewMacro(vtkMAFAssembly);

// Construct object with no children.
//----------------------------------------------------------------------------
vtkMAFAssembly::vtkMAFAssembly()
//----------------------------------------------------------------------------
{
  this->Parts = vtkProp3DCollection::New();
}

//----------------------------------------------------------------------------
vtkMAFAssembly::~vtkMAFAssembly()
//----------------------------------------------------------------------------
{
  this->Parts->Delete();
  this->Parts = NULL;
}

// Add a part to the list of Parts.
//----------------------------------------------------------------------------
void vtkMAFAssembly::AddPart(vtkProp3D *prop)
//----------------------------------------------------------------------------
{
  if ( ! this->Parts->IsItemPresent(prop) )
  {
    this->Parts->AddItem(prop);
    this->Modified();
  } 
}

// Remove a part from the list of parts,
//----------------------------------------------------------------------------
void vtkMAFAssembly::RemovePart(vtkProp3D *prop)
//----------------------------------------------------------------------------
{
  if ( this->Parts->IsItemPresent(prop) )
  {
    this->Parts->RemoveItem(prop);
    this->Modified();
  } 
}

// Shallow copy another assembly.
//----------------------------------------------------------------------------
void vtkMAFAssembly::ShallowCopy(vtkProp *prop)
//----------------------------------------------------------------------------
{
  vtkMAFAssembly *a = vtkMAFAssembly::SafeDownCast(prop);
  if ( a != NULL )
  {
    this->Parts->RemoveAllItems();
    a->Parts->InitTraversal();
    for (int i=0; i<0; i++)
    {
      this->Parts->AddItem(a->Parts->GetNextProp3D());
    }
  }

  // Now do superclass
  this->vtkProp3D::ShallowCopy(prop);
}
/*
// Render this assembly and all its Parts. The rendering process is recursive.
// Note that a mapper need not be defined. If not defined, then no geometry 
// will be drawn for this assembly. This allows you to create "logical"
// assemblies; that is, assemblies that only serve to group and transform
// its Parts.
int vtkMAFAssembly::RenderTranslucentGeometry(vtkViewport *ren)
{
  vtkProp3D *prop3D;
  vtkAssemblyPath *path;
  float fraction;
  int renderedSomething = 0;

  this->UpdatePaths();

  // for allocating render time between components
  // simple equal allocation
  fraction = this->AllocatedRenderTime 
    / (float)(this->Paths->GetNumberOfItems());
  
  // render the Paths
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
    {
    prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
    if ( prop3D->GetVisibility() )
      {
      prop3D->SetAllocatedRenderTime(fraction, ren);
      prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
      renderedSomething += prop3D->RenderTranslucentGeometry(ren);
      prop3D->PokeMatrix(NULL);
      }
    }

  renderedSomething = (renderedSomething > 0)?(1):(0);

  return renderedSomething;
}
*/


/*
// Render this assembly and all its Parts. The rendering process is recursive.
// Note that a mapper need not be defined. If not defined, then no geometry 
// will be drawn for this assembly. This allows you to create "logical"
// assemblies; that is, assemblies that only serve to group and transform
// its Parts.
int vtkMAFAssembly::RenderOpaqueGeometry(vtkViewport *ren)
{
  vtkProp3D *prop3D;
  vtkAssemblyPath *path;
  float fraction;
  int   renderedSomething = 0;

  this->UpdatePaths();

  // for allocating render time between components
  // simple equal allocation
  fraction = this->AllocatedRenderTime 
    / (float)(this->Paths->GetNumberOfItems());
  
  // render the Paths
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
    {
    prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
    if ( prop3D->GetVisibility() )
      {
      prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
      prop3D->SetAllocatedRenderTime(fraction, ren);
      renderedSomething += prop3D->RenderOpaqueGeometry(ren);
      prop3D->PokeMatrix(NULL);
      }
    }

  renderedSomething = (renderedSomething > 0)?(1):(0);

  return renderedSomething;
}
*/

//----------------------------------------------------------------------------
void vtkMAFAssembly::ReleaseGraphicsResources(vtkWindow *renWin)
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;
  for ( this->Parts->InitTraversal(); prop3D = this->Parts->GetNextProp3D(); )
  {
    prop3D->ReleaseGraphicsResources(renWin);
  }
}

//----------------------------------------------------------------------------
void vtkMAFAssembly::GetActors(vtkPropCollection *ac)
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;
  vtkActor *actor;
  vtkAssemblyPath *path;

  this->UpdatePaths();
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
  {
    prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
    if ( (actor = vtkActor::SafeDownCast(prop3D)) != NULL )
    {
      ac->AddItem(actor);
    }
  }
}

//----------------------------------------------------------------------------
void vtkMAFAssembly::GetVolumes(vtkPropCollection *ac)
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;
  vtkVolume *volume;
  vtkAssemblyPath *path;

  this->UpdatePaths();
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
  {
    prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
    if ( (volume = vtkVolume::SafeDownCast(prop3D)) != NULL )
    {
      ac->AddItem(volume);
    }
  }
}

//----------------------------------------------------------------------------
void vtkMAFAssembly::InitPathTraversal()
//----------------------------------------------------------------------------
{
  this->UpdatePaths();
  this->Paths->InitTraversal();
}

// Return the next part in the hierarchy of assembly Parts.  This method 
// returns a properly transformed and updated actor.
//----------------------------------------------------------------------------
vtkAssemblyPath *vtkMAFAssembly::GetNextPath()
//----------------------------------------------------------------------------
{
  if ( this->Paths )
  {
    return this->Paths->GetNextItem();
  }
  else
  {
    return NULL;
  }
}

//----------------------------------------------------------------------------
int vtkMAFAssembly::GetNumberOfPaths()
//----------------------------------------------------------------------------
{
  this->UpdatePaths();
  return this->Paths->GetNumberOfItems();
}

/*
// Build the assembly paths if necessary. UpdatePaths()
// is only called when the assembly is at the root
// of the hierarchy; otherwise UpdatePaths() is called.
void vtkMAFAssembly::UpdatePaths()
{
  if ( this->GetMTime() > this->PathTime ||
    (this->Paths != NULL && this->Paths->GetMTime() > this->PathTime) )
    {
    if ( this->Paths != NULL )
      {
      this->Paths->Delete();
      this->Paths = NULL;
      }

    // Create the list to hold all the paths
    this->Paths = vtkAssemblyPaths::New();
    vtkAssemblyPath *path = vtkAssemblyPath::New();

    //add ourselves to the path to start things off
    path->AddNode(this,this->GetMatrix());
    
    // Add nodes as we proceed down the hierarchy
    vtkProp3D *prop3D;
    for ( this->Parts->InitTraversal(); 
          (prop3D = this->Parts->GetNextProp3D()); )
      {
      path->AddNode(prop3D,prop3D->GetMatrix());

      // dive into the hierarchy
      prop3D->BuildPaths(this->Paths,path);
      
      // when returned, pop the last node off of the
      // current path
      path->DeleteLastNode();
      }

    path->Delete();
    this->PathTime.Modified();
    }
}
*/

/*
// Build assembly paths from this current assembly. A path consists of
// an ordered sequence of props, with transformations properly concatenated.
void vtkMAFAssembly::BuildPaths(vtkAssemblyPaths *paths, vtkAssemblyPath *path)
{
  vtkProp3D *prop3D;

  for ( this->Parts->InitTraversal(); 
        (prop3D = this->Parts->GetNextProp3D()); )
    {
    path->AddNode(prop3D,prop3D->GetMatrix());

    // dive into the hierarchy
    prop3D->BuildPaths(paths,path);

    // when returned, pop the last node off of the
    // current path
    path->DeleteLastNode();
    }
}
*/

// Get the bounds for the assembly as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
//----------------------------------------------------------------------------
double *vtkMAFAssembly::GetBounds()
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;
  vtkAssemblyPath *path;
  int i, n;
  double *bounds, bbox[24];
  int propVisible=0;

  this->UpdatePaths();

  // now calculate the new bounds
  this->Bounds[0] = this->Bounds[2] = this->Bounds[4] = VTK_DOUBLE_MAX;
  this->Bounds[1] = this->Bounds[3] = this->Bounds[5] = VTK_DOUBLE_MIN;

  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
  {
    prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
    if ( prop3D->GetVisibility() )
    {
      propVisible = 1;
      prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
      bounds = prop3D->GetBounds();
      prop3D->PokeMatrix(NULL);

      // if bounds are invalid skip merging
      if (bounds[0] == VTK_DOUBLE_MAX && bounds[1]==VTK_DOUBLE_MIN && \
        bounds[2]==VTK_DOUBLE_MAX && bounds[3]==VTK_DOUBLE_MIN &&
        bounds[4]==VTK_DOUBLE_MAX && bounds[5]==VTK_DOUBLE_MIN)
      {
        continue;
      }

      // fill out vertices of a bounding box
      bbox[ 0] = bounds[1]; bbox[ 1] = bounds[3]; bbox[ 2] = bounds[5];
      bbox[ 3] = bounds[1]; bbox[ 4] = bounds[2]; bbox[ 5] = bounds[5];
      bbox[ 6] = bounds[0]; bbox[ 7] = bounds[2]; bbox[ 8] = bounds[5];
      bbox[ 9] = bounds[0]; bbox[10] = bounds[3]; bbox[11] = bounds[5];
      bbox[12] = bounds[1]; bbox[13] = bounds[3]; bbox[14] = bounds[4];
      bbox[15] = bounds[1]; bbox[16] = bounds[2]; bbox[17] = bounds[4];
      bbox[18] = bounds[0]; bbox[19] = bounds[2]; bbox[20] = bounds[4];
      bbox[21] = bounds[0]; bbox[22] = bounds[3]; bbox[23] = bounds[4];

      for (i = 0; i < 8; i++)
      {
        for (n = 0; n < 3; n++)
        {
          if (bbox[i*3+n] < this->Bounds[n*2])
          {
            this->Bounds[n*2] = bbox[i*3+n];
          }
          if (bbox[i*3+n] > this->Bounds[n*2+1])
          {
            this->Bounds[n*2+1] = bbox[i*3+n];
          }
        }//for each coordinate axis
      }//for each point of box
    }//if visible && prop3d
  }//for each path

  if ( ! propVisible )
  {
    this->Bounds[0] = this->Bounds[2] = this->Bounds[4] = -1.0;
    this->Bounds[1] = this->Bounds[3] = this->Bounds[5] =  1.0;
  }

  return this->Bounds;
}

//----------------------------------------------------------------------------
unsigned long int vtkMAFAssembly::GetMTime()
//----------------------------------------------------------------------------
{
  unsigned long mTime=this->vtkProp3D::GetMTime();
  unsigned long time;
  vtkProp3D *prop;

  for (this->Parts->InitTraversal(); (prop = this->Parts->GetNextProp3D()); )
  {
    time = prop->GetMTime();
    mTime = ( time > mTime ? time : mTime );
  }

  return mTime;
}

//----------------------------------------------------------------------------
void vtkMAFAssembly::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "There are: " << this->Parts->GetNumberOfItems()
     << " parts in this assembly\n";
}



//-----------------------------------------------------------------------------
// the following code was written by Sasha in vtkMAFAssembly
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// this method was reimplemented from vtkAssembly to correct div-by-zero bug
//-----------------------------------------------------------------------------
int vtkMAFAssembly::RenderTranslucentGeometry(vtkViewport *ren) 
//----------------------------------------------------------------------------
{
  vtkAssemblyPath *path;
  int renderedSomething = 0;
  
  this->UpdatePaths();
  
  // for allocating render time between components
  const float fraction = this->AllocatedRenderTime 
    / (float)(this->Paths->GetNumberOfItems() > 0 ? this->Paths->GetNumberOfItems() : 1);
  
  // render the Paths
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); ) 
  {
    vtkProp3D *prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
    if ( prop3D->GetVisibility()) 
    {
      prop3D->SetAllocatedRenderTime(fraction, ren);
      prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
      renderedSomething += prop3D->RenderTranslucentGeometry(ren);
      prop3D->PokeMatrix(NULL);
    }
  }
  
  return renderedSomething > 0;
}

//-----------------------------------------------------------------------------
// this method was reimplemented from vtkAssembly to correct div-by-zero bug
//-----------------------------------------------------------------------------
int vtkMAFAssembly::RenderOpaqueGeometry(vtkViewport *ren) 
//----------------------------------------------------------------------------
{
  vtkAssemblyPath *path;
  int renderedSomething = 0;
  
  this->UpdatePaths();
  
  // for allocating render time between components
  const float fraction = this->AllocatedRenderTime 
    / (float)(this->Paths->GetNumberOfItems() > 0 ? this->Paths->GetNumberOfItems() : 1);
  
  // render the Paths
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); ) 
  {
    vtkProp3D *prop3D = (vtkProp3D *)path->GetLastNode()->GetProp();
    if ( prop3D->GetVisibility()) 
    {
      prop3D->SetAllocatedRenderTime(fraction, ren);
      prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
      renderedSomething += prop3D->RenderOpaqueGeometry(ren);
      prop3D->PokeMatrix(NULL);
    }
  }
  
  return renderedSomething > 0;
}


//-----------------------------------------------------------------------------
// Build the assembly paths if necessary. UpdatePaths()
// is only called when the assembly is at the root
// of the hierarchy; otherwise UpdatePaths() is called.
//-----------------------------------------------------------------------------
void vtkMAFAssembly::UpdatePaths()
//----------------------------------------------------------------------------
{
  if ( this->GetMTime() > this->PathTime ||
    (this->Paths != NULL && this->Paths->GetMTime() > this->PathTime) )
  {
    if ( this->Paths != NULL )
    {
      this->Paths->Delete();
      this->Paths = NULL;
    }

    // Create the list to hold all the paths
    this->Paths = vtkAssemblyPaths::New();
    vtkAssemblyPath *path = vtkAssemblyPath::New();

    //add ourselves to the path to start things off
    path->AddNode(this,this->GetMatrix());
    
    // Add nodes as we proceed down the hierarchy
    vtkProp3D *prop3D;
    if (this->GetVisibility())
    {
      for ( this->Parts->InitTraversal(); 
            (prop3D = this->Parts->GetNextProp3D()); )
      {
        if (prop3D->GetVisibility())
        {
          path->AddNode(prop3D,prop3D->GetMatrix());

          // dive into the hierarchy
          prop3D->BuildPaths(this->Paths,path);
  
          // when returned, pop the last node off of the
          // current path
          path->DeleteLastNode();
        }
      }
    }

    path->Delete();
    this->PathTime.Modified();
  }
}


// Build assembly paths from this current assembly. A path consists of
// an ordered sequence of props, with transformations properly concatenated.
//----------------------------------------------------------------------------
void vtkMAFAssembly::BuildPaths(vtkAssemblyPaths *paths, vtkAssemblyPath *path)
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;

  for ( this->Parts->InitTraversal(); 
        (prop3D = this->Parts->GetNextProp3D()); )
  {
    if (prop3D->GetVisibility())
    {
      path->AddNode(prop3D,prop3D->GetMatrix());

      // dive into the hierarchy
      prop3D->BuildPaths(paths,path);

      // when returned, pop the last node off of the
      // current path
      path->DeleteLastNode();
    }
  }
}
