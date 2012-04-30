/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewSliceNotInterpolated.cpp,v $
  Language:  C++
  Date:      $Date: 2012-04-30 15:44:00 $
  Version:   $Revision: 1.1.2.6 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "medDefines.h"

#include "medViewSliceNotInterpolated.h"
#include "medPipeVolumeSliceNotInterpolated.h"
#include "mafNode.h"
#include "mafViewVTK.h"
#include "mafGUI.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "vtkDataSet.h"
#include "vtkLookupTable.h"
#include "mafGUILutSwatch.h"
#include "wx\sizer.h"
#include "vtkMEDVolumeSlicerNotInterpolated.h"

mafCxxTypeMacro(medViewSliceNotInterpolated);

//----------------------------------------------------------------------------
medViewSliceNotInterpolated::medViewSliceNotInterpolated(wxString label /* = "View Slice not interpolated" */, bool show_ruler /* = false */)
:mafViewVTK(label,CAMERA_OS_Z)
//----------------------------------------------------------------------------
{
  // Initialize parameters
/*  m_ViewSlice = NULL;*/
  m_Bounds[0] = m_Bounds[1] = m_Bounds[2] = m_Bounds[3] = m_Bounds[4] = m_Bounds[5] = .0;
  m_SliceAxis = 2;
  m_CurrentSlice = 0;
  m_SliceSlider = NULL;
  m_ColorLUT = NULL;
}

//----------------------------------------------------------------------------
medViewSliceNotInterpolated::~medViewSliceNotInterpolated()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::PackageView()
//----------------------------------------------------------------------------
{
  PlugVisualPipe("mafVMEVolumeGray", "medPipeVolumeSliceNotInterpolated");
}

//----------------------------------------------------------------------------
mafView *medViewSliceNotInterpolated::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  // Copy the view and sub-views attributes
  m_LightCopyEnabled = lightCopyEnabled;
  medViewSliceNotInterpolated *v = new medViewSliceNotInterpolated(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->PlugVisualPipe("mafVMEVolumeGray", "medPipeVolumeSliceNotInterpolated");
  v->Create();
  return v;
}

