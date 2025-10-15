/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAAssembly
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "vtkALBAAssembly.h"
#include "vtkRenderWindow.h"
#include "vtkObjectFactory.h"
#include "vtkAssemblyPaths.h"
#include "vtkAssemblyNode.h"
#include "vtkProp3DCollection.h"
#include "vtkActor.h"
#include "vtkVolume.h"

vtkStandardNewMacro(vtkALBAAssembly);

// Construct object with no children.
//----------------------------------------------------------------------------
vtkALBAAssembly::vtkALBAAssembly()
//----------------------------------------------------------------------------
{
  this->m_Parts = vtkProp3DCollection::New();
}

//----------------------------------------------------------------------------
vtkALBAAssembly::~vtkALBAAssembly()
//----------------------------------------------------------------------------
{
  this->m_Parts->Delete();
  this->m_Parts = NULL;
}

// Add a part to the list of Parts.
//----------------------------------------------------------------------------
void vtkALBAAssembly::AddPart(vtkProp3D *prop)
//----------------------------------------------------------------------------
{
  if ( ! this->m_Parts->IsItemPresent(prop) )
  {
    this->m_Parts->AddItem(prop);
    this->Modified();
  } 
}

// Remove a part from the list of parts,
//----------------------------------------------------------------------------
void vtkALBAAssembly::RemovePart(vtkProp3D *prop)
//----------------------------------------------------------------------------
{
  if ( this->m_Parts->IsItemPresent(prop) )
  {
    this->m_Parts->RemoveItem(prop);
    this->Modified();
  } 
}

// Shallow copy another assembly.
//----------------------------------------------------------------------------
void vtkALBAAssembly::ShallowCopy(vtkProp *prop)
//----------------------------------------------------------------------------
{
  vtkALBAAssembly *a = vtkALBAAssembly::SafeDownCast(prop);
  if ( a != NULL )
  {
    this->m_Parts->RemoveAllItems();
    a->m_Parts->InitTraversal();
    for (int i=0; i<a->m_Parts->GetNumberOfItems(); i++) // change by Losi on 06/15/2010 in order to iterate over props (before the conditions was i<0)
    {
      this->m_Parts->AddItem(a->m_Parts->GetNextProp3D());
    }
  }

  // Now do superclass
  this->vtkProp3D::ShallowCopy(prop);
}

//----------------------------------------------------------------------------
void vtkALBAAssembly::ReleaseGraphicsResources(vtkWindow *renWin)
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;
  for ( this->m_Parts->InitTraversal(); prop3D = this->m_Parts->GetNextProp3D(); )
  {
    prop3D->ReleaseGraphicsResources(renWin);
  }
}

//----------------------------------------------------------------------------
void vtkALBAAssembly::GetActors(vtkPropCollection *ac)
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;
  vtkActor *actor;
  vtkAssemblyPath *path;

  this->UpdatePaths();
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
  {
    prop3D = (vtkProp3D *)path->GetLastNode()->GetViewProp();
    if ( (actor = vtkActor::SafeDownCast(prop3D)) != NULL )
    {
      ac->AddItem(actor);
    }
  }
}

//----------------------------------------------------------------------------
void vtkALBAAssembly::GetVolumes(vtkPropCollection *ac)
//----------------------------------------------------------------------------
{
  vtkProp3D *prop3D;
  vtkVolume *volume;
  vtkAssemblyPath *path;

  this->UpdatePaths();
  for ( this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
  {
    prop3D = (vtkProp3D *)path->GetLastNode()->GetViewProp();
    if ( (volume = vtkVolume::SafeDownCast(prop3D)) != NULL )
    {
      ac->AddItem(volume);
    }
  }
}

//----------------------------------------------------------------------------
void vtkALBAAssembly::InitPathTraversal()
//----------------------------------------------------------------------------
{
  this->UpdatePaths();
  this->Paths->InitTraversal();
}

// Return the next part in the hierarchy of assembly Parts.  This method 
// returns a properly transformed and updated actor.
//----------------------------------------------------------------------------
vtkAssemblyPath *vtkALBAAssembly::GetNextPath()
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
int vtkALBAAssembly::GetNumberOfPaths()
//----------------------------------------------------------------------------
{
  this->UpdatePaths();
  return this->Paths->GetNumberOfItems();
}


// Get the bounds for the assembly as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
//----------------------------------------------------------------------------
double *vtkALBAAssembly::GetBounds()
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
    prop3D = (vtkProp3D *)path->GetLastNode()->GetViewProp();
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
vtkMTimeType vtkALBAAssembly::GetMTime()
//----------------------------------------------------------------------------
{
	vtkMTimeType mTime=this->vtkProp3D::GetMTime();
	vtkMTimeType time;
  vtkProp3D *prop;

  for (this->m_Parts->InitTraversal(); (prop = this->m_Parts->GetNextProp3D()); )
  {
		if (prop->GetVisibility())
		{
			time = prop->GetMTime();
			mTime = (time > mTime ? time : mTime);
		}
  }

  return mTime;
}

//----------------------------------------------------------------------------
void vtkALBAAssembly::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "There are: " << this->m_Parts->GetNumberOfItems()
     << " parts in this assembly\n";
}

