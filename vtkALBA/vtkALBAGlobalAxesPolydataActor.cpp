/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAGlobalAxesPolydataActor.cpp,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAGlobalAxesPolydataActor.h"

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

vtkCxxRevisionMacro(vtkALBAGlobalAxesPolydataActor, "$Revision: 1.1.2.5 $");
vtkStandardNewMacro(vtkALBAGlobalAxesPolydataActor);

#include "albaConfigure.h"
#include "wx\filefn.h"
//-------------------------------------------------------------------------
vtkALBAGlobalAxesPolydataActor::vtkALBAGlobalAxesPolydataActor(int type /*=HEAD*/)
{
  FileName = "UNDEFINED_ABSFileName";
  this->Assembly = vtkAssembly::New();

  this->Reader = vtkPolyDataReader::New();

	//SetType(type);
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::SetType(int type)
{
	ActorType = type;

	if (ActorType == HEAD)
		SetFileName("3dHead.vtk");

	if (ActorType == BODY)
		SetFileName("3dBody.vtk");

	std::string ABSFileName = GetABSFileName();

	// DEVELOPER MODE:
	// 3dHead.vtk is available in ALBA_DATA_ROOT\\VTK_Surfaces\\3dHead.vtk");

	// DEPLOY MODE:
	// The file ..\Config\HelperData\3dHead.vtk containing the 3d head polydata must exists in the current working directory (to be improved)
	// This is based on ALBA vertical apps dir structure template which is as follows:
	// albaVerticalAppDir\bin\albaApp.exe (current working directory)
	// albaVerticalAppDir\Config\HelperData\3dHead.vtk
	// The 3d head should be contained in [-0.5 , 0.5, -0.5, 0.5, -0.5 , 0.5] (approx head dim should be 1)

	assert(FileExists(ABSFileName.c_str()));

	Reader->SetFileName(ABSFileName.c_str());

	Reader->Update();

	int numCells = Reader->GetOutput()->GetNumberOfCells();
	assert(numCells > 0); // if this assert fail check for ABSFileName problems

	vtkPolyDataMapper *headMapper = vtkPolyDataMapper::New();
	this->Actor = vtkActor::New();
	headMapper->SetInput(this->Reader->GetOutput());
	this->Actor->SetMapper(headMapper);
	this->Actor->SetVisibility(1);
	headMapper->Delete();

	this->Assembly->AddPart(this->Actor);

	vtkProperty* prop = this->Actor->GetProperty();
	prop->SetRepresentationToSurface();
	prop->SetColor(1, 1, 1);

	this->UpdateProps();
}

//-------------------------------------------------------------------------
vtkALBAGlobalAxesPolydataActor::~vtkALBAGlobalAxesPolydataActor()
{
  this->Reader->Delete();
  this->Actor->Delete();

  this->Assembly->Delete();
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::SetVisibility(int vis)
{
  this->Actor->SetVisibility(vis);
  this->Assembly->Modified();
}

//-------------------------------------------------------------------------
int vtkALBAGlobalAxesPolydataActor::GetVisibility()
{
  return this->Actor->GetVisibility();
}

//-------------------------------------------------------------------------
// Shallow copy of a vtkAnnotatedCubeActor.
void vtkALBAGlobalAxesPolydataActor::ShallowCopy(vtkProp *prop)
{
  vtkALBAGlobalAxesPolydataActor *a = vtkALBAGlobalAxesPolydataActor::SafeDownCast(prop);

  // Now do superclass
  this->vtkProp3D::ShallowCopy(prop);
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::GetActors(vtkPropCollection *ac)
{
  this->Assembly->GetActors( ac );
}

//-------------------------------------------------------------------------
int vtkALBAGlobalAxesPolydataActor::RenderOpaqueGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  return this->Assembly->RenderOpaqueGeometry(vp);
}

//-----------------------------------------------------------------------------
int vtkALBAGlobalAxesPolydataActor::RenderTranslucentPolygonalGeometry(vtkViewport *vp)
{
  this->UpdateProps();

  //return this->Assembly->RenderTranslucentPolygonalGeometry( vp );

  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkALBAGlobalAxesPolydataActor::HasTranslucentPolygonalGeometry()
{
  this->UpdateProps();

  //return this->Assembly->HasTranslucentPolygonalGeometry();
  assert(false);
  return false;
}

//-----------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::ReleaseGraphicsResources(vtkWindow *win)
{
  this->Assembly->ReleaseGraphicsResources( win );
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::GetBounds(double bounds[6])
{
  this->Assembly->GetBounds( bounds );
}

//-------------------------------------------------------------------------
// Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *vtkALBAGlobalAxesPolydataActor::GetBounds()
{
  return this->Assembly->GetBounds( );
}

//-------------------------------------------------------------------------
unsigned long int vtkALBAGlobalAxesPolydataActor::GetMTime()
{
  return this->Assembly->GetMTime();
}

//-------------------------------------------------------------------------
vtkProperty *vtkALBAGlobalAxesPolydataActor::GetProperty()
{
  return this->Actor->GetProperty();
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::UpdateProps()
{
  // nothing to do
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-------------------------------------------------------------------------
std::string vtkALBAGlobalAxesPolydataActor::GetABSFileName()
{
	// search the file .vtk
	// search in deploy dir (deploy scenario)
	char buf[256];
	vtkDirectory::GetCurrentWorkingDirectory(buf, 256);
	std::string path = buf;
	path.append("\\..\\Config\\HelperData\\");
	path.append(FileName);

	bool exists = FileExists(path.c_str());

	if (exists == true)
	{
		return path;
	}
	
	// else search in ALBA testing dir (developer scenario)
	path.clear();
	path.append(ALBA_DATA_ROOT);
	path.append("\\VTK_Surfaces\\");
	path.append(FileName);

	exists = FileExists(path.c_str());

	if (exists == true)
	{
		return path;
	}

	return FileName + " NOT FOUND!";
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::SetFileName(std::string filename)
{
	this->FileName = filename;
}

using namespace std;

//-------------------------------------------------------------------------
bool vtkALBAGlobalAxesPolydataActor::FileExists(const char* filename)
{
	return wxFileExists(filename);
}

//-------------------------------------------------------------------------
void vtkALBAGlobalAxesPolydataActor::SetInitialPose(vtkMatrix4x4* initMatrix)
{
  // Directly transform polydata
  vtkPolyData* data = ((vtkPolyData*)((vtkPolyDataMapper*)this->Actor->GetMapper())->GetInput());

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
  ((vtkPolyDataMapper*)this->Actor->GetMapper())->Update();
  ((vtkPolyDataMapper*)this->Actor->GetMapper())->Modified();
  this->Actor->Modified();

  transformer->Delete();
  initTransform->Delete();
}