//----------------------------------------------------------------------------
mafGUI *medViewSliceNotInterpolated::CreateGui()
//----------------------------------------------------------------------------
{
  // Create the view gui
  m_Gui = new mafGUI(this);
  m_LutSwatch = m_Gui->Lut(ID_LUT,"LUT",m_ColorLUT); // Lut widget
  wxString choices[3] = {"X","Y","Z"};
  m_Gui->Combo(ID_AXIS,"Axis",&m_SliceAxis,3,choices); // Slice Axis
  m_SliceSlider = m_Gui->FloatSlider(ID_SLICE,"Slice",&m_CurrentSlice,MINDOUBLE,MAXDOUBLE); // Current slice coordinate
  m_Gui->Divider();
  EnableGuiWidgets(false);
  return m_Gui;
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  // Call superclass vme show method
  Superclass::VmeShow(node,show);
  mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(node);
  if(volume)
  {
    if(show)
    {
      // Get the volume visual pipe
      m_PipesSlice[node] = medPipeVolumeSliceNotInterpolated::SafeDownCast(this->GetNodePipe(node));
      assert(m_PipesSlice[node]);
      // Get the vme parameters (bounds and lut)
      GetVolumeParameters(volume);
      // Update gui
      m_LutSwatch->SetLut(m_ColorLUT);
      m_SliceSlider->SetRange(m_Bounds[m_SliceAxis * 2], m_Bounds[(m_SliceAxis * 2) + 1]);
      // Set the pipe lut
      m_PipesSlice[node]->SetLut(m_ColorLUT);
      m_PipesSlice[node]->SetSlice(m_CurrentSlice,m_SliceAxis);
      CameraReset();
      CameraUpdate();
    }
    else
    {
      // De-reference the pipe
      m_PipesSlice.erase(node);
    }
    EnableGuiWidgets(show);
  }
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::EnableGuiWidgets(bool enable)
//----------------------------------------------------------------------------
{
  // Enable/disable gui widgets
  m_Gui->Enable(ID_LUT,enable);
  m_Gui->Enable(ID_AXIS,enable);
  m_Gui->Enable(ID_SLICE,enable);
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::GetVolumeParameters(mafVMEVolumeGray *volume)
//----------------------------------------------------------------------------
{
  vtkDataSet * data = volume->GetOutput()->GetVTKData();
  m_ColorLUT = volume->GetMaterial()->m_ColorLut;
  // Get bounds
  data->GetBounds(m_Bounds);
  // Evaluate the origin from bounds and current slice
  m_CurrentSlice = m_Bounds[m_SliceAxis * 2];

}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::OnEvent(mafEventBase * event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    // Trap gui events
    switch(e->GetId()) 
    {
    case ID_LUT:
      {
        // Set the pipe lut
        for(std::map<mafNode*,medPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
        {
          it->second->SetLut(m_ColorLUT);
        }
        CameraUpdate();
      } break;
    case ID_AXIS:
      {
        SetSliceAxis();
        switch(m_SliceAxis)
        {
          case vtkMEDVolumeSlicerNotInterpolated::SLICE_X:
            {
              m_CameraPositionId = CAMERA_OS_X;
            } break;
          case vtkMEDVolumeSlicerNotInterpolated::SLICE_Y:
            {
              m_CameraPositionId = CAMERA_OS_Y;
            } break;
          case vtkMEDVolumeSlicerNotInterpolated::SLICE_Z:
            {
              m_CameraPositionId = CAMERA_OS_Z;
            } break;
        }
        CameraSet(m_CameraPositionId);
        CameraReset();
        CameraUpdate();
      } break;
    case ID_SLICE:
      {
        // Update the pipe
        for(std::map<mafNode*,medPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
        {
          it->second->SetSlice(m_CurrentSlice,m_SliceAxis);
        }
        CameraUpdate();
      } break;
    case ID_RANGE_MODIFIED:
      {
        Superclass::OnEvent(event);
        // Set the pipe lut
        for(std::map<mafNode*,medPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
        {
          it->second->SetLut(m_ColorLUT);
        }
      }
    default:
      {
        Superclass::OnEvent(event);
      }
    }
  }
  mafEventMacro(*event);
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::SetSliceAxis(int axis)
//----------------------------------------------------------------------------
{
  m_SliceAxis = axis;
  SetSliceAxis();
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::SetSliceAxis()
//----------------------------------------------------------------------------
{
  // Update parameters
  m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
  m_SliceSlider->SetRange(m_Bounds[m_SliceAxis * 2], m_Bounds[(m_SliceAxis * 2) + 1]);
  m_Gui->Update();

  UpdateSlice();
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::SetSlice(double position)
//----------------------------------------------------------------------------
{
  assert(position >= m_Bounds[m_SliceAxis * 2] && position <= m_Bounds[(m_SliceAxis * 2) + 1]);

  // Update parameters
  m_CurrentSlice = position;
  m_Gui->Update();

  UpdateSlice();
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
  assert(origin[m_SliceAxis] >= m_Bounds[m_SliceAxis * 2] && origin[m_SliceAxis] <= m_Bounds[(m_SliceAxis * 2) + 1]);

  // Update parameters
  m_CurrentSlice = origin[m_SliceAxis];
  m_Gui->Update();

  UpdateSlice();
}

//----------------------------------------------------------------------------
void medViewSliceNotInterpolated::UpdateSlice()
//----------------------------------------------------------------------------
{
  // Update the pipe
  for(std::map<mafNode*,medPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
  {
    it->second->SetSlice(m_CurrentSlice,m_SliceAxis);
  }
  CameraReset();
  CameraUpdate();
}