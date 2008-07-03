/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWI.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:30:29 $
  Version:   $Revision: 1.47 $
  Authors:   Silvano Imboden
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

#include "mafRWI.h"

#include "mafDecl.h"  // per CAMERA_POSITIONS
#include "mafEvent.h"
#include "mafAxes.h"
#include "mmgGui.h"
#include "mafNode.h"
#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mmgPicButton.h"
#include "mmgMeasureUnitSettings.h"
#include "mafGUICrossIncremental.h"

#include "mafVME.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafAbsMatrixPipe.h"

#include "vtkMAFGridActor.h"  // users must see GRID_XYZ const declared in vtkMAFGridActor
#include "vtkLight.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkLinearTransform.h"
#include "vtkRenderWindow.h"
#include "vtkMath.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkDataSet.h"
#include "vtkMAFSimpleRulerActor2D.h"
#include "vtkMAFTextOrientator.h"

#define DEFAULT_BG_COLOR 0.28

//----------------------------------------------------------------------------
mafRWI::mafRWI()
//----------------------------------------------------------------------------
{
  m_Listener= NULL;
  m_Sg      = NULL;
  m_RenBack = NULL;
  m_RenFront= NULL;
  m_Gui     = NULL;
  m_Light   = NULL;
  m_Camera  = NULL;
  m_RwiBase = NULL;
  m_RenderWindow = NULL;
  m_Ruler   = NULL;
	m_Orientator = NULL;
  m_Grid    = NULL;
  m_Axes    = NULL;
  for (int b=0; b<6; b++)
  {
    m_CameraButtons[b] = NULL;
  }
  m_Sizer = NULL;

  m_LinkCamera   = 0;
  m_GridPosition = 0;
  m_BGColour  = wxColour(DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255);
  m_GridColour= wxColour(DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255);

  m_StereoMovieDir    = "";
  m_StereoMovieEnable = 0;

  m_TopBottomAccumulation = m_TopBottomAccumulationLast = 0.0;
  m_LeftRigthAccumulation = m_LeftRigthAccumulationLast = 0.0;
  m_StepCameraOrientation = 10.0;
}
//----------------------------------------------------------------------------
mafRWI::mafRWI(wxWindow *parent, RWI_LAYERS layers, bool use_grid, bool show_axes, bool show_ruler, int stereo, bool show_orientator)
//----------------------------------------------------------------------------
{
  m_Listener= NULL;
  m_Sg      = NULL;
  m_RenBack = NULL;
  m_RenFront= NULL;
  m_Gui     = NULL;
  m_Light   = NULL;
  m_Camera  = NULL;
  m_RwiBase = NULL;
  m_RenderWindow = NULL;
  m_Ruler   = NULL;
  m_Grid    = NULL;
  m_Axes    = NULL;
  for (int b=0; b<6; b++)
  {
    m_CameraButtons[b] = NULL;
  }
  m_Sizer = NULL;

  m_LinkCamera   = 0;
  m_GridPosition = 0;
  m_BGColour  = wxColour(DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255,DEFAULT_BG_COLOR * 255);
  m_GridColour= wxColour(DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255,DEFAULT_GRID_COLOR * 255);

  m_StereoMovieDir    = "";
  m_StereoMovieEnable = 0;

  m_StepCameraOrientation = 10.0;
  m_TopBottomAccumulation = 0.0;
  m_LeftRigthAccumulation = 0.0;
  m_TopBottomAccumulation = m_TopBottomAccumulationLast = 0.0;
  m_LeftRigthAccumulation = m_LeftRigthAccumulationLast = 0.0;

  CreateRenderingScene(parent, layers, use_grid, show_axes, show_ruler, stereo, show_orientator);
}
//----------------------------------------------------------------------------
void mafRWI::CreateRenderingScene(wxWindow *parent, RWI_LAYERS layers, bool use_grid, bool show_axes, bool show_ruler, int stereo, bool show_orientator)
//----------------------------------------------------------------------------
{
  if (m_Light != NULL)
  {
    return;
  }

  m_StereoType = stereo;

  m_Light = vtkLight::New();
  m_Light->SetLightTypeToCameraLight();

  m_Camera = vtkCamera::New();
	m_Camera->SetViewAngle(20); 
  m_Camera->ParallelProjectionOn(); 
  UpdateCameraParameters();

	m_RenFront = vtkRenderer::New();
  m_RenFront->SetBackground(DEFAULT_BG_COLOR,DEFAULT_BG_COLOR,DEFAULT_BG_COLOR);
  m_RenFront->SetActiveCamera(m_Camera);
  m_RenFront->AddLight(m_Light);
  m_RenFront->BackingStoreOff();
  m_RenFront->LightFollowCameraOn(); 
	
  m_RwiBase = new mafRWIBase(parent, -1);
  m_RenderWindow = vtkRenderWindow::New();
  m_RenderWindow->GlobalWarningDisplayOff();
  if(stereo)
  {
    SetStereo(stereo);
  }
  m_RenderWindow->AddRenderer(m_RenFront);

  m_RwiBase->SetRenderWindow(m_RenderWindow);
//#ifdef WIN32
  m_RwiBase->Initialize();
//#endif
  
  if(layers == TWO_LAYER)
  {
    m_RenBack = vtkRenderer::New();
    m_RenBack->SetBackground(DEFAULT_BG_COLOR,DEFAULT_BG_COLOR,DEFAULT_BG_COLOR);
    m_RenBack->SetActiveCamera(m_Camera);
    m_RenBack->AddLight(m_Light);
    m_RenBack->BackingStoreOff();
    m_RenBack->LightFollowCameraOn(); 
    m_RenBack->SetInteractive(0); 

    m_RenFront->SetLayer(0); 
    m_RenBack->SetLayer(1); 
    m_RenderWindow->SetNumberOfLayers(2);
    m_RenderWindow->AddRenderer(m_RenBack);
  }

  m_ShowRuler = show_ruler;
	m_ShowOrientator = show_orientator;

  mmgMeasureUnitSettings *unit_settings = new mmgMeasureUnitSettings(this);
  m_RulerScaleFactor = unit_settings->GetScaleFactor();
  m_RulerLegend = unit_settings->GetUnitName();
  cppDEL(unit_settings);
  vtkNEW(m_Ruler);
  m_Ruler->SetLabelAxesVisibility();
  m_Ruler->SetLabelScaleVisibility(true);
  m_Ruler->SetAxesVisibility(false);
  m_Ruler->SetTickVisibility(true);
  m_Ruler->CenterAxesOnScreen(false);
  m_Ruler->UseGlobalAxesOff();
  m_Ruler->SetLegend(m_RulerLegend.GetCStr());
  m_Ruler->SetScaleFactor(m_RulerScaleFactor);
  m_Ruler->SetColor(1,1,1);
  m_RenFront->AddActor2D(m_Ruler);
  m_Ruler->SetVisibility(m_ShowRuler);

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

  m_ShowGrid    = use_grid;
  m_GridNormal  = GRID_Z;

  m_Grid = vtkMAFGridActor::New();
  m_RenFront->AddActor(m_Grid);
  m_RenFront->AddActor2D(m_Grid->GetLabelActor());
  SetGridNormal(m_GridNormal);
  SetGridVisibility(m_ShowGrid != 0);

  m_ShowAxes = show_axes;
  m_Axes = new mafAxes(m_RenFront);
  m_Axes->SetVisibility(show_axes);
}
//----------------------------------------------------------------------------
mafRWI::~mafRWI()
//----------------------------------------------------------------------------
{
  if (m_LinkCamera != 0) 
  {
    LinkCamera(false);
  }
  if(m_Grid) m_RenFront->RemoveActor(m_Grid);
  if(m_Grid) m_RenFront->RemoveActor2D(m_Grid->GetLabelActor());
  vtkDEL(m_Grid);
	
  if(m_Ruler) m_RenFront->RemoveActor2D(m_Ruler);
  vtkDEL(m_Ruler);

	if(m_Orientator) m_RenFront->RemoveActor2D(m_Orientator);
	vtkDEL(m_Orientator);

  cppDEL(m_Axes); //Must be removed before deleting renderers
  vtkDEL(m_Light);
  vtkDEL(m_Camera);
  if(m_RenFront) 
  {
    m_RenFront->RemoveAllProps();
    m_RenderWindow->RemoveRenderer(m_RenFront);
  }
  vtkDEL(m_RenFront);
  if(m_RenBack)
  {
    m_RenBack->RemoveAllProps();
    m_RenderWindow->RemoveRenderer(m_RenBack);
  }
  vtkDEL(m_RenBack);
  if(m_RenderWindow) 
    m_RenderWindow->SetInteractor(NULL);
  m_RenderWindow->Delete();
  if(m_RwiBase) 
    m_RwiBase->SetRenderWindow(NULL);
  vtkDEL(m_RwiBase);  //The renderer has to be Deleted as last
}
//-----------------------------------------------------------------------------------------
void mafRWI::CameraSet(int cam_position, double zoom)
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

  switch (cam_position) 
  {
    case CAMERA_FRONT:
    case CAMERA_PERSPECTIVE_FRONT:
      x=0; y=0; z=1; vx=0; vy=1; vz=0;
    break;
    case CAMERA_BACK:
    case CAMERA_PERSPECTIVE_BACK:
      x=0; y=0; z=-1;vx=0; vy=1; vz=0;
    break;
    case CAMERA_LEFT:
    case CAMERA_PERSPECTIVE_LEFT:
      x=-1 ;y=0; z=0; vx=0; vy=1; vz=0;
    break;
    case CAMERA_RIGHT:
    case CAMERA_PERSPECTIVE_RIGHT:
      x=1;y=0; z=0; vx=0; vy=1; vz=0;
    break;
    case CAMERA_TOP:
    case CAMERA_PERSPECTIVE_TOP:
      x=0; y=1; z=0; vx=0; vy=0; vz=-1;
    break;
    case CAMERA_BOTTOM:
    case CAMERA_PERSPECTIVE_BOTTOM:
      x=0; y=-1;z=0; vx=0; vy=0; vz=1;
    break;
    case CAMERA_PERSPECTIVE:
      //x=1; y=1; z=1; vx=0; vy=1; vz=0; //SIL. 23-6-2003 modified
      x=0; y=1; z=4; vx=0; vy=1; vz=0; 
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

  CameraReset((mafNode*)NULL, zoom);
}

