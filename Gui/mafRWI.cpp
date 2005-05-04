/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRWI.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:44:02 $
  Version:   $Revision: 1.6 $
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
//#include "mafAxes.h"
//#include "mafPipe.h"
//#include "mafSceneGraph.h"
//#include "mafSceneNode.h"

#include "mafNode.h"
#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafVME.h"
//#include "mafNodeLandmark.h"
//#include "mafNodeLandmarkCloud.h"
//#include "mflMatrixPipeDirectCinematic.h"
//#include "mflAssembly.h"

//#include "vtkGridActor.h"  // users must see GRID_XYZ const declared in vtkGridActor
#include "vtkLight.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkLinearTransform.h"
#include "vtkRenderWindow.h"
#include "vtkMath.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
mafRWI::mafRWI(wxWindow *parent, RWI_LAYERS layers, bool use_grid, int stereo)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_Sg = NULL;

  m_Light = vtkLight::New();
  m_Light->SetLightTypeToCameraLight();  

  m_Camera = vtkCamera::New();
	m_Camera->SetViewAngle(20); 
  m_Camera->ParallelProjectionOn(); 

	m_RenFront = vtkRenderer::New();
  m_RenFront->SetBackground(0.4,0.4,0.4);
  m_RenFront->SetActiveCamera(m_Camera);
  m_RenFront->AddLight(m_Light);
  m_RenFront->BackingStoreOff();
  m_RenFront->LightFollowCameraOn(); 
	
  if(stereo)
  {
    m_RenderWindow = vtkRenderWindow::New(); //SIL. 13-11-2003: - temporary removed - cause error in OpenGLMApper during close
    m_RenderWindow->StereoCapableWindowOn();
    m_RenderWindow->StereoRenderOn();
    SetStereo(stereo);
  }
  else
    m_RenderWindow = vtkRenderWindow::New(); 
  m_RenderWindow->AddRenderer(m_RenFront);

  m_Rwi = new mafRWIBase(parent, -1);
	m_Rwi->SetRenderWindow(m_RenderWindow);
  
	m_RenBack = NULL;
	if(layers == TWO_LAYER)
	{
		m_RenBack = vtkRenderer::New();
		m_RenBack->SetBackground(0.4,0.4,0.4);
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

	m_Grid = NULL;
/*
  m_ShowGrid = use_grid;

  if(use_grid)
	{
     m_Grid= vtkGridActor::New();
		 m_RenFront->AddActor(m_Grid);
		 m_RenFront->AddActor2D(m_Grid->GetLabelActor());
	   SetGridNormal(GRID_Y);
	}

	m_Axes = new mafAxes(m_RenFront);
  m_Axes->SetVisibility(0);
  */
}
//----------------------------------------------------------------------------
mafRWI::~mafRWI( ) 
//----------------------------------------------------------------------------
{
	  /*
    if(m_Grid) m_RenFront->RemoveActor(m_Grid);
		if(m_Grid) m_RenFront->RemoveActor2D(m_Grid->GetLabelActor());
		vtkDEL(m_Grid);
		
		wxDEL(m_Axes); //SIL. 31-10-2003: must be removed before deleting renderers
		*/
    vtkDEL(m_Light);
		vtkDEL(m_Camera);
		if(m_RenFront) 
    {
      m_RenFront->GetActors();        //Paolo 23-06-2004
      m_RenderWindow->RemoveRenderer(m_RenFront);
    }
    vtkDEL(m_RenFront);
    if(m_RenBack) 
    {
      m_RenBack->GetActors();        //Paolo 23-06-2004
      m_RenderWindow->RemoveRenderer(m_RenBack);
    }
		vtkDEL(m_RenBack);
		if(m_RenderWindow) 
      m_RenderWindow->SetInteractor(NULL);    //Paolo 23-06-2004
    m_RenderWindow->Delete();
    //if(m_Rwi) 
      //m_Rwi->SetRenderWindow(NULL); //Paolo 23-06-2004
		vtkDEL(m_Rwi);  //SIL. 13-11-2003: The renderer has to be Deleted as last
}
//-----------------------------------------------------------------------------------------
void mafRWI::CameraSet(int cam_position)
//-----------------------------------------------------------------------------------------
{
  int x,y,z,vx,vy,vz;
  m_CameraPosition = cam_position;

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
		case CAMERA_CT:
			x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
		break;
		case CAMERA_OS_X:
			x=-1 ;y=0; z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_OS_Y:
			x=0; y=-1; z=0; vx=0; vy=0; vz=1;
		break;
		case CAMERA_OS_Z:
			x=0; y=0; z=-1; vx=0; vy=-1; vz=0;
		break;
		case CAMERA_OS_P:
      m_Camera->ParallelProjectionOff();     // Paolo 09/06/2004
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
    // ste end
  }
  m_Camera->SetFocalPoint(0,0,0);
  m_Camera->SetPosition(x*100,y*100,z*100);
  m_Camera->SetViewUp(vx,vy,vz);
  
	CameraReset();

}
//----------------------------------------------------------------------------
void mafRWI::SetSize(int x, int y, int w,int h)
//----------------------------------------------------------------------------
{
   ((wxWindow*)m_Rwi)->SetSize(x,y,w,h);
}
//----------------------------------------------------------------------------
void mafRWI::Show(bool show)
//----------------------------------------------------------------------------
{
   m_Rwi->Show(show);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridNormal(int normal_id)
//----------------------------------------------------------------------------
{
//   if(m_Grid) m_Grid->SetGridNormal(normal_id);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridPosition(float position)
//----------------------------------------------------------------------------
{
//   if(m_Grid) m_Grid->SetGridPosition(position);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridVisibility(bool show)
//----------------------------------------------------------------------------
{
/* 
  if(m_Grid)
	 {
	   m_ShowGrid = show;
		 m_Grid->SetVisibility(m_ShowGrid );
	   m_Grid->GetLabelActor()->SetVisibility(m_ShowGrid );
	 }
   */
}
//----------------------------------------------------------------------------
void mafRWI::SetAxesVisibility(bool show)
//----------------------------------------------------------------------------
{
   //if(m_Axes) m_Axes->SetVisibility(show);
}
//----------------------------------------------------------------------------
void mafRWI::SetGridColor(wxColor col)
//----------------------------------------------------------------------------
{
   //if(m_Grid) m_Grid->SetGridColor(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
}
//----------------------------------------------------------------------------
void mafRWI::SetBackgroundColor(wxColor col)
//----------------------------------------------------------------------------
{
            m_RenFront->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
   if(m_RenBack) m_RenBack->SetBackground(col.Red()/255.0,col.Green()/255.0,col.Blue()/255.0);
}
//----------------------------------------------------------------------------
void mafRWI::SetStereo(int stereo_type)
//----------------------------------------------------------------------------
{
  if(m_RenBack) {wxLogMessage("LAL WARNING: SetStereo is disabled for RWI with two layers"); return;}
	
  if (stereo_type < VTK_STEREO_CRYSTAL_EYES)
  stereo_type = 0; 
	  
  if(m_StereoType == stereo_type) 
	   return;
 
  m_StereoType = stereo_type;
  
  //warning: non portable
  //m_RenderWindow->SetWindowId( (HWND) 0 );

  
  int *size = m_RenderWindow->GetSize();
	//m_RenderWindow->RemoveRenderer(m_RenFront);
  //vtkDEL(m_RenderWindow);
	//m_RenderWindow = vtkRenderWindow::New();
  //m_RenderWindow->SetSize(size);
  m_RenderWindow->SetStereoCapableWindow(m_StereoType != 0);
  m_RenderWindow->SetStereoType(m_StereoType);
  m_RenderWindow->SetStereoRender(m_StereoType != 0);
	

  //warning: non portable
  //m_RenderWindow->SetWindowId( (HWND) m_Rwi->GetHWND() );

  //m_RenderWindow->AddRenderer(m_RenFront);
  //m_Rwi->SetRenderWindow(m_RenderWindow);
  m_Rwi->ReInitialize();
}
//----------------------------------------------------------------------------
void mafRWI::CameraUpdate()
//----------------------------------------------------------------------------
{
  //if(!m_Rwi->IsShown()) return; //TODO: capire perche' non funziona piu' - risulta sempre nascosta
  if (m_RenderWindow->GetGenericWindowId() == 0) 
		return;

	m_RenFront->ResetCameraClippingRange(); 
  m_RenderWindow->Render();

}
//----------------------------------------------------------------------------
void mafRWI::CameraReset(mafNode *vme)
//----------------------------------------------------------------------------
{
  if (m_RenderWindow->GetGenericWindowId() == 0) 
		return;

//	if(m_Grid && m_ShowGrid) m_Grid->VisibilityOff();

  mafEventMacro(mafEvent(this,CAMERA_PRE_RESET,m_RenFront));  //SIL. 16-6-2004: - Attention - I'm sending m_RenFront, I suppose that m_RenBack is never required 
  CameraReset(ComputeVisibleBounds(vme));
//	if(m_Grid && m_ShowGrid) m_Grid->VisibilityOn();

  mafEventMacro(mafEvent(this,CAMERA_POST_RESET,m_RenFront)); //SIL. 16-6-2004:
  m_RenFront->ResetCameraClippingRange(); //per la griglia
  m_RenderWindow->Render();
}
//----------------------------------------------------------------------------
double *mafRWI::ComputeVisibleBounds(mafNode *node)
//----------------------------------------------------------------------------
{
  static double b1[6],b2[6]; // static so it is possible to return it
  mafVME *vme = NULL;
  
  if(node && (vme = mafVME::SafeDownCast(node)))
		if(!vme->IsA("mafNodeRoot"))
			if(vme->GetOutput()->GetVTKData())
				if(m_Sg) 
					if(mafSceneNode *n = m_Sg->Vme2Node(vme) )
						if(n->IsVisible())
	{
		vme->GetOutput()->GetVTKData();
		vme->GetOutput()->GetVTKData()->GetBounds(b1);
    float loc_p1[3],loc_p2[3],abs_p1[3], abs_p2[3];
    loc_p1[0] = b1[0];
    loc_p1[1] = b1[2];
    loc_p1[2] = b1[4];
    loc_p2[0] = b1[1];
    loc_p2[1] = b1[3];
    loc_p2[2] = b1[5];

    double r=0;
/*    if(vme->IsA("mafNodeLandmark"))
       r = ((mafNodeLandmark*)vme)->GetRadius();
    if(vme->IsA("mafNodeLandmarkCloud"))
       r = ((mafNodeLandmarkCloud*)vme)->GetRadius();*/
		loc_p1[0] -= r;
		loc_p1[1] -= r;
		loc_p1[2] -= r;
		loc_p2[0] += r;
		loc_p2[1] += r;
		loc_p2[2] += r;
    
    vtkLinearTransform *t = (vtkLinearTransform *)vme->GetAbsMatrixPipe();
		t->TransformPoint(loc_p1,abs_p1);
		t->TransformPoint(loc_p2,abs_p2);
    // TODO: test the usage of vtkMatrix4x4::MultiplyPoint instead

		b1[0] = abs_p1[0];
		b1[1] = abs_p2[0];
		b1[2] = abs_p1[1];
		b1[3] = abs_p2[1];
		b1[4] = abs_p1[2];
		b1[5] = abs_p2[2];

		return b1;
	}
	m_RenFront->ComputeVisiblePropBounds(b1);   
	return b1;
}
//----------------------------------------------------------------------------
void mafRWI::CameraReset(double bounds[6])
//----------------------------------------------------------------------------
{
  double view_up[3], view_look[3], view_right[3];

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
	double view_aspect  = (m_Rwi->m_Width*1.0)/(m_Rwi->m_Height*1.0);
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
  m_Camera->SetPosition(center[0]+distance*vl[0],
                   center[1]+distance*vl[1],
                   center[2]+distance*vl[2]);

  // setup parallel scale
  m_Camera->SetParallelScale(height);
	
	//m_RenFront->ResetCameraClippingRange(bounds);
}