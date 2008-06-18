/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewGlobalSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2008-06-18 13:27:48 $
  Version:   $Revision: 1.27 $
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

#include "mafViewGlobalSlice.h"
#include "mafObserver.h"
#include "mafNode.h"
#include "mafSceneNode.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipeMeshSlice.h"
#include "mafAttachCamera.h"
#include "mmgFloatSlider.h"

#include "mafTransform.h"
#include "mafAbsMatrixPipe.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEVolume.h"

#include "vtkCellPicker.h"
#include "vtkRayCast3DPicker.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPlaneSource.h"
#include "vtkProperty.h"
#include "vtkActor.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewGlobalSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------
enum PLANE_ID
{
	ID_XY = 0,
	ID_XZ,
	ID_YZ,
};

//----------------------------------------------------------------------------
mafViewGlobalSlice::mafViewGlobalSlice(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
: mafViewVTK(label,camera_position,show_axes,show_grid, show_ruler, stereo)
//----------------------------------------------------------------------------
{
	m_SliceOrigin[0] = m_SliceOrigin[1] = m_SliceOrigin[2] = 0.0;
	m_IDPlane = ID_XY;
	m_SliceMode = SLICE_ARB;
	m_Opacity = 1.0;

	m_SliderOldOrigin = 0.0; 
	m_SliderOrigin = 0.0;

	m_SliceXVector[0] = 1;
	m_SliceXVector[1] = 0;
	m_SliceXVector[2] = 0;
  
	m_SliceYVector[0] = 0;
	m_SliceYVector[1] = 1;
	m_SliceYVector[2] = 0;

	vtkMath::Normalize(m_SliceXVector);
	vtkMath::Normalize(m_SliceYVector);
	vtkMath::Cross(m_SliceYVector, m_SliceXVector, m_SliceNormal);
	vtkMath::Normalize(m_SliceNormal);
	vtkMath::Cross(m_SliceNormal, m_SliceXVector, m_SliceYVector);
	vtkMath::Normalize(m_SliceYVector);

	m_GlobalBounds[0] = m_GlobalBounds[2] = m_GlobalBounds[4] = -1000.0;
	m_GlobalBounds[1] = m_GlobalBounds[3] = m_GlobalBounds[5] = 1000.0;

	m_GlobalBoundsInitialized = false;
  m_GlobalBoundsValid = true;

	m_ViewIndex = ID_XY;

	m_GlobalSlider = NULL;
	m_OpacitySlider = NULL;
	m_SelectedVolume = NULL;

	m_TextActor = NULL;
	m_TextMapper = NULL;
	m_TextColor[0]=1.0;
	m_TextColor[1]=1.0;
	m_TextColor[2]=1.0;

	m_Dn = 0.0;

	m_BoundsPlane = NULL;
	m_BoundsOutlineBox = NULL;
	m_BoundsOutlineProperty = NULL;
	m_BoundsOutlineActor = NULL;
	m_BoundsOutlineMapper = NULL;
}
//----------------------------------------------------------------------------
mafViewGlobalSlice::~mafViewGlobalSlice()
//----------------------------------------------------------------------------
{
	if(m_Rwi && m_TextActor)
		m_Rwi->m_RenFront->RemoveActor2D(m_TextActor);
	if(m_Rwi && m_BoundsOutlineActor)
		m_Rwi->m_RenFront->RemoveActor2D(m_BoundsOutlineActor);


	vtkDEL(m_BoundsOutlineActor);
	vtkDEL(m_BoundsOutlineMapper);
	vtkDEL(m_BoundsOutlineProperty);
	vtkDEL(m_BoundsOutlineBox);
	vtkDEL(m_BoundsPlane);

	vtkDEL(m_TextActor);
	vtkDEL(m_TextMapper);
}
//----------------------------------------------------------------------------
mafView *mafViewGlobalSlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewGlobalSlice *v = new mafViewGlobalSlice(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::Create()
//----------------------------------------------------------------------------
{
  RWI_LAYERS num_layers = m_CameraPositionId != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

	m_Text = "";
	m_TextMapper = vtkTextMapper::New();
	m_TextMapper->SetInput(m_Text.GetCStr());
	m_TextMapper->GetTextProperty()->AntiAliasingOff();

	m_TextActor = vtkActor2D::New();
	m_TextActor->SetMapper(m_TextMapper);
	m_TextActor->SetPosition(3,3);
	m_TextActor->GetProperty()->SetColor(m_TextColor);

	m_Rwi->m_RenFront->AddActor(m_TextActor);

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.001);
  //m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::InizializePlane()
//----------------------------------------------------------------------------
{
	vtkNEW(m_BoundsPlane);
	m_BoundsPlane->SetOrigin(m_GlobalBounds[0],m_GlobalBounds[2],m_SliceOrigin[2]);
	m_BoundsPlane->SetPoint1(m_GlobalBounds[1],m_GlobalBounds[2],m_SliceOrigin[2]);
	m_BoundsPlane->SetPoint2(m_GlobalBounds[0],m_GlobalBounds[3],m_SliceOrigin[2]);

	m_BoundsPlane->Update();

	vtkNEW(m_BoundsOutlineBox);
	m_BoundsOutlineBox->SetInput(m_BoundsPlane->GetOutput());  

	vtkNEW(m_BoundsOutlineMapper);
	m_BoundsOutlineMapper->SetInput(m_BoundsOutlineBox->GetOutput());

	vtkNEW(m_BoundsOutlineProperty);
	m_BoundsOutlineProperty->SetColor(1,1,1);
	m_BoundsOutlineProperty->SetAmbient(1);
	m_BoundsOutlineProperty->SetRepresentationToWireframe();
	m_BoundsOutlineProperty->SetInterpolationToFlat();

	vtkNEW(m_BoundsOutlineActor);
	m_BoundsOutlineActor->SetMapper(m_BoundsOutlineMapper);
	m_BoundsOutlineActor->VisibilityOn();
	m_BoundsOutlineActor->PickableOff();
	m_BoundsOutlineActor->SetProperty(m_BoundsOutlineProperty);

	m_Rwi->m_RenFront->AddActor(m_BoundsOutlineActor);
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::VmeSelect(mafNode *node,bool select)
//----------------------------------------------------------------------------
{
	assert(m_Sg); 
  m_Sg->VmeSelect(node,select);
  if(select && m_Gui)
  {
    /*if (!m_GlobalBoundsInitialized)
    {
      ((mafVME*)node->GetRoot())->GetOutput()->GetVME4DBounds(m_GlobalBounds);

			double bounds[6];
			((mafVME*)node->GetRoot())->GetOutput()->GetBounds(bounds);
			UpdateSliceParameters();
			InizializePlane();

			CameraUpdate();
    }*/

    m_SelectedVolume = m_Sg->Vme2Node(node);
    if (m_SelectedVolume->m_Pipe)
    {
      //m_Gui->Enable(ID_LUT,true);
			m_Gui->Enable(ID_POS_SLIDER,true);
      if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
      {
        m_Opacity = ((mafPipeVolumeSlice *)m_SelectedVolume->m_Pipe)->GetSliceOpacity();
        m_Gui->Enable(ID_OPACITY_SLIDER,true);
      }
      m_Gui->Update();
    }
		else
		{
			m_Gui->Enable(ID_POS_SLIDER,false);
			m_Gui->Update();
		}
  }
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::VmeCreatePipe(mafNode *node)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(node, pipe_name);

  mafSceneNode *n = m_Sg->Vme2Node(node);
  assert(n && !n->m_Pipe);

  m_GlobalBoundsValid = false; // new VME is shown into the view => Update the Global Bounds
	if (!m_GlobalBoundsInitialized || !m_GlobalBoundsValid)
  {
    ((mafVME*)node)->GetOutput()->GetVME4DBounds(m_GlobalBounds);
    UpdateSliceParameters();
		InizializePlane();
		//CameraUpdate();
  }

  double *new_point;
  double applied_origin[3];
  float applied_xVector[3];
  float applied_yVector[3];
  float applied_normal[3];
	
	mafVME *vme = mafVME::SafeDownCast(node);
	vtkTransform *transform;
	vtkNEW(transform);
	transform->SetMatrix(vme->GetOutput()->GetMatrix()->GetVTKMatrix());
	transform->Inverse();
	transform->Update();
	new_point=transform->TransformDoublePoint(m_SliceOrigin);
	applied_origin[0] = new_point[0];
  applied_origin[1] = new_point[1];
  applied_origin[2] = new_point[2];
	transform->TransformNormal(m_SliceNormal,applied_normal);
  transform->TransformNormal(m_SliceXVector, applied_xVector);
  transform->TransformNormal(m_SliceYVector, applied_yVector);

  if (pipe_name != "")
  {
    if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 1)
    {
      m_NumberOfVisibleVme = 1;
    }
    else
    {
      m_NumberOfVisibleVme++;
    }
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      if (pipe_name.Equals("mafPipeVolumeSlice"))
      {
        ((mafPipeVolumeSlice *)pipe)->InitializeSliceParameters(m_SliceMode,applied_origin,applied_xVector,applied_yVector,true,false);
			}
      else if(pipe_name.Equals("mafPipeSurfaceSlice"))
      {
				((mafPipeSurfaceSlice *)pipe)->ShowBoxSelectionOn();
        ((mafPipeSurfaceSlice *)pipe)->SetSlice(m_SliceOrigin);
				double DoubleNormal[3];
				DoubleNormal[0]=(double)m_SliceNormal[0];
				DoubleNormal[1]=(double)m_SliceNormal[1];
				DoubleNormal[2]=(double)m_SliceNormal[2];
				((mafPipeSurfaceSlice *)pipe)->SetNormal(DoubleNormal);
			}
      else if(pipe_name.Equals("mafPipeMeshSlice"))
      {
        double DoubleNormal[3];
        DoubleNormal[0]=(double)m_SliceNormal[0];
        DoubleNormal[1]=(double)m_SliceNormal[1];
        DoubleNormal[2]=(double)m_SliceNormal[2];
        double positionSlice[3];
        positionSlice[0] = m_SliceOrigin[0];
        positionSlice[1] = m_SliceOrigin[1];
        positionSlice[2] = m_SliceOrigin[2];
        ((mafPipeMeshSlice *)pipe)->SetSlice(positionSlice);
        ((mafPipeMeshSlice *)pipe)->SetNormal(DoubleNormal);
      }
      pipe->Create(n);
			n->m_Pipe = (mafPipe*)pipe;

			if (pipe_name.Equals("mafPipeVolumeSlice"))
			{
				((mafPipeVolumeSlice *)pipe)->HideSlider();
			}

			if (m_SelectedVolume == n && ((mafVME *)m_SelectedVolume->m_Vme)->GetOutput()->IsA("mafVMEOutputVolume"))
			{
				m_Gui->Enable(ID_OPACITY_SLIDER,true);
				m_Opacity=((mafPipeVolumeSlice *)m_SelectedVolume->m_Pipe)->GetSliceOpacity();
				m_Gui->Update();
			}

      if (m_NumberOfVisibleVme == 1)
      {
        mafEventMacro(mafEvent(this,CAMERA_RESET));
      }
      else
      {
        CameraUpdate();
      }
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
	transform->Delete();
  transform = NULL;

	UpdateText();
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_GlobalBoundsValid = false;
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 0)
    m_NumberOfVisibleVme = 0;
  else
    m_NumberOfVisibleVme--;

  if (((mafVME *)vme)->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (m_AttachCamera)
    {
      m_AttachCamera->SetVme(NULL);
    }
  }
	if (m_SelectedVolume == n)
  {
    //m_Gui->Enable(ID_LUT,false);
    m_Gui->Enable(ID_OPACITY_SLIDER,false);
    m_Gui->Update();
  }
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(maf_event->GetId()) 
    { 
			/*case ID_LUT:
			{
				ColorLookupTable(m_lut_index != 0);
				CameraUpdate();
			}
			break;*/
			case ID_OPACITY_SLIDER:
			{
				mafPipeVolumeSlice* pipe = (mafPipeVolumeSlice *)m_SelectedVolume->m_Pipe;
				if (pipe)
				{
					pipe->SetSliceOpacity(m_Opacity);
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
					m_OpacitySlider->SetValue(m_Opacity);
				}
			}
			break;
			case ID_POS_SLIDER:
				m_Dn = m_SliderOrigin - m_SliderOldOrigin;
				UpdateSlice();
				m_SliderOldOrigin = m_SliderOrigin;
				m_GlobalSlider->SetValue(m_SliderOrigin);
			break;
			case ID_CHANGE_VIEW:
			{
				UpdateSliceParameters();
				//InizializePlane();
				SetSlice(m_SliceOrigin,m_SliceXVector,m_SliceYVector);
			}
      default:
				mafViewVTK::OnEvent(maf_event);
    }
  }
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::UpdateSliceParameters()
//----------------------------------------------------------------------------
{
	if(!m_GlobalBoundsInitialized)
	{
		m_SliceOrigin[0] = (m_GlobalBounds[0] + m_GlobalBounds[1]) * .5;
		m_SliceOrigin[1] = (m_GlobalBounds[2] + m_GlobalBounds[3]) * .5;
		m_SliceOrigin[2] = (m_GlobalBounds[4] + m_GlobalBounds[5]) * .5;
	}

  double new_bounds[3] = {0.0,0.0,0.0};
	int index;
  switch(m_ViewIndex)
  {
    case ID_XY:
			index = 2;
			m_SliceXVector[0] = 1;
			m_SliceXVector[1] = 0;
			m_SliceXVector[2] = 0;
			m_SliceYVector[0] = 0;
			m_SliceYVector[1] = 1;
			m_SliceYVector[2] = 0;
      new_bounds[0] = m_GlobalBounds[4];
      new_bounds[1] = m_GlobalBounds[5];
      new_bounds[2] = (m_GlobalBounds[4] + m_GlobalBounds[5]) * .5;
      if(!m_GlobalBoundsInitialized)
				m_SliceOrigin[2] = new_bounds[2];
    break;
    case ID_XZ:
			index = 1;
			m_SliceXVector[0] = 1;
			m_SliceXVector[1] = 0;
			m_SliceXVector[2] = 0;
			m_SliceYVector[0] = 0;
			m_SliceYVector[1] = 0;
			m_SliceYVector[2] = 1;
      new_bounds[0] = m_GlobalBounds[2];
      new_bounds[1] = m_GlobalBounds[3];
      new_bounds[2] = (m_GlobalBounds[2] + m_GlobalBounds[3]) * .5;
      if(!m_GlobalBoundsInitialized)
				m_SliceOrigin[1] = new_bounds[2];
    break;
    case ID_YZ:
			index = 0;
			m_SliceXVector[0] = 0.0001;
			m_SliceXVector[1] = 1;
			m_SliceXVector[2] = 0;
			m_SliceYVector[0] = 0;
			m_SliceYVector[1] = 0;
			m_SliceYVector[2] = 1;
      new_bounds[0] = m_GlobalBounds[0];
      new_bounds[1] = m_GlobalBounds[1];
      new_bounds[2] = (m_GlobalBounds[0] + m_GlobalBounds[1]) * .5;
      if(!m_GlobalBoundsInitialized)
				m_SliceOrigin[0] = new_bounds[2];
    break;
  }
	m_GlobalSlider->SetNumberOfSteps((new_bounds[1] - new_bounds[0]) * 10);
  m_GlobalSlider->SetRange(new_bounds[0], new_bounds[1], m_SliceOrigin[index]);
	m_GlobalSlider->SetValue(m_SliceOrigin[index]);
	m_GlobalSlider->Update();

	m_SliderOldOrigin = m_GlobalSlider->GetValue();
	m_SliderOrigin = m_GlobalSlider->GetValue();
	m_GlobalBoundsInitialized = true;

	UpdateText();
  m_Gui->Update();
}
//----------------------------------------------------------------------------
mmgGui* mafViewGlobalSlice::CreateGui()
//----------------------------------------------------------------------------
{
	wxString Views[3] = {"XY","XZ","YZ"};

	assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
	
	m_GlobalSlider = m_Gui->FloatSlider(ID_POS_SLIDER,"pos.",&m_SliderOrigin,m_GlobalBounds[4],m_GlobalBounds[5]);
	m_Gui->Combo(ID_CHANGE_VIEW,"view",&m_ViewIndex,3,Views);
	m_OpacitySlider = m_Gui->FloatSlider(ID_OPACITY_SLIDER,"opacity",&m_Opacity,0.1,1.0);

	bool Enable = false;
  mafNode *selVME = m_Sg->GetSelectedVme();
  if (selVME&&(((mafVME *)selVME)->GetOutput()->IsA("mafVMEOutputVolume")))
  {
		m_SelectedVolume = m_Sg->Vme2Node(selVME);
		//m_gui->Enable(ID_LUT,en);
		m_Gui->Enable(ID_OPACITY_SLIDER,true);
  }
	else
	{
		m_Gui->Enable(ID_OPACITY_SLIDER,false);
	}

  m_SliderOldOrigin = m_SliderOrigin;

	m_Gui->Divider();
	m_Gui->Update();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::UpdateSlice()
//----------------------------------------------------------------------------
{
	if(!mafEquals(m_Dn, 0.0))
	{
		m_SliceOrigin[0] = m_SliceOrigin[0] + abs(m_SliceNormal[0]) * m_Dn;
		m_SliceOrigin[1] = m_SliceOrigin[1] + abs(m_SliceNormal[1]) * m_Dn;
		m_SliceOrigin[2] = m_SliceOrigin[2] + abs(m_SliceNormal[2]) * m_Dn;

		if(m_SliceOrigin[0] > m_GlobalBounds[1])
			m_SliceOrigin[0] = m_GlobalBounds[1];
		if(m_SliceOrigin[0] < m_GlobalBounds[0])
			m_SliceOrigin[0] = m_GlobalBounds[0];
		if(m_SliceOrigin[1] > m_GlobalBounds[3])
			m_SliceOrigin[1] = m_GlobalBounds[3];
		if(m_SliceOrigin[1] < m_GlobalBounds[2])
			m_SliceOrigin[1] = m_GlobalBounds[2];
		if(m_SliceOrigin[2] > m_GlobalBounds[5])
			m_SliceOrigin[2] = m_GlobalBounds[5];
		if(m_SliceOrigin[2] < m_GlobalBounds[4])
			m_SliceOrigin[2] = m_GlobalBounds[4];
	}

	for(mafSceneNode *node = m_Sg->GetNodeList(); node; node=node->m_Next)
	{
    if(node->m_Pipe)
    {
			double *new_point;
			double applied_origin[3];
			float applied_xVector[3];
			float applied_yVector[3];
			float applied_normal[3];

			mafVME *vme = mafVME::SafeDownCast(node->m_Vme);
			vtkTransform *transform;
			vtkNEW(transform);
			transform->Identity();
			transform->SetMatrix(vme->GetOutput()->GetMatrix()->GetVTKMatrix());
			transform->Inverse();
			transform->Update();
			new_point=transform->TransformDoublePoint(m_SliceOrigin);
			applied_origin[0] = new_point[0];
			applied_origin[1] = new_point[1];
			applied_origin[2] = new_point[2];
			transform->TransformNormal(m_SliceNormal,applied_normal);
			transform->TransformNormal(m_SliceXVector, applied_xVector);
			transform->TransformNormal(m_SliceYVector, applied_yVector);

		  if(node->m_Vme->IsA("mafVMESurface")||node->m_Vme->IsA("mafVMESurfaceParametric")||node->m_Vme->IsA("mafVMELandmarkCloud")||node->m_Vme->IsA("mafVMELandmark"))
		  {
        ((mafPipeSurfaceSlice *)node->m_Pipe)->SetSlice(m_SliceOrigin);
				double DoubleNormal[3];
				DoubleNormal[0]=(double)m_SliceNormal[0];
				DoubleNormal[1]=(double)m_SliceNormal[1];
				DoubleNormal[2]=(double)m_SliceNormal[2];
				((mafPipeSurfaceSlice *)node->m_Pipe)->SetNormal(DoubleNormal);
      }
      if(node->m_Vme->IsA("mafVMEMesh"))
      {
        ((mafPipeMeshSlice *)node->m_Pipe)->SetSlice(m_SliceOrigin);
        double DoubleNormal[3];
        DoubleNormal[0]=(double)m_SliceNormal[0];
        DoubleNormal[1]=(double)m_SliceNormal[1];
        DoubleNormal[2]=(double)m_SliceNormal[2];
        ((mafPipeMeshSlice *)node->m_Pipe)->SetNormal(DoubleNormal);
      }
      else if(((mafVME *)node->m_Vme)->GetOutput()->IsA("mafVMEOutputVolume"))
      {
        ((mafPipeVolumeSlice *)node->m_Pipe)->SetSlice(applied_origin, applied_xVector, applied_yVector);
      }
      transform->Delete();
      transform = NULL;
    }
  }

	m_Dn = 0.0;

	UpdatePlane();
	UpdateText();
  
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::UpdatePlane()
//----------------------------------------------------------------------------
{
	if(m_BoundsPlane &&  m_BoundsOutlineActor && m_Rwi)
	{
		switch(m_ViewIndex)
		{
		  case ID_XY:
			  m_BoundsPlane->SetOrigin(m_GlobalBounds[0],m_GlobalBounds[2],m_SliceOrigin[2]);
			  m_BoundsPlane->SetPoint1(m_GlobalBounds[1],m_GlobalBounds[2],m_SliceOrigin[2]);
			  m_BoundsPlane->SetPoint2(m_GlobalBounds[0],m_GlobalBounds[3],m_SliceOrigin[2]);
			  m_BoundsPlane->Update();
			break;
		  case ID_XZ:
			  m_BoundsPlane->SetOrigin(m_GlobalBounds[0],m_SliceOrigin[1],m_GlobalBounds[4]);
			  m_BoundsPlane->SetPoint1(m_GlobalBounds[1],m_SliceOrigin[1],m_GlobalBounds[4]);
			  m_BoundsPlane->SetPoint2(m_GlobalBounds[0],m_SliceOrigin[1],m_GlobalBounds[5]);
			  m_BoundsPlane->Update();
			break;
		  case ID_YZ:
			  m_BoundsPlane->SetOrigin(m_SliceOrigin[0],m_GlobalBounds[2],m_GlobalBounds[4]);
			  m_BoundsPlane->SetPoint1(m_SliceOrigin[0],m_GlobalBounds[3],m_GlobalBounds[4]);
			  m_BoundsPlane->SetPoint2(m_SliceOrigin[0],m_GlobalBounds[2],m_GlobalBounds[5]);
			  m_BoundsPlane->Update();
			break;
		}
	}
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::SetSlice(double origin[3], float xVect[3], float yVect[3])
//----------------------------------------------------------------------------
{
	m_SliceXVector[0] = xVect[0];
	m_SliceXVector[1] = xVect[1];
	m_SliceXVector[2] = xVect[2];

	m_SliceYVector[0] = yVect[0];
	m_SliceYVector[1] = yVect[1];
	m_SliceYVector[2] = yVect[2];

	vtkMath::Normalize(m_SliceXVector);
	vtkMath::Normalize(m_SliceYVector);
	vtkMath::Cross(m_SliceYVector, m_SliceXVector, m_SliceNormal);
	vtkMath::Normalize(m_SliceNormal);
	vtkMath::Cross(m_SliceNormal, m_SliceXVector, m_SliceYVector);
	vtkMath::Normalize(m_SliceYVector);

  SetSlice(origin);
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::SetSlice(double origin[3], double dn)
//----------------------------------------------------------------------------
{
	if(m_SliceMode != SLICE_ARB)
  {
    m_SliceOrigin[m_SliceMode] = origin[m_SliceMode];
  }
  else
  {
    m_SliceOrigin[0] = origin[0];
	  m_SliceOrigin[1] = origin[1];
	  m_SliceOrigin[2] = origin[2];
  }

	m_Dn = dn;

	UpdateSlice();
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::UpdateText()
//----------------------------------------------------------------------------
{
	m_Text = wxString::Format("o = [%.1f %.1f %.1f]  n = [%.1f %.1f %.1f]",m_SliceOrigin[0],m_SliceOrigin[1],m_SliceOrigin[2],m_SliceNormal[0],m_SliceNormal[1],m_SliceNormal[2]);
	m_TextMapper->SetInput(m_Text.GetCStr());
	m_TextMapper->Modified();
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::CameraUpdate()
//----------------------------------------------------------------------------
{
	Superclass::CameraUpdate();
  if (!m_GlobalBoundsValid)
  {
    mafOBB globalBounds;
    for(mafSceneNode *n = m_Sg->GetNodeList(); n; n = n->m_Next)
    {
      if(n->m_Pipe)
      {
        mafOBB b;
        ((mafVME *)n->m_Vme)->GetOutput()->GetVME4DBounds(b);
        globalBounds.MergeBounds(b);
      }
    }
    if (globalBounds.IsValid())
    {
      mafOBB previousGlobalBound;
      previousGlobalBound.DeepCopy(m_GlobalBounds);
      if(!previousGlobalBound.Equals(&globalBounds))
      {
        globalBounds.CopyTo(m_GlobalBounds);
        UpdateSliceParameters();
        UpdateSlice();
      }
    }
    m_GlobalBoundsValid = true;
  }

  /*if (m_NumberOfVisibleVme > 0)
	{
    mafNode *node = m_Sg->GetSelectedVme();
    mafOBB globalBounds;
    ((mafVME*)node->GetRoot())->GetOutput()->Get4DBounds(globalBounds);
    if(globalBounds.IsValid() && m_GlobalBoundsInitialized)
    {
      mafOBB b;
      b.DeepCopy(m_GlobalBounds);
      if(!b.Equals(&globalBounds))
      {
        ((mafVME*)node->GetRoot())->GetOutput()->Get4DBounds(b);
        //m_GlobalBoundsInitialized = false;
        if (b.IsValid())
        {
          b.CopyTo(m_GlobalBounds);
          // there is at least one VME visualized
          UpdateSliceParameters();
          UpdateSlice();
        }
      }
    }
	}*/
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	Superclass::VmeShow(node,show);

  m_SelectedVolume = m_Sg->Vme2Node(node);
  if (m_SelectedVolume->m_Pipe)
  {
    //m_Gui->Enable(ID_LUT,true);
    m_Gui->Enable(ID_POS_SLIDER,true);
    if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
    {
      m_Opacity   = ((mafPipeVolumeSlice *)m_SelectedVolume->m_Pipe)->GetSliceOpacity();
      m_Gui->Enable(ID_OPACITY_SLIDER,true);
    }
    m_Gui->Update();
  }
  else
  {
    m_Gui->Enable(ID_POS_SLIDER,false);
    m_Gui->Update();
  }
}