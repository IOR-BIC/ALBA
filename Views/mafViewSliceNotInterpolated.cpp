/*=========================================================================

 Program: MAF2
 Module: mafViewSliceNotInterpolated
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"

#include "mafViewSliceNotInterpolated.h"
#include "mafPipeVolumeSliceNotInterpolated.h"
#include "mafVME.h"
#include "mafViewVTK.h"
#include "mafGUI.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "vtkDataSet.h"
#include "vtkLookupTable.h"
#include "mafGUILutSwatch.h"
#include "wx\sizer.h"
#include "vtkMAFVolumeSlicerNotInterpolated.h"

mafCxxTypeMacro(mafViewSliceNotInterpolated);

//----------------------------------------------------------------------------
mafViewSliceNotInterpolated::mafViewSliceNotInterpolated(wxString label /* = "View Slice not interpolated" */, bool show_ruler /* = false */)
:mafViewVTK(label,CAMERA_OS_Z)
//----------------------------------------------------------------------------
{
  // Initialize parameters
/*  m_ViewSlice = NULL;*/
  m_Bounds[0] = m_Bounds[1] = m_Bounds[2] = m_Bounds[3] = m_Bounds[4] = m_Bounds[5] = .0;
  m_SliceAxis = 2;
  m_CurrentSlice = VTK_DOUBLE_MAX;
  m_SliceSlider = NULL;
  m_ColorLUT = NULL;
}

//----------------------------------------------------------------------------
mafViewSliceNotInterpolated::~mafViewSliceNotInterpolated()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::PackageView()
//----------------------------------------------------------------------------
{
  PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSliceNotInterpolated");
}

//----------------------------------------------------------------------------
mafView *mafViewSliceNotInterpolated::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  // Copy the view and sub-views attributes
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSliceNotInterpolated *v = new mafViewSliceNotInterpolated(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSliceNotInterpolated");
  v->Create();
  return v;
}

//----------------------------------------------------------------------------
mafGUI *mafViewSliceNotInterpolated::CreateGui()
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
void mafViewSliceNotInterpolated::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  // Call superclass vme show method
  Superclass::VmeShow(vme,show);
  mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(vme);
  if(volume)
  {
    if(show)
    {
      // Get the volume visual pipe
      m_PipesSlice[vme] = mafPipeVolumeSliceNotInterpolated::SafeDownCast(this->GetNodePipe(vme));
      assert(m_PipesSlice[vme]);
      // Get the vme parameters (bounds and lut)
      GetVolumeParameters(volume);
      // Update gui
      m_LutSwatch->SetLut(m_ColorLUT);
      m_SliceSlider->SetRange(m_Bounds[m_SliceAxis * 2], m_Bounds[(m_SliceAxis * 2) + 1]);
      // Set the pipe lut
      m_PipesSlice[vme]->SetLut(m_ColorLUT);
      m_PipesSlice[vme]->SetSlice(m_CurrentSlice,m_SliceAxis);
//       CameraReset();
//       CameraUpdate();
    }
    else
    {
      // De-reference the pipe
      m_PipesSlice.erase(vme);
    }
    EnableGuiWidgets(show);
  }
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::EnableGuiWidgets(bool enable)
//----------------------------------------------------------------------------
{
  // Enable/disable gui widgets
  m_Gui->Enable(ID_LUT,enable);
  m_Gui->Enable(ID_AXIS,enable);
  m_Gui->Enable(ID_SLICE,enable);
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::GetVolumeParameters(mafVMEVolumeGray *volume)
//----------------------------------------------------------------------------
{
  vtkDataSet * data = volume->GetOutput()->GetVTKData();
  m_ColorLUT = volume->GetMaterial()->m_ColorLut;
  // Get bounds
  data->GetBounds(m_Bounds);
  // Evaluate the origin from bounds and current slice
  if(m_CurrentSlice == VTK_DOUBLE_MAX)
  {
    m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
  }
  if(m_CurrentSlice < m_Bounds[m_SliceAxis * 2])
  {
    m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
    UpdateSlice();
  }
  if(m_CurrentSlice > m_Bounds[m_SliceAxis * 2 + 1])
  {
    m_CurrentSlice = m_Bounds[m_SliceAxis * 2 + 1];
    UpdateSlice();
  }
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::OnEvent(mafEventBase * event)
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
        for(std::map<mafVME*,mafPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
        {
          it->second->SetLut(m_ColorLUT);
        }
        CameraUpdate();
      } break;
    case ID_AXIS:
      {
        SetSliceAxis();
        CameraReset();
        CameraUpdate();
      } break;
    case ID_SLICE:
      {
        // Update the pipe
        for(std::map<mafVME*,mafPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
        {
          it->second->SetSlice(m_CurrentSlice,m_SliceAxis);
        }
        CameraUpdate();
      } break;
    case ID_RANGE_MODIFIED:
      {
        Superclass::OnEvent(event);
        // Set the pipe lut
        for(std::map<mafVME*,mafPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
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
void mafViewSliceNotInterpolated::SetSliceAxis(int axis)
//----------------------------------------------------------------------------
{
  m_SliceAxis = axis;
  SetSliceAxis();
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::SetSliceAxis()
//----------------------------------------------------------------------------
{
  // Update parameters
  m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
  m_SliceSlider->SetRange(m_Bounds[m_SliceAxis * 2], m_Bounds[(m_SliceAxis * 2) + 1]);
  m_Gui->Update();

  switch(m_SliceAxis)
  {
  case vtkMAFVolumeSlicerNotInterpolated::SLICE_X:
    {
      m_CameraPositionId = CAMERA_OS_X;
    } break;
  case vtkMAFVolumeSlicerNotInterpolated::SLICE_Y:
    {
      m_CameraPositionId = CAMERA_OS_Y;
    } break;
  case vtkMAFVolumeSlicerNotInterpolated::SLICE_Z:
    {
      m_CameraPositionId = CAMERA_OS_Z;
    } break;
  }
  CameraSet(m_CameraPositionId);

  UpdateSlice();
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::SetSlice(double position)
//----------------------------------------------------------------------------
{
  assert(position >= m_Bounds[m_SliceAxis * 2] && position <= m_Bounds[(m_SliceAxis * 2) + 1]);

  // Update parameters
  m_CurrentSlice = position;
  m_Gui->Update();

  UpdateSlice();
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
  //assert(origin[m_SliceAxis] >= m_Bounds[m_SliceAxis * 2] && origin[m_SliceAxis] <= m_Bounds[(m_SliceAxis * 2) + 1]);

  // Update parameters
  m_CurrentSlice = origin[m_SliceAxis];
  m_Gui->Update();

  UpdateSlice();
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::UpdateSlice()
//----------------------------------------------------------------------------
{
  // Update the pipe
  for(std::map<mafVME*,mafPipeVolumeSliceNotInterpolated*>::iterator it = m_PipesSlice.begin(); it != m_PipesSlice.end(); it++)
  {
    it->second->SetSlice(m_CurrentSlice,m_SliceAxis);
  }
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolated::SetLut(mafVME *volume, vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_PipesSlice[volume]->SetLut(lut);
}