/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView3D.cpp,v $
  Language:  C++
  Date:      $Date: 2007-02-23 15:32:49 $
  Version:   $Revision: 1.4 $
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
#include "medPipeVolumeDRR.h"
#include "mmgFloatSlider.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkXRayVolumeMapper.h"

enum ID_PIPE
{
  ID_PIPE_ISO = 0,
	ID_PIPE_MIP ,
	ID_PIPE_DRR,
	ID_PIPE_VR,
	ID_PIPE_ALL,
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
	m_CurrentVolume = NULL;
	m_SliderContourIso = NULL;
	m_SliderAlphaIso = NULL;
}
//----------------------------------------------------------------------------
mafView3D::~mafView3D()
//----------------------------------------------------------------------------
{
	m_CurrentVolume = NULL;
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
		case ID_CONTOUR_VALUE_ISO:
			{
				mafPipeIsosurface *pipe=mafPipeIsosurface::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetContourValue((float)m_ContourValueIso);
					CameraUpdate();
				}
			}
			break;
		case ID_ALPHA_VALUE_ISO:
			{
				mafPipeIsosurface *pipe=mafPipeIsosurface::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetAlphaValue(m_AlphaValueIso);
					CameraUpdate();
				}
			}
			break;
		case ID_COMBO_PIPE:
			{
				if(((mafVME*)m_CurrentVolume)->GetVisualPipe())
				{
					mafVMEVolumeGray *TempVolume=m_CurrentVolume;
					this->VmeShow(m_CurrentVolume,false);
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
					this->VmeShow(TempVolume,true);

					//mafEventMacro(mafEvent(this,VME_SELECT,selectedvme->GetParent()));
          //mafEventMacro(mafEvent(this,VME_SELECT,selectedvme));
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
	wxString choices[4] = {_("ISO"),_("MIP"),_("DRR"),_("VR")};
	m_Gui->Combo(ID_COMBO_PIPE,_("Choose pipe"),&m_Choose,4,choices);
	m_Gui->Enable(ID_COMBO_PIPE,m_CurrentVolume!=NULL);
	m_Gui->Label("");

	//Isosurface GUI
	m_Gui->Label(_("Isosurface settings:"));
	double range[2] = {VTK_DOUBLE_MIN, VTK_DOUBLE_MAX};
	m_ContourValueIso = 0.0;
	m_SliderContourIso = m_Gui->FloatSlider(ID_CONTOUR_VALUE_ISO,_("contour"), &m_ContourValueIso,range[0],range[1]);
	m_SliderAlphaIso = m_Gui->FloatSlider(ID_ALPHA_VALUE_ISO,_("alpha"), &m_AlphaValueIso,0.0,1.0);

	//DDR GUI
	m_Gui->Label(_("DRR settings:"));
	m_Gui->Color(ID_VOLUME_COLOR, _("Color"), &m_VolumeColor);
	vtkXRayVolumeMapper::GetExposureCorrection(m_ExposureCorrection);
	m_Gui->FloatSlider(ID_EXPOSURE_CORRECTION_L,	_("Min"), &m_ExposureCorrection[0], -1.f, 1.f);
	m_Gui->FloatSlider(ID_EXPOSURE_CORRECTION_H,	_("Max"), &m_ExposureCorrection[1], -1.f, 1.f);
	m_Gui->FloatSlider(ID_GAMMA,	_("Gamma"), &m_Gamma, 0.1f, 3.f);
	m_Gui->Label(_("Camera settings:"));
	vtkCamera *camera = m_Sg->m_RenFront->GetActiveCamera();
	this->m_CameraAngle = camera->GetViewAngle();
	m_Gui->FloatSlider(ID_CAMERA_ANGLE, _("View angle"), &m_CameraAngle, 0.5, 45.0);
	camera->GetPosition(m_CameraPositionDRR);
	m_Gui->Vector(ID_CAMERA_POSITION, _("Position"),	m_CameraPositionDRR);
	camera->GetFocalPoint(m_CameraFocus);
	m_Gui->Vector(ID_CAMERA_FOCUS, _("Focal point"),	m_CameraFocus);
	this->m_CameraRoll = camera->GetRoll();
	m_Gui->FloatSlider(ID_CAMERA_ROLL, _("Roll angle"), &m_CameraRoll, -180., 180.0);

	m_Gui->Label("");

	EnableSubGui(ID_PIPE_ALL,false);

	m_Gui->Update();
  return m_Gui;
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
//-------------------------------------------------------------------------
void mafView3D::EnableSubGui(int idSubPipe,bool enable)
//-------------------------------------------------------------------------
{
	switch(idSubPipe)
	{
		case ID_PIPE_ISO:
			{
				m_Gui->Enable(ID_ALPHA_VALUE_ISO,enable);
				m_Gui->Enable(ID_CONTOUR_VALUE_ISO,enable);

				m_Gui->Enable(ID_VOLUME_COLOR,!enable);
				m_Gui->Enable(ID_EXPOSURE_CORRECTION_L,!enable);
				m_Gui->Enable(ID_EXPOSURE_CORRECTION_H,!enable);
				m_Gui->Enable(ID_GAMMA,!enable);
				m_Gui->Enable(ID_CAMERA_ANGLE,!enable);
				m_Gui->Enable(ID_CAMERA_POSITION,!enable);
				m_Gui->Enable(ID_CAMERA_FOCUS,!enable);
				m_Gui->Enable(ID_CAMERA_ROLL,!enable);
				m_Gui->Enable(ID_IMAGE_COLOR,!enable);
				m_Gui->Enable(ID_IMAGE_OFFSET_X,!enable);
				m_Gui->Enable(ID_IMAGE_OFFSET_Y,!enable);
				m_Gui->Enable(ID_IMAGE_ANGLE,!enable);
			}
			break;
		case ID_PIPE_DRR:
			{
				m_Gui->Enable(ID_ALPHA_VALUE_ISO,!enable);
				m_Gui->Enable(ID_CONTOUR_VALUE_ISO,!enable);

				m_Gui->Enable(ID_VOLUME_COLOR,enable);
				m_Gui->Enable(ID_EXPOSURE_CORRECTION_L,enable);
				m_Gui->Enable(ID_EXPOSURE_CORRECTION_H,enable);
				m_Gui->Enable(ID_GAMMA,enable);
				m_Gui->Enable(ID_CAMERA_ANGLE,enable);
				m_Gui->Enable(ID_CAMERA_POSITION,enable);
				m_Gui->Enable(ID_CAMERA_FOCUS,enable);
				m_Gui->Enable(ID_CAMERA_ROLL,enable);
				m_Gui->Enable(ID_IMAGE_COLOR,enable);
				m_Gui->Enable(ID_IMAGE_OFFSET_X,enable);
				m_Gui->Enable(ID_IMAGE_OFFSET_Y,enable);
				m_Gui->Enable(ID_IMAGE_ANGLE,enable);
			}
			break;
		case ID_PIPE_ALL:
			{
				m_Gui->Enable(ID_ALPHA_VALUE_ISO,enable);
				m_Gui->Enable(ID_CONTOUR_VALUE_ISO,enable);

				m_Gui->Enable(ID_VOLUME_COLOR,enable);
				m_Gui->Enable(ID_EXPOSURE_CORRECTION_L,enable);
				m_Gui->Enable(ID_EXPOSURE_CORRECTION_H,enable);
				m_Gui->Enable(ID_GAMMA,enable);
				m_Gui->Enable(ID_CAMERA_ANGLE,enable);
				m_Gui->Enable(ID_CAMERA_POSITION,enable);
				m_Gui->Enable(ID_CAMERA_FOCUS,enable);
				m_Gui->Enable(ID_CAMERA_ROLL,enable);
				m_Gui->Enable(ID_IMAGE_COLOR,enable);
				m_Gui->Enable(ID_IMAGE_OFFSET_X,enable);
				m_Gui->Enable(ID_IMAGE_OFFSET_Y,enable);
				m_Gui->Enable(ID_IMAGE_ANGLE,enable);
			}
			break;

	}
}
//-------------------------------------------------------------------------
void mafView3D::InizializeSubGui()
//-------------------------------------------------------------------------
{
	switch(m_Choose)
	{
	case ID_PIPE_ISO:
		{
			mafPipeIsosurface *pipe=mafPipeIsosurface::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
			if(pipe)
			{
				double sr[2];
				m_CurrentVolume->GetOutput()->GetVTKData()->GetScalarRange(sr);
				m_ContourValueIso=pipe->GetContourValue();
				m_SliderContourIso->SetRange(sr[0],sr[1],m_ContourValueIso);
				m_SliderContourIso->Update();
				m_SliderContourIso->SetValue(m_ContourValueIso);
				m_SliderContourIso->Update();
				m_AlphaValueIso=pipe->GetAlphaValue();
				m_SliderAlphaIso->SetValue(m_AlphaValueIso);
				m_Gui->Update();
			}
		}
		break;
	case ID_PIPE_DRR:
		{
	
		}
		break;
	}
}
//-------------------------------------------------------------------------
void mafView3D::VmeShow(mafNode *vme,bool show)
//-------------------------------------------------------------------------
{
	Superclass::VmeShow(vme,show);

	if(vme->IsA("mafVMEVolumeGray"))
	{
		if(show)
		{
			m_CurrentVolume = mafVMEVolumeGray::SafeDownCast(vme);
			InizializeSubGui();
			EnableSubGui(m_Choose);
			m_Gui->Enable(ID_COMBO_PIPE,m_CurrentVolume!=NULL);
		}
		else
		{
			m_CurrentVolume = NULL;
			EnableSubGui(ID_PIPE_ALL,false);
			m_Gui->Enable(ID_COMBO_PIPE,m_CurrentVolume!=NULL);
		}
	}
}