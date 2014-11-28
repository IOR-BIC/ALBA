/*=========================================================================

 Program: MAF2
 Module: mafViewGlobalSliceCompound
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewGlobalSliceCompound.h"
#include "mafViewGlobalSlice.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "mafGUIFloatSlider.h"
#include "mmaVolumeMaterial.h"

#include "vtkLookupTable.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
  ID_VIEW_GLOBAL_SLICE = 0,
};

enum VIEW_WIDGET_ID
{
	ID_FIRST = MINID,
	ID_HELP,
	ID_LAST
};
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewGlobalSliceCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewGlobalSliceCompound::mafViewGlobalSliceCompound( wxString label, int num_row, int num_col)
: mafViewCompoundWindowing(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	/*m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;*/
}
//----------------------------------------------------------------------------
mafViewGlobalSliceCompound::~mafViewGlobalSliceCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
mafView *mafViewGlobalSliceCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewGlobalSliceCompound *v = new mafViewGlobalSliceCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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

//-------------------------------------------------------------------------
mafGUI* mafViewGlobalSliceCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

  if (buildHelpGui.GetArg() == true)
  {
	  m_Gui->Button(ID_HELP, "Help","");	
  }

	m_Gui->AddGui(((mafViewGlobalSlice*)m_ChildViewList[ID_VIEW_GLOBAL_SLICE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider(0);
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafViewGlobalSliceCompound::PackageView()
//-------------------------------------------------------------------------
{
	m_ViewGlobalSlice = new mafViewGlobalSlice("",CAMERA_OS_P);
	m_ViewGlobalSlice->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("mafVMESurfaceParametric", "mafPipeSurfaceSlice");
	m_ViewGlobalSlice->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  m_ViewGlobalSlice->PlugVisualPipe("medVMELabeledVolume", "mafPipeVolumeSlice");
  m_ViewGlobalSlice->PlugVisualPipe("mafVMEMesh", "mafPipeMeshSlice");
  m_ViewGlobalSlice->PlugVisualPipe("medVMEAnalog", "mafPipeBox", NON_VISIBLE);
  m_ViewGlobalSlice->PlugVisualPipe("mafVMELandmark", "mafPipeSurfaceSlice");
  m_ViewGlobalSlice->PlugVisualPipe("mafVMELandmarkCloud", "mafPipeSurfaceSlice");
	
	PlugChildView(m_ViewGlobalSlice);
}

//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{

		case ID_HELP:
			{
				mafEvent helpEvent;
				helpEvent.SetSender(this);
				mafString viewLabel = this->m_Label;
				helpEvent.SetString(&viewLabel);
				helpEvent.SetId(OPEN_HELP_PAGE);
				mafEventMacro(helpEvent);
			}
			break;

		default:
			mafEventMacro(*maf_event);
		}
	}
	else
	{
		mafEventMacro(*maf_event);
	}
}
