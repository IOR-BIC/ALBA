/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaView3D
 Authors: Matteo Giacomoni - Daniele Giunchi
 
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

#include "albaView3D.h"
#include "albaPipeFactory.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaAttachCamera.h"
#include "albaGUI.h"
#include "albaPipe.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"
#include "albaPipeIsosurface.h"
#include "albaPipeVolumeDRR.h"
#include "albaPipeVolumeVR.h"
#include "albaPipeVolumeMIP.h"
#include "albaPipePolyline.h"
#include "albaVMEPolyline.h"
#include "albaGUIFloatSlider.h"

#include "vtkDataSet.h"
#include "vtkALBARayCast3DPicker.h"
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
albaCxxTypeMacro(albaView3D);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaView3D::albaView3D(wxString label, int camera_position, bool show_axes, bool show_grid,  int stereo)
:albaViewVTK(label,camera_position,show_axes,show_grid,stereo)
//----------------------------------------------------------------------------
{
	m_Choose = ID_PIPE_ISO;
	m_CurrentVolume = NULL;
	m_CurrentSurface = NULL;
	m_SliderContourIso = NULL;
	m_SliderAlphaIso = NULL;
	m_ResampleFactor = 0.5;
}
//----------------------------------------------------------------------------
albaView3D::~albaView3D()
//----------------------------------------------------------------------------
{
	m_CurrentVolume = NULL;
	m_CurrentSurface = NULL;
}
//----------------------------------------------------------------------------
albaView *albaView3D::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaView3D *v = new albaView3D(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaView3D::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
		case ID_RESAMPLE_FACTOR:
			{
				albaPipeVolumeDRR *pipeDDR=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipeDDR)
				{
					pipeDDR->SetResampleFactor(m_ResampleFactor);
				}
				albaPipeVolumeVR *pipeVR=albaPipeVolumeVR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipeVR)
				{
					pipeVR->SetResampleFactor(m_ResampleFactor);
				}
				albaPipeVolumeMIP *pipeMIP=albaPipeVolumeMIP::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipeMIP)
				{
					pipeMIP->SetResampleFactor(m_ResampleFactor);
				}
				//CameraReset();
				CameraUpdate();
			}
			break;
		case ID_CONTOUR_VALUE_ISO:
			{
				albaPipeIsosurface *pipe=albaPipeIsosurface::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetContourValue((float)m_ContourValueIso);
					CameraUpdate();
				}
			}
			break;
		case ID_ALPHA_VALUE_ISO:
			{
				albaPipeIsosurface *pipe=albaPipeIsosurface::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetAlphaValue(m_AlphaValueIso);
					CameraUpdate();
				}
			}
			break;
		case ID_EXTRACT_ISO:
			{
				albaPipeIsosurface *pipe=albaPipeIsosurface::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->ExctractIsosurface();
					CameraUpdate();
				}
			}
			break;
		case ID_VOLUME_COLOR:
			{
				albaPipeVolumeDRR *pipe=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
          pipe->SetColor(m_VolumeColor);
					CameraUpdate();
				}
			}
			break;
		case ID_EXPOSURE_CORRECTION_L:
		case ID_EXPOSURE_CORRECTION_H:
			{
				albaPipeVolumeDRR *pipe=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetExposureCorrection(m_ExposureCorrection);
					CameraUpdate();
				}
			}
			break;
		case ID_GAMMA:
			{
				albaPipeVolumeDRR *pipe=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetGamma(m_Gamma);
					CameraUpdate();
				}
			}
			break;
		case ID_CAMERA_ANGLE:
			{
				albaPipeVolumeDRR *pipe=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetCameraAngle(m_CameraAngle);
					CameraUpdate();
				}
			}
			break;
		case ID_CAMERA_POSITION:
			{
				albaPipeVolumeDRR *pipe=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetCameraPosition(m_CameraPositionDRR);
					CameraUpdate();
				}
			}
			break;
		case ID_CAMERA_FOCUS:
			{
				albaPipeVolumeDRR *pipe=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetCameraFocus(m_CameraFocus);
					CameraUpdate();
				}
			}
			break;
		case ID_CAMERA_ROLL:
			{
				albaPipeVolumeDRR *pipe=albaPipeVolumeDRR::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
				if(pipe)
				{
					pipe->SetCameraRoll(m_CameraRoll);
					CameraUpdate();
				}
			}
			break;
		case ID_COMBO_PIPE:
			{
				if(m_CurrentVolume->GetVisualPipe())
				{
					albaVME *TempVolume=m_CurrentVolume;
					if(m_CurrentSurface)
					{
						if(!(m_Choose == ID_PIPE_ISO))
						{
							VmeShow(m_CurrentSurface,false);
						}
						CameraUpdate();
					}
					this->VmeShow(m_CurrentVolume,false);
					wxBusyCursor wait;
					if(m_Choose == ID_PIPE_ISO)
					{
						this->PlugVisualPipe("albaVMEVolumeGray","albaPipeIsosurface");
					}
          else if(m_Choose == ID_PIPE_MIP)
          {
            this->PlugVisualPipe("albaVMEVolumeGray","albaPipeVolumeMIP");
          }
					else if(m_Choose == ID_PIPE_DRR)
          {
						this->PlugVisualPipe("albaVMEVolumeGray","albaPipeVolumeDRR");
          }
          else if(m_Choose == ID_PIPE_VR)
          {
            this->PlugVisualPipe("albaVMEVolumeGray","albaPipeVolumeVR");
          }
					this->VmeShow(TempVolume,true);
				}
			}
			break;
    default:
			Superclass::OnEvent(alba_event);
    break;
    }
  }
  else
  {
		Superclass::OnEvent(alba_event);
  }
}
//----------------------------------------------------------------------------
void albaView3D::Create()
//----------------------------------------------------------------------------
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  m_Rwi = new albaRWI(albaGetFrame(), TWO_LAYER, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new albaSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack,m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();

	this->PlugVisualPipe("albaVMEVolumeGray","albaPipeIsosurface");
}
//----------------------------------------------------------------------------
void albaView3D::VmeCreatePipe(albaVME *vme)
//----------------------------------------------------------------------------
{
  albaString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;
    albaPipeFactory *pipe_factory  = albaPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    albaObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    albaPipe *pipe = (albaPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      albaSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->GetPipe());
			if(pipe_name == "albaPipeVolumeDRR")
			{
				((albaPipeVolumeDRR *)pipe)->SetResampleFactor(m_ResampleFactor);
			}
			if(pipe_name == "albaPipeVolumeVR")
			{
				((albaPipeVolumeVR *)pipe)->SetResampleFactor(m_ResampleFactor);
			}
      if(pipe_name == "albaPipeIsosurface")
      {
        ((albaPipeIsosurface *)pipe)->SetShowOutLine(false);
      }
			if(pipe_name == "albaPipeVolumeMIP")
			{
				((albaPipeVolumeMIP *)pipe)->SetResampleFactor(m_ResampleFactor);
			}
      pipe->Create(n);
      if (m_NumberOfVisibleVme == 1)
      {
        //CameraReset();
      }
      else
      {
        CameraUpdate();
      }
    }
    else
    {
      albaErrorMessage(_("Cannot create visual pipe object of type \"%s\"!"),pipe_name.GetCStr());
    }
  }
}
/*
//----------------------------------------------------------------------------
void albaView3D::VmeDeletePipe(albaVME *vme)
//----------------------------------------------------------------------------
{
  m_NumberOfVisibleVme--;
  albaSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}*/
