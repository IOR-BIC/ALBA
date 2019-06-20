/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAGlobalAxesHeadActor.cpp,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAGlobalAxesHeadActor.h"

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
#include "vtkTransformPolydataFilter.h"

vtkCxxRevisionMacro(vtkALBAGlobalAxesHeadActor, "$Revision: 1.1.2.5 $");
vtkStandardNewMacro(vtkALBAGlobalAxesHeadActor);

#include "albaConfigure.h"
//-------------------------------------------------------------------------
vtkALBAGlobalAxesHeadActor::vtkALBAGlobalAxesHeadActor()
{

  HeadFileName = "UNDEFINED_HeadABSFileName";
  this->Assembly = vtkAssembly::New();

  this->HeadReader = vtkPolyDataReader::New();
  this->HeadFileName = "3dHead.vtk";

  std::string headABSFileName = GetHeadABSFileName();

  // DEVELOPER MODE:
  // 3dHead.vtk is available in ALBA_DATA_ROOT\\VTK_Surfaces\\3dHead.vtk");

  // DEPLOY MODE:
  // The file ..\Config\HelperData\3dHead.vtk containing the 3d head polydata must exists in the current working directory (to be improved)
  // This is based on ALBA vertical apps dir structure template which is as follows:
  // albaVerticalAppDir\bin\albaApp.exe (current working directory)
  // albaVerticalAppDir\Config\HelperData\3dHead.vtk
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
vtkALBAGlobalAxesHeadActor::~vtkALBAGlobalAxesHeadActor()
{
  this->HeadReader->Delete();
  this->HeadActor->Delete();

  this->Assembly->Delete();
}


//-------------------------------------------------------------------------
void vtkALBAGlobalAxesHeadActor::SetHeadVisibility(int vis)
{
  this->HeadActor->SetVisibility(vis);
  this->Assembly->Modified();
}

//-------------------------------------------------------------------------
int vtkALBAGlobalAxesHeadActor::GetHeadVisibility()
{
  return this->HeadActor->GetVisibility();
}

//-------------------------------------------------------------------------
// Shallow copy of a vtkAnnotatedCubeActor.
void vtkALBAGlobalAxesHeadActor::ShallowCopy(vtkProp *prop)
{
  vtkALBAGlobalAxesHeadActor *a = vtkALBAGlobalAxesHeadActor::SafeDownCast(prop);

  // Now do superclass
  this->vtkProp3D::ShallowCopy(prop);
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesHeadActor::GetActors(vtkPropCollection *ac)
{
  this->Assembly->GetActors( ac );
}

//-------------------------------------------------------------------------
int vtkALBAGlobalAxesHeadActor::RenderOpaqueGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  return this->Assembly->RenderOpaqueGeometry(vp);
}

//-----------------------------------------------------------------------------
int vtkALBAGlobalAxesHeadActor::RenderTranslucentPolygonalGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  //return this->Assembly->RenderTranslucentPolygonalGeometry( vp );

  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkALBAGlobalAxesHeadActor::HasTranslucentPolygonalGeometry()
{
  this->UpdateProps();

  //return this->Assembly->HasTranslucentPolygonalGeometry();
  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
void vtkALBAGlobalAxesHeadActor::ReleaseGraphicsResources(vtkWindow *win)
{
  this->Assembly->ReleaseGraphicsResources( win );
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesHeadActor::GetBounds(double bounds[6])
{
  this->Assembly->GetBounds( bounds );
}

//-------------------------------------------------------------------------
// Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *vtkALBAGlobalAxesHeadActor::GetBounds()
{
  return this->Assembly->GetBounds( );
}

//-------------------------------------------------------------------------
unsigned long int vtkALBAGlobalAxesHeadActor::GetMTime()
{
  return this->Assembly->GetMTime();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAGlobalAxesHeadActor::GetHeadProperty()
{
  return this->HeadActor->GetProperty();
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesHeadActor::UpdateProps()
{
  // nothing to do
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesHeadActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


std::string vtkALBAGlobalAxesHeadActor::GetHeadABSFileName()
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
	
	// else search in ALBA testing dir (developer scenario)
	path.clear();
	path.append(ALBA_DATA_ROOT);
	path.append("\\VTK_Surfaces\\3dHead.vtk");

	exists = FileExists(path.c_str());

	if (exists == true)
	{
		return path;
	}

	return "3dHead.vtk NOT FOUND!";
}

using namespace std;

bool vtkALBAGlobalAxesHeadActor::FileExists(const char* filename)
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

void vtkALBAGlobalAxesHeadActor::SetInitialPose(vtkMatrix4x4* initMatrix)
{
  // Directly transform polydata
  vtkPolyData* data = ((vtkPolyData*)((vtkPolyDataMapper*)this->HeadActor->GetMapper())->GetInput());

  vtkTransform* initTransform;
  vtkTransformPolyDataFilter* transformer;

  initTransform = vtkTransform::New();
  transformer = vtkTransformPolyDataFilter::New();

  initTransform->SetMatrix(initMatrix);
  transformer->SetInput(data);
  transformer->SetTransform(initTransform);
  transformer->Update();

  data->DeepCopy(transformer->GetOutput());
  data->Update();
  data->Modified();
  ((vtkPolyDataMapper*)this->HeadActor->GetMapper())->Update();
  ((vtkPolyDataMapper*)this->HeadActor->GetMapper())->Modified();
  this->HeadActor->Modified();

  transformer->Delete();
  initTransform->Delete();
}