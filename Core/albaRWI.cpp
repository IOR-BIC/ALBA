/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRWI
 Authors: Silvano Imboden
 
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

#include "albaRWI.h"

#include "albaDecl.h"  // per CAMERA_POSITIONS
#include "albaEvent.h"
#include "albaAxes.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaSceneNode.h"
#include "albaSceneGraph.h"
#include "albaGUIPicButton.h"
#include "albaGUICrossIncremental.h"

#include "albaVME.h"
#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaAbsMatrixPipe.h"

#include "vtkALBAGridActor.h"  // users must see GRID_XYZ const declared in vtkALBAGridActor
#include "vtkLight.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkLinearTransform.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkMath.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkDataSet.h"
#include "vtkALBATextOrientator.h"
#include "vtkALBAProfilingActor.h"

#define DEFAULT_BG_COLOR 0.28

//----------------------------------------------------------------------------
albaRWI::albaRWI()
//----------------------------------------------------------------------------
{
	m_Listener= NULL;
	m_Sg      = NULL;
	m_RenBack = NULL;
	m_RenFront= NULL;
	m_AlwaysVisibleRenderer = NULL;
	m_Gui     = NULL;
	m_Light   = NULL;
	m_Camera  = NULL;
	m_RwiBase = NULL;
	m_RenderWindow = NULL;
	m_Orientator = NULL;
	m_Grid    = NULL;
	m_Axes    = NULL;

	m_ProfilingActor = NULL;
	for (int b=0; b<6; b++)
	{
		m_CameraButtons[b] = NULL;
	}
	m_Sizer = NULL;

	m_GridPosition = 0;
	m_BGColour  = wxColour(DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255);
	m_GridColour= wxColour(DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255);

	m_StereoMovieDir    = "";
	m_StereoMovieEnable = 0;
	m_ParallelView = 0;
	m_ShowProfilingInformation = 0;

	m_TopBottomAccumulation = m_TopBottomAccumulationLast = 0.0;
	m_LeftRigthAccumulation = m_LeftRigthAccumulationLast = 0.0;
	m_StepCameraOrientation = 10.0;

}
//----------------------------------------------------------------------------
albaRWI::albaRWI(wxWindow *parent, RWI_LAYERS layers, bool use_grid, bool show_axes, int stereo, bool show_orientator, int axesType)
//----------------------------------------------------------------------------
{
	m_Listener= NULL;
	m_Sg      = NULL;
	m_RenBack = NULL;
	m_RenFront= NULL;
	m_AlwaysVisibleRenderer = NULL;
	m_Gui     = NULL;
	m_Light   = NULL;
	m_Camera  = NULL;
	m_RwiBase = NULL;
	m_RenderWindow = NULL;
	m_Grid    = NULL;
	m_Axes    = NULL;
	m_AxesType = axesType;

	m_ProfilingActor = NULL;

	for (int b=0; b<6; b++)
	{
		m_CameraButtons[b] = NULL;
	}
	m_Sizer = NULL;

	m_GridPosition = 0;
	m_BGColour  = wxColour(DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255);
	m_GridColour= wxColour(DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255);

	m_StereoMovieDir    = "";
	m_StereoMovieEnable = 0;
	m_ShowProfilingInformation = 0;

	m_StepCameraOrientation = 10.0;
	m_TopBottomAccumulation = 0.0;
	m_LeftRigthAccumulation = 0.0;
	m_TopBottomAccumulation = m_TopBottomAccumulationLast = 0.0;
	m_LeftRigthAccumulation = m_LeftRigthAccumulationLast = 0.0;

	CreateRenderingScene(parent, layers, use_grid, show_axes, stereo, show_orientator, m_AxesType);

}
//----------------------------------------------------------------------------
void albaRWI::CreateRenderingScene(wxWindow *parent, RWI_LAYERS layers, bool use_grid, bool show_axes, int stereo, bool show_orientator, int axesType)
//----------------------------------------------------------------------------
{
	if (m_Light != NULL)
	{
		return;
	}

	m_AxesType = axesType;

	m_StereoType = stereo;

	m_Light = vtkLight::New();
	m_Light->SetLightTypeToCameraLight();

	m_Camera = vtkCamera::New();
	m_Camera->SetViewAngle(20); 
	m_Camera->ParallelProjectionOn(); 
	m_ParallelView = m_Camera->GetParallelProjection();
	UpdateCameraParameters();

	m_RenFront = vtkRenderer::New();
	m_RenFront->SetBackground(DEFAULT_BG_COLOR,DEFAULT_BG_COLOR,DEFAULT_BG_COLOR);
	m_RenFront->SetActiveCamera(m_Camera);
	m_RenFront->AddLight(m_Light);
	m_RenFront->BackingStoreOff();
	m_RenFront->LightFollowCameraOn(); 

	m_RwiBase = new albaRWIBase(parent, -1);
	m_RenderWindow = vtkRenderWindow::New();
	m_RenderWindow->GlobalWarningDisplayOff();

	if(stereo)
	{
		SetStereo(stereo);
	}

	// create gizmo layer
	m_AlwaysVisibleRenderer = vtkRenderer::New();
	m_AlwaysVisibleRenderer->SetBackground(DEFAULT_BG_COLOR,DEFAULT_BG_COLOR,DEFAULT_BG_COLOR);
	m_AlwaysVisibleRenderer->SetActiveCamera(m_Camera);
	m_AlwaysVisibleRenderer->AddLight(m_Light);
	m_AlwaysVisibleRenderer->BackingStoreOff();
	m_AlwaysVisibleRenderer->LightFollowCameraOn(); 
  
	if(layers == TWO_LAYER)
	{
		m_RenBack = vtkRenderer::New();
		m_RenBack->SetBackground(DEFAULT_BG_COLOR,DEFAULT_BG_COLOR,DEFAULT_BG_COLOR);
		m_RenBack->SetActiveCamera(m_Camera);
		m_RenBack->AddLight(m_Light);
		m_RenBack->BackingStoreOff();
		m_RenBack->LightFollowCameraOn(); 

		m_RenFront->SetLayer(1); 
		m_RenBack->SetLayer(2); 
		m_AlwaysVisibleRenderer->SetLayer(0);

    m_RenderWindow->SetNumberOfLayers(3);
    m_RenderWindow->AddRenderer(m_AlwaysVisibleRenderer);
    m_RenderWindow->AddRenderer(m_RenFront);
    m_RenderWindow->AddRenderer(m_RenBack);

	}
	else
	{
		m_RenFront->SetLayer(1); 
		assert(m_RenBack == NULL);
		m_AlwaysVisibleRenderer->SetLayer(0);

		m_RenderWindow->SetNumberOfLayers(2);
    m_RenderWindow->AddRenderer(m_AlwaysVisibleRenderer);
    m_RenderWindow->AddRenderer(m_RenFront);

	}

  m_RwiBase->SetRenderWindow(m_RenderWindow);
  m_RwiBase->Initialize();

  //assert(m_AlwaysVisibleRenderer->Transparent()	== true);

	m_ShowOrientator = show_orientator;

	vtkNEW(m_Orientator);
	m_Orientator->SetTextUp("U");
	m_Orientator->SetTextDown("D");
	m_Orientator->SetTextLeft("L");
	m_Orientator->SetTextRight("R");

	m_RenFront->AddActor2D(m_Orientator);
	m_Orientator->SetVisibility(m_ShowOrientator);
	//m_Orientator->SetBackgroundVisibility(false);
	m_Orientator->SetTextColor(1.0,1.0,1.0);
	m_Orientator->SetBackgroundColor(0.0,0.0,0.0);

	vtkNEW(m_ProfilingActor);
	m_RenFront->AddActor2D(m_ProfilingActor);
	m_ProfilingActor->SetVisibility(m_ShowProfilingInformation);


	m_ShowGrid    = use_grid;
	m_GridNormal  = GRID_Z;

	m_Grid = vtkALBAGridActor::New();
	m_RenFront->AddActor(m_Grid);
	m_RenFront->AddActor2D(m_Grid->GetLabelActor());
	SetGridNormal(m_GridNormal);
	SetGridVisibility(m_ShowGrid != 0);

	m_ShowAxes = show_axes;
	m_Axes = new albaAxes(m_AlwaysVisibleRenderer,NULL,m_AxesType);
	m_Axes->SetVisibility(show_axes);
}
//----------------------------------------------------------------------------
albaRWI::~albaRWI()
//----------------------------------------------------------------------------
{
	if(m_Grid) m_RenFront->RemoveActor(m_Grid);
	if(m_Grid) m_RenFront->RemoveActor2D(m_Grid->GetLabelActor());
	vtkDEL(m_Grid);

	if(m_Orientator) m_RenFront->RemoveActor2D(m_Orientator);
	vtkDEL(m_Orientator);

	cppDEL(m_Axes); //Must be removed before deleting renderers
	vtkDEL(m_Light);
	vtkDEL(m_Camera);


	if(m_RenFront) 
	{
		m_RenFront->RemoveAllViewProps();
		m_RenderWindow->RemoveRenderer(m_RenFront);
	}
	vtkDEL(m_ProfilingActor);

	vtkDEL(m_RenFront);

	if(m_RenBack)
	{
		m_RenBack->RemoveAllViewProps();
		m_RenderWindow->RemoveRenderer(m_RenBack);
	}

	vtkDEL(m_RenBack);

	if(m_AlwaysVisibleRenderer)
	{
		m_AlwaysVisibleRenderer->RemoveAllViewProps();
		m_RenderWindow->RemoveRenderer(m_AlwaysVisibleRenderer);
	}

	vtkDEL(m_AlwaysVisibleRenderer);


	if(m_RenderWindow) 
		m_RenderWindow->SetInteractor(NULL);
	m_RenderWindow->Delete();
	if(m_RwiBase) 
		m_RwiBase->SetRenderWindow(NULL);
	vtkDEL(m_RwiBase);  //The renderer has to be Deleted as last
}
//-----------------------------------------------------------------------------------------
void albaRWI::CameraSet(int cam_position, double zoom)
//-----------------------------------------------------------------------------------------
{
	int x,y,z,vx,vy,vz;
	m_CameraPositionId = cam_position;

	if(cam_position == CAMERA_PERSPECTIVE
		|| cam_position == CAMERA_PERSPECTIVE_FRONT
		|| cam_position == CAMERA_PERSPECTIVE_BACK
		|| cam_position == CAMERA_PERSPECTIVE_LEFT
		|| cam_position == CAMERA_PERSPECTIVE_RIGHT 
		|| cam_position == CAMERA_PERSPECTIVE_TOP 
		|| cam_position == CAMERA_PERSPECTIVE_BOTTOM 
		)
		m_Camera->ParallelProjectionOff();
	else
		m_Camera->ParallelProjectionOn();

	m_ParallelView = m_Camera->GetParallelProjection();

	switch (cam_position) 
	{
	case CAMERA_FRONT:
	case CAMERA_PERSPECTIVE_FRONT:
		x = 0; y = -1; z = 0; vx = 0; vy = 0; vz = 1;
		break;
	case CAMERA_BACK:
	case CAMERA_PERSPECTIVE_BACK:
		x = 0; y = 1; z = 0; vx = 0; vy = 0; vz = 1;
		break;
	case CAMERA_LEFT:
	case CAMERA_PERSPECTIVE_LEFT:
		x = -1; y = 0; z = 0; vx = 0; vy = 0; vz = 1;
		break;
	case CAMERA_RIGHT:
	case CAMERA_PERSPECTIVE_RIGHT:
		x = 1; y = 0; z = 0; vx = 0; vy = 0; vz = 1;
		break;
	case CAMERA_TOP:
	case CAMERA_PERSPECTIVE_TOP:
		x = 0; y = 0; z = 1; vx = 0; vy = -1; vz = 0;
		break;
	case CAMERA_BOTTOM:
	case CAMERA_PERSPECTIVE_BOTTOM:
		x = 0; y = 0; z = -1; vx = 0; vy = -1; vz = 0;
		break;
	case CAMERA_PERSPECTIVE:
		x = -1; y = -1; z = 1; vx = 0; vy = 0; vz = 1;
		break;
	case CAMERA_RX_FRONT:
		x=0; y=-1; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_RX_LEFT:
		x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_RX_RIGHT:
		x=-1;y=0; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_DRR_FRONT:
		m_Camera->ParallelProjectionOff();
		x=0; y=-1; z=0; vx=0; vy=0; vz=1;	//modified by Paolo 10-6-2003
		break;
	case CAMERA_DRR_LEFT:
		m_Camera->ParallelProjectionOff();
		x=1 ;y=0; z=0; vx=0; vy=0; vz=1;	//modified by Paolo 10-6-2003
		break;
	case CAMERA_DRR_RIGHT:
		m_Camera->ParallelProjectionOff();
		x=-1;y=0; z=0; vx=0; vy=0; vz=1;	//modified by Paolo 10-6-2003
		break;
		//    case CAMERA_CT:
		//x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
		//    break;
	case CAMERA_OS_X:
		//x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
		x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_OS_Y:
		x=0; y=-1; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_CT:
	case CAMERA_OS_Z:
		//x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
		x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
		break;
	case CAMERA_OS_P:
		m_Camera->ParallelProjectionOff();     // Paolo 09/06/2004
		x=-1; y=-1; z=1; vx=0; vy=0; vz=1;
	case CAMERA_ARB:
		m_Camera->ParallelProjectionOff();     // Daniele 09/05/2007
		x=-1; y=-1; z=1; vx=0; vy=0; vz=1;
		break;
		// ste beg
	case CAMERA_RXFEM_XNEG:
		x=-1;y=0; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_RXFEM_XPOS:
		x=1 ;y=0; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_RXFEM_YNEG:
		x=0; y=-1;z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_RXFEM_YPOS:
		x=0; y=1; z=0; vx=0; vy=0; vz=1;
		break;
	case CAMERA_RXFEM_ZNEG:
		x=0; y=0; z=-1;vx=1; vy=0; vz=0;
		break;
	case CAMERA_RXFEM_ZPOS:
		x=0; y=0; z=1; vx=1; vy=0; vz=0;
		break;
	case CAMERA_BLEND:
		x=0; y=0; z=1; vx=0; vy=-1; vz=0;
		break;
		// ste end
	}
	m_Camera->SetFocalPoint(0,0,0);
	m_Camera->SetPosition(x*100,y*100,z*100);
	m_Camera->SetViewUp(vx,vy,vz);
	m_Camera->SetClippingRange(0.1,1000);

	CameraReset((albaVME*)NULL, zoom);
}

