/*=========================================================================

 Program: MAF2
 Module: mafViewSliceNotInterpolatedCompound
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"

#include "mafViewSliceNotInterpolatedCompound.h"
#include "mafPipeVolumeSliceNotInterpolated.h"
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

mafCxxTypeMacro(mafViewSliceNotInterpolatedCompound);

//----------------------------------------------------------------------------
mafViewSliceNotInterpolatedCompound::mafViewSliceNotInterpolatedCompound(wxString label /* = "View Slice not interpolated" */, bool show_ruler /* = false */)
:mafViewCompoundWindowing(label,1,1)
//----------------------------------------------------------------------------
{
  // Initialize parameters
  m_ViewSlice = NULL;
  m_ColorLUT = NULL;
}

//----------------------------------------------------------------------------
mafViewSliceNotInterpolatedCompound::~mafViewSliceNotInterpolatedCompound()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolatedCompound::PackageView()
//----------------------------------------------------------------------------
{
  // Create child slice view
  m_ViewSlice = new mafViewSliceNotInterpolated("",CAMERA_CT);
  m_ViewSlice->PackageView();
  m_ViewSlice->Create();
  PlugChildView(m_ViewSlice);
}

//----------------------------------------------------------------------------
mafView *mafViewSliceNotInterpolatedCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  // Copy the view and sub-views attributes
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSliceNotInterpolatedCompound *v = new mafViewSliceNotInterpolatedCompound(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  for (int i=0;i<m_PluggedChildViewList.size();i++)
  {
    v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
  }
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();
  return v;
}

//----------------------------------------------------------------------------
mafGUI *mafViewSliceNotInterpolatedCompound::CreateGui()
//----------------------------------------------------------------------------
{
  // Create the view gui
  m_Gui = new mafGUI(this);
  m_Gui->AddGui(m_ChildViewList[0]->GetGui());
  return m_Gui;
}

//----------------------------------------------------------------------------
void mafViewSliceNotInterpolatedCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  // Call superclass vme show method
  Superclass::VmeShow(node,show);
}