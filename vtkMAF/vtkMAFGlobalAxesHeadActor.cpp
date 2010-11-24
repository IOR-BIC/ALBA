/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFGlobalAxesHeadActor.cpp,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFGlobalAxesHeadActor.h"

#include "vtkActor.h"
#include "vtkAppendPolyData.h"
#include "vtkAssembly.h"
#include "vtkCubeSource.h"
#include "vtkFeatureEdges.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPropCollection.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkVectorText.h"
#include "vcl_cassert.h"
#include "vtkPolyDataReader.h"
#include "vtkDirectory.h"

vtkCxxRevisionMacro(vtkMAFGlobalAxesHeadActor, "$Revision: 1.1.2.4 $");
vtkStandardNewMacro(vtkMAFGlobalAxesHeadActor);

#include "mafConfigure.h"
//-------------------------------------------------------------------------
vtkMAFGlobalAxesHeadActor::vtkMAFGlobalAxesHeadActor()
{

  HeadFileName = "UNDEFINED_HeadABSFileName";
  this->Assembly = vtkAssembly::New();

  this->HeadReader = vtkPolyDataReader::New();
  this->HeadFileName = "3dHead.vtk";

  std::string headABSFileName = GetHeadABSFileName();

  // DEVELOPER MODE:
  // 3dHead.vtk is available in MAF_DATA_ROOT\\VTK_Surfaces\\3dHead.vtk");

  // DEPLOY MODE:
  // The file ..\Config\HelperData\3dHead.vtk containing the 3d head polydata must exists in the current working directory (to be improved)
  // This is based on MAF vertical apps dir structure template which is as follows:
  // mafVerticalAppDir\bin\mafApp.exe (current working directory)
  // mafVerticalAppDir\Config\HelperData\3dHead.vtk
  // The 3d head should be contained in [-0.5 , 0.5, -0.5, 0.5, -0.5 , 0.5] (approx head dim should be 1)  
  assert(FileExists(headABSFileName.c_str()));

  HeadReader->SetFileName(headABSFileName.c_str());

  HeadReader->Update();

  int numCells = HeadReader->GetOutput()->GetNumberOfCells();
  assert(numCells > 0); // if this assert fail check for HeadABSFileName problems

  vtkPolyDataMapper *headMapper = vtkPolyDataMapper::New();
  this->HeadActor = vtkActor::New();
  headMapper->SetInput( this->HeadReader->GetOutput() );
  this->HeadActor->SetMapper( headMapper );
  this->HeadActor->SetVisibility(1);
  headMapper->Delete();

  this->Assembly->AddPart( this->HeadActor );

  vtkProperty* prop = this->HeadActor->GetProperty();
  prop->SetRepresentationToSurface();
  prop->SetColor(1, 1, 1);
 
  this->UpdateProps();
}

//-------------------------------------------------------------------------
vtkMAFGlobalAxesHeadActor::~vtkMAFGlobalAxesHeadActor()
{
  this->HeadReader->Delete();
  this->HeadActor->Delete();

  this->Assembly->Delete();
}


//-------------------------------------------------------------------------
void vtkMAFGlobalAxesHeadActor::SetHeadVisibility(int vis)
{
  this->HeadActor->SetVisibility(vis);
  this->Assembly->Modified();
}

//-------------------------------------------------------------------------
int vtkMAFGlobalAxesHeadActor::GetHeadVisibility()
{
  return this->HeadActor->GetVisibility();
}

//-------------------------------------------------------------------------
// Shallow copy of a vtkAnnotatedCubeActor.
void vtkMAFGlobalAxesHeadActor::ShallowCopy(vtkProp *prop)
{
  vtkMAFGlobalAxesHeadActor *a = vtkMAFGlobalAxesHeadActor::SafeDownCast(prop);

  // Now do superclass
  this->vtkProp3D::ShallowCopy(prop);
}

//-------------------------------------------------------------------------
void vtkMAFGlobalAxesHeadActor::GetActors(vtkPropCollection *ac)
{
  this->Assembly->GetActors( ac );
}

//-------------------------------------------------------------------------
int vtkMAFGlobalAxesHeadActor::RenderOpaqueGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  return this->Assembly->RenderOpaqueGeometry(vp);
}

//-----------------------------------------------------------------------------
int vtkMAFGlobalAxesHeadActor::RenderTranslucentPolygonalGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  //return this->Assembly->RenderTranslucentPolygonalGeometry( vp );

  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkMAFGlobalAxesHeadActor::HasTranslucentPolygonalGeometry()
{
  this->UpdateProps();

  //return this->Assembly->HasTranslucentPolygonalGeometry();
  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
void vtkMAFGlobalAxesHeadActor::ReleaseGraphicsResources(vtkWindow *win)
{
  this->Assembly->ReleaseGraphicsResources( win );
}

//-------------------------------------------------------------------------
void vtkMAFGlobalAxesHeadActor::GetBounds(double bounds[6])
{
  this->Assembly->GetBounds( bounds );
}

//-------------------------------------------------------------------------
// Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *vtkMAFGlobalAxesHeadActor::GetBounds()
{
  return this->Assembly->GetBounds( );
}

//-------------------------------------------------------------------------
unsigned long int vtkMAFGlobalAxesHeadActor::GetMTime()
{
  return this->Assembly->GetMTime();
}

//-------------------------------------------------------------------------
vtkProperty *vtkMAFGlobalAxesHeadActor::GetHeadProperty()
{
  return this->HeadActor->GetProperty();
}

//-------------------------------------------------------------------------
void vtkMAFGlobalAxesHeadActor::UpdateProps()
{
  // nothing to do
}

//-------------------------------------------------------------------------
void vtkMAFGlobalAxesHeadActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


std::string vtkMAFGlobalAxesHeadActor::GetHeadABSFileName()
{

	// search the 3dHead.vtk

	// search in deploy dir (deploy scenario)
	char buf[256];
	vtkDirectory::GetCurrentWorkingDirectory(buf, 256);
	std::string path = buf;
	path.append("\\..\\Config\\HelperData\\");
	path.append(HeadFileName);

	bool exists = FileExists(path.c_str());

	if (exists == true)
	{
		return path;
	}
	
	// else search in MAF testing dir (developer scenario)
	path.clear();
	path.append(MAF_DATA_ROOT);
	path.append("\\VTK_Surfaces\\3dHead.vtk");

	exists = FileExists(path.c_str());

	if (exists == true)
	{
		return path;
	}

	return "3dHead.vtk NOT FOUND!";
}

using namespace std;

bool vtkMAFGlobalAxesHeadActor::FileExists(const char* filename)
{
	FILE* fp = NULL;

	fp = fopen(filename, "r");

	if(fp != NULL)
	{
		fclose(fp);
		return true;
	}

	return false;
}