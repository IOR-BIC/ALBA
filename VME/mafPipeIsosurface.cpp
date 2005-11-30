/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeIsosurface.cpp,v $
Language:  C++
Date:      $Date: 2005-11-30 14:49:56 $
Version:   $Revision: 1.3 $
Authors:   Alexander Savenko  -  Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#if defined(WIN32)
#pragma warning (disable : 4018)
#endif

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeIsosurface.h"
#include "mafEvent.h"
#include "mafSceneNode.h"
#include "mmgFloatSlider.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkContourVolumeMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeIsosurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeIsosurface::mafPipeIsosurface() 
: mafPipe()
//----------------------------------------------------------------------------
{
  m_Isosurface      = NULL;
  m_IsosurfaceMapper= NULL;
  m_IsosurfaceActor = NULL;
  m_OutlineActor    = NULL;
  m_ContourMapper   = NULL; 
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_ContourSlider   = NULL;

  m_ContourValue    = 300.0;
  m_Opacity         = 1.0;
}
//----------------------------------------------------------------------------
void mafPipeIsosurface::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  assert(m_Vme->IsMAFType(mafVMEVolumeGray));

  vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();
  dataset->Update();

  // contour pipeline
  vtkNEW(m_ContourMapper);
  m_ContourMapper->SetInput(dataset);
  m_ContourMapper->EnableAutoLODOn();

  vtkNEW(m_Isosurface);
  
  vtkNEW(m_IsosurfaceMapper);
  m_IsosurfaceMapper->SetInput(m_Isosurface);
  
  vtkNEW(m_IsosurfaceActor);
  m_IsosurfaceActor->SetMapper(m_IsosurfaceMapper);
  m_IsosurfaceActor->GetProperty()->SetOpacity(m_Opacity);

  double range[2] = {0, 0};
  dataset->GetScalarRange(range);
  float value = 0.5f * (range[0] + range[1]);
  while (value < range[1] && m_ContourMapper->EstimateRelevantVolume(value) > 0.3f)
    value += 0.05f * (range[1] + range[0]) + 1.f;
  m_ContourMapper->SetContourValue(value);
  m_ContourMapper->GetOutput(0, m_Isosurface);
  m_ContourMapper->Update();
  m_Isosurface->Modified();

  m_ContourValue = m_ContourMapper->GetContourValue();

  m_AssemblyFront->AddPart(m_IsosurfaceActor);

  // selection box
  vtkNEW(m_OutlineBox);
  m_OutlineBox->SetInput(dataset);
  
  vtkNEW(m_OutlineMapper);
  m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());
  
  vtkNEW(m_OutlineActor);
  m_OutlineActor->SetMapper(m_OutlineMapper);
  m_OutlineActor->VisibilityOn();
  m_OutlineActor->PickableOff();

  vtkMAFSmartPointer<vtkProperty> property;
	property->SetColor(1,1,1);
	property->SetAmbient(1);
	property->SetRepresentationToWireframe();
	property->SetInterpolationToFlat();
  m_OutlineActor->SetProperty(property);
  
  m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
mafPipeIsosurface::~mafPipeIsosurface()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_IsosurfaceActor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_Isosurface);
  vtkDEL(m_IsosurfaceMapper);
  vtkDEL(m_IsosurfaceActor);
  vtkDEL(m_OutlineActor);
  vtkDEL(m_ContourMapper);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
}

//----------------------------------------------------------------------------
void mafPipeIsosurface::Select(bool sel) 
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if (m_IsosurfaceActor->GetVisibility())
  {
    m_OutlineActor->SetVisibility(sel);
  }
}

//----------------------------------------------------------------------------
bool mafPipeIsosurface::SetContourValue(float value) 
//----------------------------------------------------------------------------
{
  if (m_ContourMapper == NULL)
    return false;
  m_ContourMapper->SetContourValue(value);
  m_ContourMapper->GetOutput(0, m_Isosurface);
  m_ContourMapper->Update();
  m_Isosurface->Modified();
  return true;
}

//----------------------------------------------------------------------------
float mafPipeIsosurface::GetContourValue() 
//----------------------------------------------------------------------------
{
  if (m_ContourMapper == NULL)
    return 0.;
  return m_ContourMapper->GetContourValue();
}
//----------------------------------------------------------------------------
mmgGui *mafPipeIsosurface::CreateGui()
//----------------------------------------------------------------------------
{
  double range[2] = {0, 0};
  m_Vme->GetOutput()->GetVTKData()->GetScalarRange(range);

  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_ContourSlider = m_Gui->FloatSlider(ID_CONTOUR_VALUE,"contour", &m_ContourValue,range[0],range[1]);
  m_Gui->Double(ID_ISOSURFACE_OPACITY,"opacity",&m_Opacity,0.01,1.0);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeIsosurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_CONTOUR_VALUE:
      {
        SetContourValue((float)m_ContourValue);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        //m_Vme->ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_ISOSURFACE_OPACITY:
        m_IsosurfaceActor->GetProperty()->SetOpacity(m_Opacity);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      default:
      break;
    }
  }
}
