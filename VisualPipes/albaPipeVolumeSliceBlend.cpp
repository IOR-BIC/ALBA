/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeSliceBlend
 Authors: Paolo Quadrani, Gianluigi Crimi
 
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

#include "albaPipeVolumeSliceBlend.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSceneNode.h"
#include "mmaVolumeMaterial.h"
#include "albaPipeVolumeOrthoSlice.h"
#include "albaTransformBase.h"
#include "albaVMEVolume.h"
#include "albaVMEOutputVolume.h"
#include "albaGUIFloatSlider.h"
#include "albaLODActor.h"
#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkALBAVolumeOrthoSlicer.h"
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkTransform.h"
#include "vtkRenderer.h"
#include "vtkOutlineSource.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkPlaneSource.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVolumeSliceBlend);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVolumeSliceBlend::albaPipeVolumeSliceBlend()
:albaPipe()
//----------------------------------------------------------------------------
{
  //Initialize values
  for(int j=0;j<2;j++)
    for(int i = 0; i<3; i++)
    {
      m_Slicer[j][i]				= NULL;
      m_Image[j][i]							= NULL;
      m_Texture[j][i]						= NULL;
      m_SliceMapper[j][i]				= NULL;
      m_SliceActor[j][i]				= NULL;
			m_SlicePlane[j][i] = NULL;
    }
    m_SliceParametersInitialized  = false;
    m_ShowVolumeBox               = false;
    m_ShowBounds									= false;
    m_AssemblyUsed = NULL;
    m_ColorLUT  = NULL;
    m_CustomColorLUT = NULL;
    m_Box = NULL;
    m_Mapper = NULL;
    m_Actor = NULL;
    m_TickActor = NULL;
    m_SliceDirection  = albaPipeVolumeOrthoSlice::SLICE_Z;
    m_SliceOpacity  = 1.0;
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds)
//----------------------------------------------------------------------------
{
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
  m_ShowBounds = show_bounds;
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::InitializeSliceParameters(int direction, double slice_origin0[3],double slice_origin1[3], bool show_vol_bbox,bool show_bounds)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
  m_ShowBounds = show_bounds;
  m_Origin[0][0] = slice_origin0[0];
  m_Origin[0][1] = slice_origin0[1];
  m_Origin[0][2] = slice_origin0[2];
  m_Origin[1][0] = slice_origin1[0];
  m_Origin[1][1] = slice_origin1[1];
  m_Origin[1][2] = slice_origin1[2];
}
void albaPipeVolumeSliceBlend::Create(albaSceneNode *n)
  //----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  m_AssemblyUsed = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  assert(m_Vme->IsALBAType(albaVMEVolume));
  double b[6];
  //Update input data
  m_Vme->GetOutput()->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  mmaVolumeMaterial *material = ((albaVMEVolume *)m_Vme)->GetMaterial();
  //If material has a valid table range use it
  if (material->GetTableRange()[1] < material->GetTableRange()[0]) 
  {
		material->SetTableRange(m_Vme->GetOutput()->GetVTKData()->GetScalarRange());
  }

  //Update material
  m_ColorLUT = material->m_ColorLut;
  material->UpdateProp();

  double sr[2];
  m_Vme->GetOutput()->GetVTKData()->GetScalarRange(sr);
  m_ColorLUT->SetTableRange(sr[0], sr[1]);
  material->UpdateFromTables();
  if (!m_SliceParametersInitialized)
  {
    //If slices aren't initialized
    m_Origin[0][0] = (b[0] + b[1])*.5;
    m_Origin[0][1] = (b[2] + b[3])*.5;
    m_Origin[0][2] = (b[4] + b[5])*.5;

    m_Origin[1][0] = (b[0] + b[1])*.5;
    m_Origin[1][1] = (b[2] + b[3])*.5;
    m_Origin[1][2] = (b[4] + b[5])*.5;
  }
  
  CreateSlice(m_SliceDirection);

  //Create selection actor
  vtkNEW(m_VolumeBox);
  m_VolumeBox->SetInput(m_Vme->GetOutput()->GetVTKData());

  vtkNEW(m_VolumeBoxMapper);
  m_VolumeBoxMapper->SetInput(m_VolumeBox->GetOutput());

  vtkNEW(m_VolumeBoxActor);
  m_VolumeBoxActor->SetMapper(m_VolumeBoxMapper);
  m_VolumeBoxActor->PickableOff();
  m_VolumeBoxActor->SetVisibility(m_ShowVolumeBox);
  if(m_AssemblyUsed==m_AssemblyFront)
    m_VolumeBoxActor->SetScale(1.01);
  else
    m_VolumeBoxActor->SetScale(1);
  m_AssemblyFront->AddPart(m_VolumeBoxActor);

  if(m_ShowBounds)
  {
    double bounds[6];
    m_Vme->GetOutput()->Update();
    m_Vme->GetOutput()->GetVMELocalBounds(bounds);
    vtkNEW(m_Box);
    m_Box->SetBounds(bounds);
    vtkNEW(m_Mapper);
    m_Mapper->SetInput(m_Box->GetOutput());
    vtkNEW(m_Actor);
    m_Actor->SetMapper(m_Mapper);
    m_AssemblyUsed->AddPart(m_Actor);
  }

  // if the actor is in the background renderer
  // create something invisible in the front renderer so that ResetCamera will work
  for(int i=0;i<2;i++)
  {
    if(m_AssemblyBack != NULL)
    {
      vtkNEW(m_GhostActor[i]);
      m_GhostActor[i]->SetMapper(m_SliceMapper[i][m_SliceDirection]);
      m_GhostActor[i]->PickableOff();
      m_GhostActor[i]->GetProperty()->SetOpacity(0);
      m_GhostActor[i]->GetProperty()->SetRepresentationToPoints();
      m_GhostActor[i]->GetProperty()->SetInterpolationToFlat();
      m_AssemblyFront->AddPart(m_GhostActor[i]);
    }
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::CreateSlice(int direction)
//----------------------------------------------------------------------------
{
	double bounds[6];
	double xmin, xmax, ymin, ymax, zmin, zmax;

  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();
 	vtk_data->GetBounds(bounds);
		
	xmin = bounds[0];
	xmax = bounds[1];
	ymin = bounds[2];
	ymax = bounds[3];
	zmin = bounds[4];
	zmax = bounds[5];
	for(int i=0;i<2;i++)
  {
    vtkNEW(m_Slicer[i][direction]);
		m_Slicer[i][direction]->SetSclicingMode(direction);
    m_Slicer[i][direction]->SetPlaneOrigin(m_Origin[i]);
    m_Slicer[i][direction]->SetInput(vtk_data);



    vtkNEW(m_Texture[i][direction]);
    m_Texture[i][direction]->RepeatOff();
    m_Texture[i][direction]->InterpolateOn();
    m_Texture[i][direction]->SetQualityTo32Bit();
    m_Texture[i][direction]->SetInput((vtkImageData*)m_Slicer[i][direction]->GetOutput());
    m_Texture[i][direction]->SetLookupTable(m_ColorLUT);
    m_Texture[i][direction]->MapColorScalarsThroughLookupTableOn();

		//---- pipeline for the Plane --------------------------
		m_SlicePlane[i][direction] = vtkPlaneSource::New();
		

		switch (direction)
		{
			case albaPipeVolumeOrthoSlice::SLICE_X:
				m_SlicePlane[i][direction]->SetOrigin(m_Origin[i][0], ymin, zmin);
				m_SlicePlane[i][direction]->SetPoint1(m_Origin[i][0], ymax, zmin);
				m_SlicePlane[i][direction]->SetPoint2(m_Origin[i][0], ymin, zmax);
				break;
			case albaPipeVolumeOrthoSlice::SLICE_Y:
				m_SlicePlane[i][direction]->SetOrigin(xmin, m_Origin[i][1], zmin);
				m_SlicePlane[i][direction]->SetPoint1(xmax, m_Origin[i][1], zmin);
				m_SlicePlane[i][direction]->SetPoint2(xmin, m_Origin[i][1], zmax);
				break;
			case albaPipeVolumeOrthoSlice::SLICE_Z:
				m_SlicePlane[i][direction]->SetOrigin(xmin, ymin, m_Origin[i][2]);
				m_SlicePlane[i][direction]->SetPoint1(xmax, ymin, m_Origin[i][2]);
				m_SlicePlane[i][direction]->SetPoint2(xmin, ymax, m_Origin[i][2]);
				break;
		}


		vtkNEW(m_SliceMapper[i][direction]);
		m_SliceMapper[i][direction]->SetInput(m_SlicePlane[i][direction]->GetOutput());
    m_SliceMapper[i][direction]->ScalarVisibilityOff();

    vtkNEW(m_SliceActor[i][direction]);
    m_SliceActor[i][direction]->SetMapper(m_SliceMapper[i][direction]);
    m_SliceActor[i][direction]->SetTexture(m_Texture[i][direction]);
    m_SliceActor[i][direction]->GetProperty()->SetAmbient(1.f);
    m_SliceActor[i][direction]->GetProperty()->SetDiffuse(0.f);
    m_SliceActor[i][direction]->GetProperty()->SetOpacity(m_SliceOpacity);

    SetSliceOpacity(m_SliceOpacity);

    m_AssemblyUsed->AddPart(m_SliceActor[i][direction]);
  }
}
//----------------------------------------------------------------------------
albaPipeVolumeSliceBlend::~albaPipeVolumeSliceBlend()
//----------------------------------------------------------------------------
{
  //remove all actors from renderer
  if(m_VolumeBoxActor)
    m_AssemblyFront->RemovePart(m_VolumeBoxActor);
  if(m_Actor)
    m_AssemblyUsed->RemovePart(m_Actor);
  if(m_TickActor)
    m_AssemblyUsed->RemovePart(m_TickActor);
  //Delete all pipe stuff
  for(int j=0;j<2;j++)
  {
		for (int i = 0; i < 3; i++)
		{
			if (m_SliceActor[j][i])
				m_AssemblyUsed->RemovePart(m_SliceActor[j][i]);
			vtkDEL(m_Slicer[j][i]);
			vtkDEL(m_Image[j][i]);
			vtkDEL(m_Texture[j][i]);
			vtkDEL(m_SliceMapper[j][i]);
			vtkDEL(m_SliceActor[j][i]);
			if (m_SlicePlane[j][i])
				vtkDEL(m_SlicePlane[j][i]);
		}
  }

  vtkDEL(m_VolumeBox);
  vtkDEL(m_VolumeBoxMapper);
  vtkDEL(m_VolumeBoxActor);
  vtkDEL(m_Actor);
  vtkDEL(m_TickActor);

  for(int i=0;i<2;i++)
  {
    if(m_GhostActor[i]) 
      m_AssemblyFront->RemovePart(m_GhostActor[i]);
    vtkDEL(m_GhostActor[i]);
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::SetLutRange(double low, double high)
//----------------------------------------------------------------------------
{
  //Update material with new LUT range
  mmaVolumeMaterial *material = ((albaVMEVolume *)m_Vme)->GetMaterial();
  material->m_Window_LUT = high-low;
  material->m_Level_LUT  = (low+high)*.5;
	material->SetTableRange(low, high);
  material->UpdateProp();
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = ((albaVMEVolume *)m_Vme)->GetMaterial();
  material->m_ColorLut->GetTableRange(range);
}

//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::SetSlice(int nSlice,double origin[3])
//----------------------------------------------------------------------------
{
  //set new slice position
  m_Origin[nSlice][0] = origin[0];
  m_Origin[nSlice][1] = origin[1];
  m_Origin[nSlice][2] = origin[2];
  for(int j=0;j<2;j++)
  {
    for(int i=0;i<3;i++)
    {
      if(m_Slicer[j][i])
      {
        m_Slicer[j][i]->SetPlaneOrigin(m_Origin[j]);
        m_Slicer[j][i]->Update();
				m_SlicePlane[j][i]->Update();
      }
    }
  }
  SetSliceOpacity(m_SliceOpacity);
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::GetSliceOrigin(int nSlice,double origin[3])
//----------------------------------------------------------------------------
{
  origin[0] = m_Origin[nSlice][0];
  origin[1] = m_Origin[nSlice][1];
  origin[2] = m_Origin[nSlice][2];
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::SetSliceOpacity(double opacity)
  //----------------------------------------------------------------------------
{
  m_SliceOpacity = opacity;

  for (int i=0;i<3;i++)
  {
    //Implemented only for z-axis
    if(m_SliceDirection== albaPipeVolumeOrthoSlice::SLICE_Z)
    {
      //Check witch slice are at the TOP
      if(m_Origin[0][2]<=m_Origin[1][2])
      {
        if(m_SliceActor[0][i]&&m_SliceActor[1][i])
        {
          m_SliceActor[0][i]->GetProperty()->SetOpacity(m_SliceOpacity);
          //Set opacity 1 for slice at bottom
          m_SliceActor[1][i]->GetProperty()->SetOpacity(1.0);
        }
      }
      else
      {
        if(m_SliceActor[0][i]&&m_SliceActor[1][i])
        {
          m_SliceActor[1][i]->GetProperty()->SetOpacity(m_SliceOpacity);
          //Set opacity 1 for slice at bottom
          m_SliceActor[0][i]->GetProperty()->SetOpacity(1.0);
        }
      }
    }
  }
}
//----------------------------------------------------------------------------
float albaPipeVolumeSliceBlend::GetSliceOpacity()
  //----------------------------------------------------------------------------
{
  return m_SliceOpacity;
}
//----------------------------------------------------------------------------
albaGUI *albaPipeVolumeSliceBlend::CreateGui()
  //----------------------------------------------------------------------------
{
  //Create pipe gui
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  m_Gui->Lut(ID_LUT_CHOOSER,"Lut",m_ColorLUT);
  m_Gui->FloatSlider(ID_OPACITY_SLIDER,"Opacity",&m_SliceOpacity,0.0,1.0);
  m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::OnEvent(albaEventBase *alba_event)
  //----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
    case ID_LUT_CHOOSER:
      {
        mmaVolumeMaterial *material = ((albaVMEVolume *)m_Vme)->GetMaterial();
        material->UpdateFromTables();
				GetLogicManager()->CameraUpdate();
      }
      break;
    case ID_OPACITY_SLIDER:
      {
        //Event to modify the opacity of the slice at top
        SetSliceOpacity(m_SliceOpacity);
				GetLogicManager()->CameraUpdate();
      }
      break;
    default:
      //Other events
      albaEventMacro(*alba_event);
      break;
    }
  }
  else
  {
    albaEventMacro(*alba_event);
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::SetColorLookupTable(vtkLookupTable *lut)
  //----------------------------------------------------------------------------
{
  //Set for every slice the new LUT
  int i;
  if (lut == NULL)
  {
    for(int j=0;j<2;j++)
    {
      for (i=0;i<3;i++)
      {
        if(m_Texture[j][i])
          m_Texture[j][i]->SetLookupTable(m_ColorLUT);
      }
    }
    return;
  }
  m_CustomColorLUT = lut;
  for(int j=0;j<2;j++)
  {
    for (i=0;i<3;i++)
    {
      if(m_Texture[j][i])
        m_Texture[j][i]->SetLookupTable(m_CustomColorLUT);
    }
  }
}
//----------------------------------------------------------------------------
void albaPipeVolumeSliceBlend::Select(bool sel)
//----------------------------------------------------------------------------
{
  //Update actor selection
  m_Selected = sel;
  m_ShowVolumeBox = sel;
  m_VolumeBoxActor->SetVisibility(sel);
}