//-------------------------------------------------------------------------
albaGUI *albaView3D::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

	wxString choices[4] = {_("ISO"),_("MIP"),_("DRR"),_("VR")};
	m_Gui->Combo(ID_COMBO_PIPE,_("Choose pipe"),&m_Choose,4,choices);
	m_Gui->Enable(ID_COMBO_PIPE,m_CurrentVolume!=NULL);
	m_Gui->Double(ID_RESAMPLE_FACTOR,_("Resample"),&m_ResampleFactor,0.000001,1);
	m_Gui->Enable(ID_RESAMPLE_FACTOR,m_CurrentVolume!=NULL);
	m_Gui->Label("");

	//Isosurface GUI
	m_Gui->Label(_("Isosurface settings:"));
	double range[2] = {VTK_DOUBLE_MIN, VTK_DOUBLE_MAX};
	m_ContourValueIso = 0.0;
	m_SliderContourIso = m_Gui->FloatSlider(ID_CONTOUR_VALUE_ISO,_("contour"), &m_ContourValueIso,range[0],range[1]);
	m_SliderAlphaIso = m_Gui->FloatSlider(ID_ALPHA_VALUE_ISO,_("alpha"), &m_AlphaValueIso,0.0,1.0);
	m_Gui->Button(ID_EXTRACT_ISO,_("Extract Iso"));

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
int albaView3D::GetNodeStatus(albaVME *vme)
//-------------------------------------------------------------------------
{
	albaSceneNode *n = NULL;
	if (m_Sg != NULL)
	{
    n = m_Sg->Vme2Node(vme);
		if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
		{
			if (n != NULL)
			{
				n->SetMutex(true);
			}
		}
    else if(vme->IsALBAType(albaVMEPolyline) || 
      vme->IsALBAType(albaVMESurface) ||
      vme->IsALBAType(albaVMESurfaceParametric))
    {
      if (n != NULL)
      {
      	n->SetMutex(false);
      }
    }
		else
		{
			if (n != NULL)
			{
				n->SetPipeCreatable(false);
			}
		}
	}

	return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
void albaView3D::EnableSubGui(int idSubPipe,bool enable)
//-------------------------------------------------------------------------
{
	switch(idSubPipe)
	{
		case ID_PIPE_ISO:
			{
				m_Gui->Enable(ID_ALPHA_VALUE_ISO,enable);
				m_Gui->Enable(ID_CONTOUR_VALUE_ISO,enable);
				m_Gui->Enable(ID_EXTRACT_ISO,enable);

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
				m_Gui->Enable(ID_EXTRACT_ISO,!enable);

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
				m_Gui->Enable(ID_EXTRACT_ISO,enable);

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
void albaView3D::InizializeSubGui()
//-------------------------------------------------------------------------
{
	switch(m_Choose)
	{
	case ID_PIPE_ISO:
		{
			albaPipeIsosurface *pipe=albaPipeIsosurface::SafeDownCast(this->GetNodePipe(m_CurrentVolume));
			if(pipe)
			{
				double sr[2];
				((albaVMEOutputVolume *)m_CurrentVolume->GetOutput())->GetVTKData()->GetScalarRange(sr);
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
void albaView3D::VmeShow(albaVME *vme,bool show)
//-------------------------------------------------------------------------
{
	Superclass::VmeShow(vme,show);

	if(vme->GetOutput()->IsA("albaVMEOutputVolume"))
	{
		if(show)
		{
			m_CurrentVolume = vme;
			InizializeSubGui();
			EnableSubGui(m_Choose);
			m_Gui->Enable(ID_COMBO_PIPE,m_CurrentVolume!=NULL);
			m_Gui->Enable(ID_RESAMPLE_FACTOR,m_CurrentVolume!=NULL);
		}
		else
		{
			m_CurrentVolume = NULL;
			EnableSubGui(ID_PIPE_ALL,false);
			m_Gui->Enable(ID_COMBO_PIPE,m_CurrentVolume!=NULL);
			m_Gui->Enable(ID_RESAMPLE_FACTOR,m_CurrentVolume!=NULL);
		}
	}
	if(vme->IsA("albaVMESurface"))
	{
		if(show)
		{
			m_CurrentSurface = albaVMESurface::SafeDownCast(vme);
		}
		else
		{
			m_CurrentSurface = NULL;
		}
	}
	else if((vme->IsALBAType(albaVMEPolyline)) && show)
	{
		((albaPipePolyline*)GetNodePipe(vme))->SetRepresentationToTube();
	}
	//CameraReset();
	CameraUpdate();
}