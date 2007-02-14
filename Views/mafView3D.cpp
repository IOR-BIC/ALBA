/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView3D.cpp,v $
  Language:  C++
  Date:      $Date: 2007-02-14 13:53:22 $
  Version:   $Revision: 1.3 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafView3D.h"
#include "mafPipeFactory.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"
#include "mmgGui.h"
#include "mafPipe.h"
#include "mafPipeVolumeProjected.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESlicer.h"
#include "mafPipeIsosurface.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"

enum ID_PIPE
{
  ID_PIPE_ISO = 0,
	ID_PIPE_MIP ,
	ID_PIPE_DRR,
	ID_PIPE_VR
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafView3D);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafView3D::mafView3D(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid,show_ruler,stereo)
//----------------------------------------------------------------------------
{
	m_Choose = ID_PIPE_ISO;
}
//----------------------------------------------------------------------------
mafView3D::~mafView3D()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafView *mafView3D::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafView3D *v = new mafView3D(m_Label, m_CameraPosition, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafView3D::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
			case ID_COMBO_PIPE:
				{
					mafNode *selectedvme=this->GetSceneGraph()->GetSelectedVme();
					if(((mafVME*)selectedvme)->GetVisualPipe())
					{
						this->VmeShow(selectedvme,false);
						wxBusyCursor wait;
						if(m_Choose == ID_PIPE_ISO)
						{
							this->PlugVisualPipe("mafVMEVolumeGray","mafPipeIsosurface");
						}
            else if(m_Choose == ID_PIPE_MIP)
            {
              this->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeMIP");
            }
						else if(m_Choose == ID_PIPE_DRR)
            {
							this->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeDRR");
            }
            else if(m_Choose == ID_PIPE_VR)
            {
              this->PlugVisualPipe("mafVMEVolumeGray","medPipeVolumeVR");
            }
						this->VmeShow(selectedvme,true);
						mafEventMacro(mafEvent(this,VME_SELECT,selectedvme->GetParent()));
            mafEventMacro(mafEvent(this,VME_SELECT,selectedvme));
						//mafEventMacro(mafEvent(this,VME_SELECT,selectedvme,true));
					}
				}
			break;
      default:
        mafEventMacro(*maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafView3D::Create()
//----------------------------------------------------------------------------
{
  m_Rwi = new mafRWI(mafGetFrame(), TWO_LAYER, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();

	this->PlugVisualPipe("mafVMEVolumeGray","mafPipeIsosurface");
}
//----------------------------------------------------------------------------
void mafView3D::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->m_Pipe);
      if(pipe_name == "mafPipeIsosurface")
      {
        ((mafPipeIsosurface *)pipe)->EnableBoundingBoxVisibility(false);
      }
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
      if (m_NumberOfVisibleVme == 1)
      {
        CameraReset();
      }
      else
      {
        CameraUpdate();
      }
    }
    else
    {
      mafErrorMessage(_("Cannot create visual pipe object of type \"%s\"!"),pipe_name.GetCStr());
    }
  }
}
/*
//----------------------------------------------------------------------------
void mafView3D::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_NumberOfVisibleVme--;
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}*/
//-------------------------------------------------------------------------
mmgGui *mafView3D::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
	wxString choices[4] = {"ISO","MIP","DRR","VR"};
	m_Gui->Combo(ID_COMBO_PIPE,_("Choose pipe"),&m_Choose,4,choices);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafView3D::VmeSelect(mafNode *vme, bool select)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);

	if(m_Gui)
	{
		if(vme->IsA("mafVMEVolumeGray"))
		{
			m_Gui->Enable(ID_COMBO_PIPE,select);
		}
		else
		{
			m_Gui->Enable(ID_COMBO_PIPE,false);
		}
	}
	CameraUpdate();
}
//-------------------------------------------------------------------------
int mafView3D::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
	mafSceneNode *n = NULL;
	if (m_Sg != NULL)
	{
		if (vme->IsMAFType(mafVMEVolumeGray))
		{
			n = m_Sg->Vme2Node(vme);
			n->m_Mutex = true;
		}
		else
		{
			n = m_Sg->Vme2Node(vme);
			n->m_PipeCreatable = false;
		}
	}

	return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}