//----------------------------------------------------------------------------
void albaRWI::CameraSet( double pos[3],double viewUp[3], double zoom /*= 1.*/, bool parallelProjection /*= false*/ )
//----------------------------------------------------------------------------
{
	if (parallelProjection)
		m_Camera->ParallelProjectionOn();
	else
		m_Camera->ParallelProjectionOff();

	m_ParallelView = m_Camera->GetParallelProjection();

	m_Camera->SetFocalPoint(0,0,0);
	m_Camera->SetPosition(pos[0]*100,pos[1]*100,pos[2]*100);
	m_Camera->SetViewUp(viewUp[0],viewUp[1],viewUp[2]);
	m_Camera->SetClippingRange(0.1,1000);

	CameraReset((albaVME*)NULL, zoom);
}
//----------------------------------------------------------------------------
void albaRWI::SetSize(int x, int y, int w,int h)
//----------------------------------------------------------------------------
{
	((wxWindow*)m_RwiBase)->SetSize(x,y,w,h);
	((wxWindow*)m_RwiBase)->SetMinSize(wxSize(w,h));
}
//----------------------------------------------------------------------------
void albaRWI::Show(bool show)
//----------------------------------------------------------------------------
{
	m_RwiBase->Show(show);
}
//----------------------------------------------------------------------------
void albaRWI::SetGridNormal(int normal_id)
//----------------------------------------------------------------------------
{
	if(m_Grid)
	{
		m_GridNormal = normal_id;
		m_Grid->SetGridNormal(normal_id);
		if (m_Gui != NULL)
		{
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetGridPosition(double position)
//----------------------------------------------------------------------------
{
	if(m_Grid)
	{
		m_GridPosition = position;
		m_Grid->SetGridPosition(position);
		if (m_Gui != NULL)
		{
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetGridVisibility(bool show)
//----------------------------------------------------------------------------
{
	if(m_Grid)
	{
		m_ShowGrid = show;
		m_Grid->SetVisibility(m_ShowGrid);
		m_Grid->GetLabelActor()->SetVisibility(m_ShowGrid);
		m_RenFront->ResetCameraClippingRange();
		if (m_Gui != NULL)
		{
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetAxesVisibility(bool show)
//----------------------------------------------------------------------------
{
	if(m_Axes) 
	{
		m_ShowAxes = show;
		m_Axes->SetVisibility(show);
		if (m_Gui != NULL)
		{
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetOrientatorVisibility(bool show)
//----------------------------------------------------------------------------
{
	if(m_Orientator)
	{
		m_ShowOrientator = show;
		m_Orientator->SetVisibility(show);
		if (m_Gui != NULL)
		{
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetProfilingActorVisibility(bool show)
//----------------------------------------------------------------------------
{
	m_ProfilingActor->SetVisibility(show);
}
//----------------------------------------------------------------------------
void albaRWI::SetGridColor(const wxColor &col)
//----------------------------------------------------------------------------
{
	if(m_Grid)
	{
		m_GridColour = col;
		m_Grid->SetGridColor(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
		if (m_Gui != NULL) 
		{
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetBackgroundColor(const wxColor &col)
//----------------------------------------------------------------------------
{
	m_BGColour = col;
	if (m_Gui != NULL) 
	{
		m_Gui->Update();
	}
	m_RenFront->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
	m_AlwaysVisibleRenderer->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
  if(m_RenBack) 
		m_RenBack->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);

  
}
//----------------------------------------------------------------------------
void albaRWI::SetStereo(int stereo_type)
//----------------------------------------------------------------------------
{
	if(m_RenBack) 
	{
		albaLogMessage("WARNING: SetStereo is disabled for RWI with two layers");
		return;
	}

	if(stereo_type < VTK_STEREO_CRYSTAL_EYES)
		stereo_type = 0; 

	if(m_StereoType == stereo_type) 
		return;

	m_StereoType = stereo_type;
	if (m_Gui != NULL) 
	{
		m_Gui->Update();
	}

	m_RenderWindow->SetStereoCapableWindow(m_StereoType != 0);
	m_RenderWindow->SetStereoRender(m_StereoType != 0);
	m_RenderWindow->SetStereoType(m_StereoType);
	m_RwiBase->ReInitialize();
}
//----------------------------------------------------------------------------
void albaRWI::CameraUpdate()
//----------------------------------------------------------------------------
{
	//if(!m_RwiBase->IsShown()) return;
	if (m_RenderWindow->GetGenericWindowId() == 0) 
		return;

  // 23.12.2010: Added patch by Simone Brazzale
  // The Clipping range is now calculated considering all layers
  this->ResetCameraClippingRange(); 

	m_RenderWindow->Render();
	if (m_StereoMovieEnable!=0)
	{
		m_RwiBase->GenerateStereoFrames();
	}
	UpdateCameraParameters();
}
//----------------------------------------------------------------------------
void albaRWI::ResetCameraClippingRange()
//----------------------------------------------------------------------------
{
	vtkRenderer *rAV = m_AlwaysVisibleRenderer; 	 
	vtkRenderer *rFR = m_RenFront;	 
  vtkRenderer *rBR = m_RenBack;

	double b1[6],b2[6],b3[6],b[6];
  if(rFR==NULL)
	{
	} 
	else if (rAV==NULL && rBR==NULL)
	{
     rFR->ResetCameraClippingRange(); 
	}
  else if (rAV)
  {
    // We have the tird layer (always visible), with default bounds (-1,1,-1,1,-1,1).
    // The clipping range must be computed with the bounds of the front renderer!
		rFR->ComputeVisiblePropBounds(b1);
		rAV->ComputeVisiblePropBounds(b2);

		if(b1[0] == VTK_FLOAT_MAX && b2[0] == VTK_FLOAT_MAX)
		{
			rFR->ResetCameraClippingRange();
		} 
		else if (b1[0] == VTK_FLOAT_MAX )
		{
			rFR->ResetCameraClippingRange(b2);
		}
		else
		{
      // WORKAROUND 
      // The only actor shown is the Axis actor in the bottom left angle:
      // it must not be taken into account.
      if (b2[0]==-1 && b2[1]==1 && b2[2]==-1 && b2[3]==1 && b2[4]==-1 && b2[5]==1)
      {
        b[0] = b1[0];
			  b[2] = b1[2];
        b[4] = b1[4];
        b[1] = b1[1];
			  b[3] = b1[3];
			  b[5] = b1[5];
      }
      // WORKAROUND 
      // There are other actors (like the GIZMO): 
      // we must take them into account.
      else
      {
        b[0] = (b1[0]<b2[0]) ?	b1[0] : b2[0];    
			  b[2] = (b1[2]<b2[2]) ?	b1[2] : b2[2];    
			  b[4] = (b1[4]<b2[4]) ?	b1[4] : b2[4];    
			  b[1] = (b1[1]>b2[1]) ?	b1[1] : b2[1];    
			  b[3] = (b1[3]>b2[3]) ?	b1[3] : b2[3];    
			  b[5] = (b1[5]>b2[5]) ?	b1[5] : b2[5];  
      }
			rFR->ResetCameraClippingRange(b);
		}

    // We have also the back renderer.
    // The clipping range must be matched between the back renderer bounds and the already calculated one.	
    if (rBR)
  	{
	  	rBR->ComputeVisiblePropBounds(b3);

		  if (b3[0] == VTK_FLOAT_MAX )
		  {
        // do nothing
			}
		  else
		  {
			  b[0] = (b[0]<b3[0]) ?	b[0] : b3[0];    
			  b[2] = (b[2]<b3[2]) ?	b[2] : b3[2];    
			  b[4] = (b[4]<b3[4]) ?	b[4] : b3[4];    
			  b[1] = (b[1]>b3[1]) ?	b[1] : b3[1];    
			  b[3] = (b[3]>b3[3]) ?	b[3] : b3[3];    
			  b[5] = (b[5]>b3[5]) ?	b[5] : b3[5];    
			  rFR->ResetCameraClippingRange(b);
		  }
	  }
  }
}
//----------------------------------------------------------------------------
void albaRWI::UpdateCameraParameters()
//----------------------------------------------------------------------------
{
	m_Camera->GetPosition(m_CameraPosition);
	m_Camera->GetFocalPoint(m_FocalPoint);
	//m_Camera->GetViewUp(m_CameraViewUp);
	double *ori = m_Camera->GetOrientation();
	/*m_CameraOrientation[0] = ori[0];
	m_CameraOrientation[1] = ori[1];
	m_CameraOrientation[2] = ori[2];*/
	if (m_Gui)
	{
		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void albaRWI::CameraReset(albaVME *vme, double zoom)
//----------------------------------------------------------------------------
{
	if (m_RenderWindow->GetGenericWindowId() == 0) 
		return;

	if(m_Grid && m_ShowGrid) 
		m_Grid->VisibilityOff();

	albaEventMacro(albaEvent(this,CAMERA_PRE_RESET,m_RenFront)); //- Attention - I'm sending m_RenFront, I suppose that m_RenBack is never required 
	CameraReset(ComputeVisibleBounds(vme), zoom);
	if(m_Grid && m_ShowGrid) 
		m_Grid->VisibilityOn();

	albaEventMacro(albaEvent(this,CAMERA_POST_RESET,m_RenFront));
	m_RenFront->ResetCameraClippingRange();
	m_RenderWindow->Render();
	UpdateCameraParameters();
}
//----------------------------------------------------------------------------
double *albaRWI::ComputeVisibleBounds(albaVME *vme)
//----------------------------------------------------------------------------
{
	static double b[6],b1[6],b2[6]; // static so it is possible to return it
	albaSceneNode *n;

	if (vme && !vme->IsA("albaVMERoot") && vme->GetOutput()->GetVTKData() && m_Sg && (n = m_Sg->Vme2Node(vme)) && n->IsVisible())
	{
		vme->GetOutput()->GetVMEBounds(b1);
		return b1;
	}
	m_RenFront->ComputeVisiblePropBounds(b1);
	if (m_RenBack)
	{
		m_RenBack->ComputeVisiblePropBounds(b2);
		b[0] = (b2[0] < b1[0]) ? b2[0] : b1[0];
		b[2] = (b2[2] < b1[2]) ? b2[2] : b1[2];
		b[4] = (b2[4] < b1[4]) ? b2[4] : b1[4];
		b[1] = (b2[1] > b1[1]) ? b2[1] : b1[1];
		b[3] = (b2[3] > b1[3]) ? b2[3] : b1[3];
		b[5] = (b2[5] > b1[5]) ? b2[5] : b1[5];
		return b;
	}
	else
		return b1;
}
//----------------------------------------------------------------------------
void albaRWI::CameraReset(double bounds[6], double zoom)
//----------------------------------------------------------------------------
{
	double view_up[3], view_look[3], view_right[3];
	if(zoom <= 0) zoom = 1;
	m_Camera->OrthogonalizeViewUp();
	m_Camera->ComputeViewPlaneNormal();
	m_Camera->GetViewUp(view_up);
	vtkMath::Normalize(view_up);
	m_Camera->GetViewPlaneNormal(view_look);
	vtkMath::Cross(view_up, view_look, view_right);
	vtkMath::Normalize(view_right);

	//convert camera vectors to float
	double vu[3],vl[3],vr[3];
	for(int i=0; i<3; i++)
	{
		vu[i]=view_up[i];
		vl[i]=view_look[i];
		vr[i]=view_right[i];
	} 	

	double height;	
	double width;	
	double depth;	
	double center[3];
	center[0] = (bounds[0] + bounds[1])/2.0;
	center[1] = (bounds[2] + bounds[3])/2.0;
	center[2] = (bounds[4] + bounds[5])/2.0;

	double diag[3];
	diag[0] = (bounds[1] - bounds[0]);
	diag[1] = (bounds[3] - bounds[2]);
	diag[2] = (bounds[5] - bounds[4]);

	//--------------------------------------
	if(m_Camera->GetParallelProjection())
	{
		height = fabs( 0.52 * vtkMath::Dot(vu,diag) );
		width  = fabs( 0.52 * vtkMath::Dot(vr,diag) );
		depth  = fabs( 0.52 * vtkMath::Dot(vl,diag) );
	}
	//--------------------------------------
	else 
	{
		height = width = depth = 0.5 * vtkMath::Norm(diag);	
	}
	//--------------------------------------

	height = (height == 0) ? 1.0 : height;	

	//check aspect ratio - and eventually compensate height
	double view_aspect  = (m_RwiBase->m_Width*1.0)/(m_RwiBase->m_Height*1.0);
	double scene_aspect = (width)/(height);
	if( scene_aspect > view_aspect )
	{
		height = width/view_aspect; 
	}

	double distance;
	distance  = height/tan(m_Camera->GetViewAngle()*vtkMath::Pi()/360.0);
	distance += depth/2;

	// update the camera
	m_Camera->SetFocalPoint(center[0],center[1],center[2]);
	m_Camera->SetPosition(center[0]+(1/zoom) * distance*vl[0],
		center[1]+(1/zoom) * distance*vl[1],
		center[2]+(1/zoom) * distance*vl[2]);

	// setup parallel scale
	m_Camera->SetParallelScale(height);

	//m_RenFront->ResetCameraClippingRange(bounds);
}
//-------------------------------------------------------------------------
/** IDs for the GUI */
//-------------------------------------------------------------------------
enum RWI_WIDGET_ID
{
	ID_SHOW_GRID = MINID,
	ID_GRID_NORMAL,
	ID_GRID_POS,
	ID_GRID_COLOR,
	ID_SHOW_AXES,
	ID_BG_COLOR,
	ID_STERO_MOVIE_DIR,
	ID_STERO_MOVIE,
	ID_CAMERA_FRONT,
	ID_CAMERA_BACK,
	ID_CAMERA_LEFT,
	ID_CAMERA_RIGHT,
	ID_CAMERA_TOP,
	ID_CAMERA_BOTTOM,
	ID_FOCAL_POINT,
	ID_CAMERA_ORTHO,
	ID_CAMERA_POSITION,
	ID_CAMERA_VIEW_UP,
	ID_CAMERA_ORIENTATION,
	ID_SHOW_ORIENTATOR,
	ID_SHOW_PROFILING_INFORMATION,
};
//-------------------------------------------------------------------------
albaGUI *albaRWI::CreateGui()
//-------------------------------------------------------------------------
{
	wxString grid_normal[3] = {"X axes","Y axes","Z axes"};

	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
	// constant used in albaRWIxxx to initialize Camera pos
	if (m_Camera->GetParallelProjection() == 0)
	{
		m_Sizer =  new wxBoxSizer( wxHORIZONTAL );
		m_CameraButtons[0] = new albaGUIPicButton(m_Gui, "PIC_BOTTOM",ID_CAMERA_BOTTOM, this);
		m_CameraButtons[1] = new albaGUIPicButton(m_Gui, "PIC_TOP",   ID_CAMERA_TOP,    this);
		m_CameraButtons[2] = new albaGUIPicButton(m_Gui, "PIC_BACK",  ID_CAMERA_BACK,   this);
		m_CameraButtons[3] = new albaGUIPicButton(m_Gui, "PIC_FRONT", ID_CAMERA_FRONT,  this);
		m_CameraButtons[4] = new albaGUIPicButton(m_Gui, "PIC_LEFT",  ID_CAMERA_LEFT,   this);
		m_CameraButtons[5] = new albaGUIPicButton(m_Gui, "PIC_RIGHT", ID_CAMERA_RIGHT,  this);

		m_CameraButtons[0]->SetToolTip("Bottom");
		m_CameraButtons[1]->SetToolTip("Top");
		m_CameraButtons[2]->SetToolTip("Back");
		m_CameraButtons[3]->SetToolTip("Front");
		m_CameraButtons[4]->SetToolTip("Left");
		m_CameraButtons[5]->SetToolTip("Right");
		
		for (int i = 0; i < 6; i++)
		{
			m_Sizer->Add(m_CameraButtons[i], 0, 0);		
		}		

		m_Gui->Add(m_Sizer);
		m_Gui->Bool(ID_CAMERA_ORTHO, "Orthographic Projection", &m_ParallelView, 1);
		m_Gui->Divider(1);

		m_Gui->Label("Camera parameters", true);
		m_Gui->Vector(ID_FOCAL_POINT, "Focal pnt.", m_FocalPoint);
		m_Gui->Vector(ID_CAMERA_POSITION, "Position", m_CameraPosition);
		//m_Gui->Vector(ID_CAMERA_VIEW_UP, "view up", m_CameraViewUp);
		//m_Gui->Vector(ID_CAMERA_ORIENTATION, "orientation", m_CameraOrientation);
		albaGUICrossIncremental *ci = m_Gui->CrossIncremental(ID_CAMERA_ORIENTATION,"Orientation", &m_StepCameraOrientation, &m_TopBottomAccumulation, &m_LeftRigthAccumulation,albaGUICrossIncremental::ID_COMPLETE_LAYOUT,"Change the rotation step value from the entry.",false);
		ci->EnableStep(true);
		m_Gui->Divider(2);
	}

	m_Gui->Bool(ID_SHOW_GRID,"Grid",&m_ShowGrid,0,"Turn On/Off the grid");
	m_Gui->Combo(ID_GRID_NORMAL,"Grid norm",&m_GridNormal,3,grid_normal,"orientation axes for the grid");
	m_Gui->Double(ID_GRID_POS,"Grid pos",	&m_GridPosition);
	m_Gui->Color(ID_GRID_COLOR,"Grid color",&m_GridColour);
	m_Gui->Divider(2);
	m_Gui->Bool(ID_SHOW_AXES,"Show axes",&m_ShowAxes,0);
	m_Gui->Color(ID_BG_COLOR,"Back color",&m_BGColour);
		
	if (m_StereoType)
	{
		m_Gui->Divider(2);
		m_Gui->Label("Stereo movie");
		m_Gui->DirOpen(ID_STERO_MOVIE_DIR,"Dir",&m_StereoMovieDir);
		m_Gui->Bool(ID_STERO_MOVIE,"Start rec",&m_StereoMovieEnable);
		m_Gui->Enable(ID_STERO_MOVIE_DIR,m_StereoType != 0);
		m_Gui->Enable(ID_STERO_MOVIE,false);
	}

	m_Gui->Divider(2);
	m_Gui->Bool(ID_SHOW_ORIENTATOR,"Orientation",&m_ShowOrientator);
	m_Gui->Bool(ID_SHOW_PROFILING_INFORMATION,"fps",&m_ShowProfilingInformation);
	m_Gui->Divider();
	return m_Gui;
}
//-------------------------------------------------------------------------
albaGUI *albaRWI::GetGui()
//-------------------------------------------------------------------------
{
	if(m_Gui == NULL)
		return CreateGui();
	else
		return m_Gui;
}
//----------------------------------------------------------------------------
void albaRWI::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId()) 
		{
		case ID_SHOW_GRID:
			SetGridVisibility(m_ShowGrid != 0);
			CameraUpdate();
			break;
		case ID_GRID_NORMAL:
			SetGridNormal(m_GridNormal);
			CameraUpdate();
			break;
		case ID_SHOW_ORIENTATOR:
			SetOrientatorVisibility(m_ShowOrientator!= 0);
			CameraUpdate();
			break;
		case ID_SHOW_PROFILING_INFORMATION:
			{  
				SetProfilingActorVisibility(m_ShowProfilingInformation!=0);
				CameraUpdate();
			}
			break;
		case ID_SHOW_AXES:
			SetAxesVisibility(m_ShowAxes != 0); 
			CameraUpdate();
			break;
		case ID_GRID_POS:
			SetGridPosition(m_GridPosition);
			CameraUpdate();
			break;
		case ID_GRID_COLOR:
			SetGridColor(m_GridColour);
			CameraUpdate();
			break;
		case ID_BG_COLOR:
			SetBackgroundColor(m_BGColour);
			CameraUpdate();
			break;
		case ID_STERO_MOVIE_DIR:
			m_Gui->Enable(ID_STERO_MOVIE,!m_StereoMovieDir.IsEmpty());
			m_RwiBase->SetStereoMovieDirectory(m_StereoMovieDir.GetCStr());
			break;
		case ID_STERO_MOVIE:
			m_RwiBase->EnableStereoMovie(m_StereoMovieEnable != 0);
			break;
		case ID_CAMERA_FRONT:
			CameraSet(CAMERA_PERSPECTIVE_FRONT);
			break;
		case ID_CAMERA_BACK:
			CameraSet(CAMERA_PERSPECTIVE_BACK);
			break;
		case ID_CAMERA_LEFT:
			CameraSet(CAMERA_PERSPECTIVE_LEFT);
			break;
		case ID_CAMERA_RIGHT:
			CameraSet(CAMERA_PERSPECTIVE_RIGHT);
			break;
		case ID_CAMERA_TOP:
			CameraSet(CAMERA_PERSPECTIVE_TOP);
			break;
		case ID_CAMERA_BOTTOM:
			CameraSet(CAMERA_PERSPECTIVE_BOTTOM);
			break;
		case ID_CAMERA_ORTHO: 
		{
			m_Camera->SetParallelProjection(m_ParallelView);
 			CameraUpdate();
		}
			break;
		case ID_FOCAL_POINT:
		case ID_CAMERA_POSITION:
			m_Camera->SetPosition(m_CameraPosition);
			m_Camera->SetFocalPoint(m_FocalPoint);
			//m_Camera->SetViewUp(m_CameraViewUp);
			CameraUpdate();
			break;
		case ID_CAMERA_ORIENTATION:
			{
				int dx = m_LeftRigthAccumulation - m_LeftRigthAccumulationLast;
				int dy = m_TopBottomAccumulation - m_TopBottomAccumulationLast;

				int *size = m_RenderWindow->GetSize();

				double delta_elevation = -20.0 / size[1];
				double delta_azimuth = -20.0 / size[0];

				double rxf = (double)dx * delta_azimuth * this->m_StepCameraOrientation;
				double ryf = (double)dy * delta_elevation * this->m_StepCameraOrientation;

				m_Camera->Azimuth(rxf);
				m_Camera->Elevation(ryf);
				m_Camera->OrthogonalizeViewUp();

				CameraUpdate();
				m_LeftRigthAccumulationLast = m_LeftRigthAccumulation;
				m_TopBottomAccumulationLast = m_TopBottomAccumulation;
			}
			break;
		default:
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
void albaRWI::SetOrientatorProperties(double rgbText[3], double rgbBackground[3], double scale)
//----------------------------------------------------------------------------
{
	if(m_Orientator)
	{
		m_Orientator->SetScale(scale);
		m_Orientator->SetTextColor(rgbText[0], rgbText[1],rgbText[2]);
		m_Orientator->SetBackgroundColor(rgbBackground[0], rgbBackground[1],rgbBackground[2]);
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetOrientatorSingleActorVisibility(bool showLeft, bool showDown, bool showRight, bool showUp)
//----------------------------------------------------------------------------
{
	if(m_Orientator)
	{
		m_Orientator->SetSingleActorVisibility(vtkALBATextOrientator::ID_ACTOR_LEFT, showLeft);
		m_Orientator->SetSingleActorVisibility(vtkALBATextOrientator::ID_ACTOR_DOWN, showDown);
		m_Orientator->SetSingleActorVisibility(vtkALBATextOrientator::ID_ACTOR_RIGHT, showRight);
		m_Orientator->SetSingleActorVisibility(vtkALBATextOrientator::ID_ACTOR_UP, showUp);
	}
}
//----------------------------------------------------------------------------
void albaRWI::SetOrientatorSingleActorText(const char* textLeft, const char* textDown, const char* textRight, const char* textUp)
//----------------------------------------------------------------------------
{
	if(m_Orientator)
	{
		m_Orientator->SetTextLeft(textLeft);
		m_Orientator->SetTextDown(textDown);
		m_Orientator->SetTextRight(textRight);
		m_Orientator->SetTextUp(textUp);
	}
}