//----------------------------------------------------------------------------
void mafRWI::CameraSet( double pos[3],double viewUp[3], double zoom /*= 1.*/, bool parallelProjection /*= false*/ )
//----------------------------------------------------------------------------
{
  if (parallelProjection)
    m_Camera->ParallelProjectionOn();
  else
    m_Camera->ParallelProjectionOff();

  m_Camera->SetFocalPoint(0,0,0);
  m_Camera->SetPosition(pos[0]*100,pos[1]*100,pos[2]*100);
  m_Camera->SetViewUp(viewUp[0],viewUp[1],viewUp[2]);
  m_Camera->SetClippingRange(0.1,1000);

  CameraReset((mafNode*)NULL, zoom);
}
//----------------------------------------------------------------------------
void mafRWI::SetSize(int x, int y, int w,int h)
//----------------------------------------------------------------------------
{
  ((wxWindow*)m_RwiBase)->SetSize(x,y,w,h);
  ((wxWindow*)m_RwiBase)->SetMinSize(wxSize(w,h));
}
//----------------------------------------------------------------------------
void mafRWI::Show(bool show)
//----------------------------------------------------------------------------
{
  m_RwiBase->Show(show);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridNormal(int normal_id)
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
void mafRWI::SetGridPosition(double position)
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
void mafRWI::SetGridVisibility(bool show)
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
void mafRWI::SetAxesVisibility(bool show)
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
void mafRWI::SetRuleVisibility(bool show)
//----------------------------------------------------------------------------
{
  if(m_Ruler)
  {
    m_ShowRuler = show;
    m_Ruler->SetVisibility(show);
    if (m_Gui != NULL)
    {
      m_Gui->Update();
    }
  }
}
//----------------------------------------------------------------------------
void mafRWI::SetOrientatorVisibility(bool show)
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
void mafRWI::SetRulerScaleFactor(const double &scale_factor)
//----------------------------------------------------------------------------
{
  m_RulerScaleFactor = scale_factor;
  m_Ruler->SetScaleFactor(m_RulerScaleFactor);
  if (m_Gui != NULL) 
  {
    m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void mafRWI::SetRulerLegend(const mafString &ruler_legend)
//----------------------------------------------------------------------------
{
  m_RulerLegend = ruler_legend;
  m_Ruler->SetLegend(m_RulerLegend.GetCStr());
  if (m_Gui != NULL) 
  {
    m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void mafRWI::SetGridColor(const wxColor &col)
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
void mafRWI::SetBackgroundColor(const wxColor &col)
//----------------------------------------------------------------------------
{
  m_BGColour = col;
  if (m_Gui != NULL) 
  {
    m_Gui->Update();
  }
  m_RenFront->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
  if(m_RenBack) 
    m_RenBack->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
}
//----------------------------------------------------------------------------
void mafRWI::SetStereo(int stereo_type)
//----------------------------------------------------------------------------
{
  if(m_RenBack) 
  {
    mafLogMessage("WARNING: SetStereo is disabled for RWI with two layers");
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
void mafRWI::CameraUpdate()
//----------------------------------------------------------------------------
{
  //if(!m_RwiBase->IsShown()) return;
  if (m_RenderWindow->GetGenericWindowId() == 0) 
    return;

  m_RenFront->ResetCameraClippingRange(); 
  m_RenderWindow->Render();
  if (m_StereoMovieEnable!=0)
  {
    m_RwiBase->GenerateStereoFrames();
  }
  UpdateCameraParameters();
}
//----------------------------------------------------------------------------
void mafRWI::UpdateCameraParameters()
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
void mafRWI::CameraReset(mafNode *vme, double zoom)
//----------------------------------------------------------------------------
{
  if (m_RenderWindow->GetGenericWindowId() == 0) 
    return;

  if(m_Grid && m_ShowGrid) 
    m_Grid->VisibilityOff();

  mafEventMacro(mafEvent(this,CAMERA_PRE_RESET,m_RenFront)); //- Attention - I'm sending m_RenFront, I suppose that m_RenBack is never required 
  CameraReset(ComputeVisibleBounds(vme), zoom);
  if(m_Grid && m_ShowGrid) 
    m_Grid->VisibilityOn();

  mafEventMacro(mafEvent(this,CAMERA_POST_RESET,m_RenFront));
  m_RenFront->ResetCameraClippingRange();
  m_RenderWindow->Render();
  UpdateCameraParameters();
}
//----------------------------------------------------------------------------
double *mafRWI::ComputeVisibleBounds(mafNode *node)
//----------------------------------------------------------------------------
{
  static double b[6],b1[6],b2[6]; // static so it is possible to return it
  mafVME *vme = NULL;
  
  if(node && (vme = mafVME::SafeDownCast(node)))
    if(!vme->IsA("mafNodeRoot"))
      if(vme->GetOutput()->GetVTKData())
	if(m_Sg) 
	  if(mafSceneNode *n = m_Sg->Vme2Node(vme) )
	    if(n->IsVisible())
	{
    /** Modified by Marco 24-6-2005: this is not generic: do ask the VME for its bounds
		vme->GetOutput()->GetVTKData()->GetBounds(b1);
    float loc_p1[3],loc_p2[3],abs_p1[3], abs_p2[3];
    loc_p1[0] = b1[0];
    loc_p1[1] = b1[2];
    loc_p1[2] = b1[4];
    loc_p2[0] = b1[1];
    loc_p2[1] = b1[3];
    loc_p2[2] = b1[5];
    */
    
    /* Modified by Marco 24-6-2005: this creates linking problems: do ask the VME for its bounds
    float loc_p1[3],loc_p2[3],abs_p1[3], abs_p2[3];
    loc_p1[0] = b1[0];
    loc_p1[1] = b1[2];
    loc_p1[2] = b1[4];
    loc_p2[0] = b1[1];
    loc_p2[1] = b1[3];
    loc_p2[2] = b1[5];
    
    double r=0;
    if(vme->IsA("mafVMELandmark"))
       r = ((mafVMELandmark *)vme)->GetRadius();
    if(vme->IsA("mafVMELandmarkCloud"))
       r = ((mafVMELandmarkCloud *)vme)->GetRadius();
		loc_p1[0] -= r;
		loc_p1[1] -= r;
		loc_p1[2] -= r;
		loc_p2[0] += r;
		loc_p2[1] += r;
		loc_p2[2] += r;
    */
    /* Modified by Marco 24-6-2005: this is not generic: do ask the VME for its bounds
    vtkLinearTransform *t = vme->GetAbsMatrixPipe()->GetVTKTransform();
		t->TransformPoint(loc_p1,abs_p1);
		t->TransformPoint(loc_p2,abs_p2);
    // TODO: test the usage of vtkMatrix4x4::MultiplyPoint instead

		b1[0] = abs_p1[0];
		b1[1] = abs_p2[0];
		b1[2] = abs_p1[1];
		b1[3] = abs_p2[1];
		b1[4] = abs_p1[2];
		b1[5] = abs_p2[2];
    */
  
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
void mafRWI::CameraReset(double bounds[6], double zoom)
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
  ID_CAMERA_POSITION,
  ID_CAMERA_VIEW_UP,
  ID_CAMERA_ORIENTATION,
  ID_LINK_CAMERA,
  ID_SHOW_RULER,
	ID_SHOW_ORIENTATOR,
  ID_RULER_SCALE_FACTOR,
  ID_RULER_LEGEND
};
//-------------------------------------------------------------------------
mmgGui *mafRWI::CreateGui()
//-------------------------------------------------------------------------
{
  wxString grid_normal[3] = {"X axes","Y axes","Z axes"};

  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  // constant used in mafRWIxxx to initialize Camera pos
  if (m_Camera->GetParallelProjection() == 0)
  {
    m_Sizer =  new wxBoxSizer( wxHORIZONTAL );
    m_CameraButtons[0] = new mmgPicButton(m_Gui, "PIC_BOTTOM",ID_CAMERA_BOTTOM, this);
    m_CameraButtons[1] = new mmgPicButton(m_Gui, "PIC_TOP",   ID_CAMERA_TOP,    this);
    m_CameraButtons[2] = new mmgPicButton(m_Gui, "PIC_BACK",  ID_CAMERA_BACK,   this);
    m_CameraButtons[3] = new mmgPicButton(m_Gui, "PIC_FRONT", ID_CAMERA_FRONT,  this);
    m_CameraButtons[4] = new mmgPicButton(m_Gui, "PIC_LEFT",  ID_CAMERA_LEFT,   this);
    m_CameraButtons[5] = new mmgPicButton(m_Gui, "PIC_RIGHT", ID_CAMERA_RIGHT,  this);
    for(int i = 0; i < 6; i++)
      m_Sizer->Add(m_CameraButtons[i],0,0);
    m_Gui->Add(m_Sizer);
    m_Gui->Label("Camera parameters");
    m_Gui->Vector(ID_FOCAL_POINT, "focal pnt.", m_FocalPoint);
    m_Gui->Vector(ID_CAMERA_POSITION, "position", m_CameraPosition);
    //m_Gui->Vector(ID_CAMERA_VIEW_UP, "view up", m_CameraViewUp);
    //m_Gui->Vector(ID_CAMERA_ORIENTATION, "orientation", m_CameraOrientation);
    mafGUICrossIncremental *ci = m_Gui->CrossIncremental(ID_CAMERA_ORIENTATION,"orientation", &m_StepCameraOrientation, &m_TopBottomAccumulation, &m_LeftRigthAccumulation,mafGUICrossIncremental::ID_COMPLETE_LAYOUT,"Change the rotation step value from the entry.",false);
    ci->EnableStep(true);
    m_Gui->Divider(2);
  }

  m_Gui->Bool(ID_SHOW_GRID,"grid",&m_ShowGrid,0,"Turn On/Off the grid");
  m_Gui->Combo(ID_GRID_NORMAL,"grid norm",&m_GridNormal,3,grid_normal,"orientation axes for the grid");
  m_Gui->Double(ID_GRID_POS,"grid pos",	&m_GridPosition);
  m_Gui->Color(ID_GRID_COLOR,"grid color",&m_GridColour);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_SHOW_AXES,"show axes",&m_ShowAxes,0);
  m_Gui->Color(ID_BG_COLOR,"back color",&m_BGColour);

  //////// ruler gui
  if (m_Camera->GetParallelProjection())
  {
    m_Gui->Divider(2);
    m_Gui->Bool(ID_SHOW_RULER,"show ruler",&m_ShowRuler);
    m_Gui->Double(ID_RULER_SCALE_FACTOR,"scale factor",&m_RulerScaleFactor,1.0e-299,MAXDOUBLE,-1);
    m_Gui->String(ID_RULER_LEGEND,"legend",&m_RulerLegend);
  }

  if (m_StereoType)
  {
    m_Gui->Divider(2);
    m_Gui->Label("stero movie");
    m_Gui->DirOpen(ID_STERO_MOVIE_DIR,"dir",&m_StereoMovieDir);
    m_Gui->Bool(ID_STERO_MOVIE,"Start rec",&m_StereoMovieEnable);
    m_Gui->Enable(ID_STERO_MOVIE_DIR,m_StereoType != 0);
    m_Gui->Enable(ID_STERO_MOVIE,false);
  }

  m_Gui->Divider(2);
  m_Gui->Bool(ID_LINK_CAMERA,"link camera",&m_LinkCamera,0,"Turn On/Off camera interaction synchronization");
	m_Gui->Bool(ID_SHOW_ORIENTATOR,"orientation",&m_ShowOrientator);
  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
mmgGui *mafRWI::GetGui()
//-------------------------------------------------------------------------
{
  if(m_Gui == NULL)
    return CreateGui();
  else
    return m_Gui;
}
//----------------------------------------------------------------------------
void mafRWI::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
      case ID_LINK_CAMERA:
      {
        LinkCamera(m_LinkCamera != 0);
      }
      break;
      case ID_SHOW_RULER:
        SetRuleVisibility(m_ShowRuler!= 0);
      break;
			case ID_SHOW_ORIENTATOR:
				SetOrientatorVisibility(m_ShowOrientator!= 0);
				CameraUpdate();
				break;
      case ID_RULER_SCALE_FACTOR:
        SetRulerScaleFactor(m_RulerScaleFactor);
      break;
      case ID_RULER_LEGEND:
        SetRulerLegend(m_RulerLegend);
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
void mafRWI::LinkCamera(bool linc_camera)
//----------------------------------------------------------------------------
{
  m_LinkCamera = linc_camera;
  if (m_Gui) 
  {
    m_Gui->Update();
  }

  mafEvent e(this,LINK_CAMERA_TO_INTERACTOR);
  e.SetVtkObj(m_Camera);
  e.SetBool(m_LinkCamera != 0);
  mafEventMacro(e);
}
//----------------------------------------------------------------------------
void mafRWI::UpdateRulerUnit()
//----------------------------------------------------------------------------
{
  mmgMeasureUnitSettings *unit_settings = new mmgMeasureUnitSettings(this);
  m_RulerScaleFactor = unit_settings->GetScaleFactor();
  m_RulerLegend = unit_settings->GetUnitName();
  cppDEL(unit_settings);
  m_Ruler->SetScaleFactor(m_RulerScaleFactor);
  m_Ruler->SetLegend(m_RulerLegend);
  if (m_Gui != NULL) 
  {
    m_Gui->Update();
  }
  CameraUpdate();
}
//----------------------------------------------------------------------------
void mafRWI::SetOrientatorProperties(double rgbText[3], double rgbBackground[3], double scale)
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
void mafRWI::SetOrientatorSingleActorVisibility(bool showLeft, bool showDown, bool showRight, bool showUp)
//----------------------------------------------------------------------------
{
  if(m_Orientator)
  {
    m_Orientator->SetSingleActorVisibility(vtkMAFTextOrientator::ID_ACTOR_LEFT, showLeft);
    m_Orientator->SetSingleActorVisibility(vtkMAFTextOrientator::ID_ACTOR_DOWN, showDown);
    m_Orientator->SetSingleActorVisibility(vtkMAFTextOrientator::ID_ACTOR_RIGHT, showRight);
    m_Orientator->SetSingleActorVisibility(vtkMAFTextOrientator::ID_ACTOR_UP, showUp);
  }
}
//----------------------------------------------------------------------------
void mafRWI::SetOrientatorSingleActorText(const char* textLeft, const char* textDown, const char* textRight, const char* textUp)
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