//-----------------------------------------------------------------------------
int vtkALBAAssembly::RenderTranslucentPolygonalGeometry(vtkViewport *ren)
{
	int renderedSomething = 0;

	if (GetVisibility())
	{
		vtkAssemblyPath *path;

		if (!Paths)
			UpdatePaths();

		// for allocating render time between components
		const float fraction = this->AllocatedRenderTime
			/ (float)(this->Paths->GetNumberOfItems() > 0 ? this->Paths->GetNumberOfItems() : 1);

		// render the Paths
		for (this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
		{
    vtkProp3D *prop3D = (vtkProp3D *)path->GetLastNode()->GetViewProp();
    if ( prop3D->GetVisibility()) 
			{
        prop3D->SetPropertyKeys(this->GetPropertyKeys());
				prop3D->SetAllocatedRenderTime(fraction, ren);
				prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
	      renderedSomething += prop3D->RenderTranslucentPolygonalGeometry(ren);
				prop3D->PokeMatrix(NULL);
			}
		}
	}

  return renderedSomething > 0;
}

//-----------------------------------------------------------------------------
// this method was reimplemented from vtkAssembly to correct div-by-zero bug
//-----------------------------------------------------------------------------
int vtkALBAAssembly::RenderOpaqueGeometry(vtkViewport *ren)
//----------------------------------------------------------------------------
{
	int renderedSomething = 0;

	if (GetVisibility())
	{
		vtkAssemblyPath *path;

		if (!Paths)
			UpdatePaths();

		// for allocating render time between components
		const float fraction = this->AllocatedRenderTime
			/ (float)(this->Paths->GetNumberOfItems() > 0 ? this->Paths->GetNumberOfItems() : 1);

		// render the Paths
		for (this->Paths->InitTraversal(); (path = this->Paths->GetNextItem()); )
		{
	    vtkProp3D *prop3D = (vtkProp3D *)path->GetLastNode()->GetViewProp();
    	if (prop3D->GetVisibility()) 
			{
				prop3D->SetAllocatedRenderTime(fraction, ren);
				prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
				renderedSomething += prop3D->RenderOpaqueGeometry(ren);
				prop3D->PokeMatrix(NULL);
			}
		}
	}

  return renderedSomething > 0;
}


//-----------------------------------------------------------------------------
// Build the assembly paths if necessary. UpdatePaths()
// is only called when the assembly is at the root
// of the hierarchy; otherwise UpdatePaths() is called.
//-----------------------------------------------------------------------------
void vtkALBAAssembly::UpdatePaths()
//----------------------------------------------------------------------------
{
  if (this->GetMTime() > this->PathTime || (this->Paths != NULL && this->Paths->GetMTime() > this->PathTime) )
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
      for ( this->m_Parts->InitTraversal(); prop3D = this->m_Parts->GetNextProp3D(); )
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
void vtkALBAAssembly::BuildPaths(vtkAssemblyPaths *paths, vtkAssemblyPath *path)
//----------------------------------------------------------------------------
{
	if (GetVisibility())
	{
		vtkProp3D *prop3D;
		for (this->m_Parts->InitTraversal(); prop3D = this->m_Parts->GetNextProp3D(); )
		{
			if (prop3D->GetVisibility())
			{
				path->AddNode(prop3D, prop3D->GetMatrix());

				// dive into the hierarchy
				prop3D->BuildPaths(paths, path);

				// when returned, pop the last node off of the
				// current path
				path->DeleteLastNode();
			}
		}
	}
}

//----------------------------------------------------------------------------
int vtkALBAAssembly::HasTranslucentPolygonalGeometry()
//----------------------------------------------------------------------------
{
	vtkProp3D *prop3D;
	for ( this->m_Parts->InitTraversal();  (prop3D = this->m_Parts->GetNextProp3D()); )
	{
		if (prop3D->HasTranslucentPolygonalGeometry())
			return true;
	}
	return false;
}
