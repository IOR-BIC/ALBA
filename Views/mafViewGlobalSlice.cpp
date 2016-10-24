/*=========================================================================

 Program: MAF2
 Module: mafViewGlobalSlice
 Authors: Matteo Giacomoni, Simone Brazzale
 
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

#include "mafViewGlobalSlice.h"
#include "mafObserver.h"
#include "mafVME.h"
#include "mafSceneNode.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipeMeshSlice.h"
#include "mafAttachCamera.h"
#include "mafGUI.h"
#include "mafGUIFloatSlider.h"

#include "mafTransform.h"
#include "mafAbsMatrixPipe.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEVolume.h"
#include "mafVMEOutput.h"

#include "vtkCellPicker.h"
#include "vtkMAFRayCast3DPicker.h"
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
mafViewGlobalSlice::mafViewGlobalSlice(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo)
: mafViewVTK(label,camera_position,show_axes,show_grid, stereo)
//----------------------------------------------------------------------------
{
	m_SliceOrigin[0] = m_SliceOrigin[1] = m_SliceOrigin[2] = 0.0;
	m_IDPlane = ID_XY;
	m_SliceMode = mafPipeVolumeSlice::SLICE_ARB;
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

	m_NumberOfVmeInstantiated = 0;
	m_MapID.clear();

	m_BoundsPlane.clear();
	m_BoundsOutlineBox.clear();
	m_BoundsOutlineProperty.clear();
	m_BoundsOutlineActor.clear();
	m_BoundsOutlineMapper.clear();

  m_TrilinearInterpolationOn = TRUE;
}
//----------------------------------------------------------------------------
mafViewGlobalSlice::~mafViewGlobalSlice()
//----------------------------------------------------------------------------
{
	if(m_Rwi && m_TextActor)
		m_Rwi->m_RenFront->RemoveActor2D(m_TextActor);
	if(m_Rwi && (m_BoundsOutlineActor.size()>0))
  {
    for (int i = 0; i < m_BoundsOutlineActor.size(); i++)
    {
      if (m_BoundsOutlineActor.at(i))
      {
		    m_Rwi->m_RenFront->RemoveActor2D(m_BoundsOutlineActor.at(i));
      }
    }
  }

  for (int i = 0; i < m_BoundsPlane.size(); i++)
  {
    vtkDEL(m_BoundsPlane.at(i));
    m_BoundsPlane.at(i) = NULL;
  }
  for (int i = 0; i < m_BoundsOutlineBox.size(); i++)
  {
    vtkDEL(m_BoundsOutlineBox.at(i));
    m_BoundsOutlineBox.at(i) = NULL;
  }
  for (int i = 0; i < m_BoundsOutlineProperty.size(); i++)
  {
    vtkDEL(m_BoundsOutlineProperty.at(i));
    m_BoundsOutlineProperty.at(i) = NULL;
  }
  for (int i = 0; i < m_BoundsOutlineMapper.size(); i++)
  {
    vtkDEL(m_BoundsOutlineMapper.at(i));
    m_BoundsOutlineMapper.at(i) = NULL;
  }
  for (int i = 0; i < m_BoundsOutlineActor.size(); i++)
  {
    vtkDEL(m_BoundsOutlineActor.at(i));
    m_BoundsOutlineActor.at(i) = NULL;
  }
  m_BoundsPlane.clear();
  m_BoundsOutlineBox.clear();
  m_BoundsOutlineProperty.clear();
  m_BoundsOutlineMapper.clear();
  m_BoundsOutlineActor.clear();

  m_MapID.clear();

	vtkDEL(m_TextActor);
	vtkDEL(m_TextMapper);
}
//----------------------------------------------------------------------------
mafView *mafViewGlobalSlice::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewGlobalSlice *v = new mafViewGlobalSlice(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::Create()
//----------------------------------------------------------------------------
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  RWI_LAYERS num_layers = TWO_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack, m_Rwi->m_AlwaysVisibleRenderer);
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
  vtkPlaneSource* boundsPlane;
	vtkNEW(boundsPlane);
	boundsPlane->SetOrigin(m_GlobalBounds[0],m_GlobalBounds[2],m_SliceOrigin[2]);
	boundsPlane->SetPoint1(m_GlobalBounds[1],m_GlobalBounds[2],m_SliceOrigin[2]);
	boundsPlane->SetPoint2(m_GlobalBounds[0],m_GlobalBounds[3],m_SliceOrigin[2]);

	boundsPlane->Update();

	vtkOutlineCornerFilter* boundsOutlineBox;
  vtkNEW(boundsOutlineBox);
	boundsOutlineBox->SetInput(boundsPlane->GetOutput());  

	vtkPolyDataMapper* boundsOutlineMapper;
  vtkNEW(boundsOutlineMapper);
	boundsOutlineMapper->SetInput(boundsOutlineBox->GetOutput());

	vtkProperty* boundsOutlineProperty;
  vtkNEW(boundsOutlineProperty);
	boundsOutlineProperty->SetColor(1,1,1);
	boundsOutlineProperty->SetAmbient(1);
	boundsOutlineProperty->SetRepresentationToWireframe();
	boundsOutlineProperty->SetInterpolationToFlat();

  vtkActor* boundsOutlineActor;
  vtkNEW(boundsOutlineActor);
	boundsOutlineActor->SetMapper(boundsOutlineMapper);
	boundsOutlineActor->VisibilityOn();
	boundsOutlineActor->PickableOff();
	boundsOutlineActor->SetProperty(boundsOutlineProperty);

	m_Rwi->m_RenFront->AddActor(boundsOutlineActor);

  m_BoundsPlane.push_back(boundsPlane);
  m_BoundsOutlineBox.push_back(boundsOutlineBox);
  m_BoundsOutlineProperty.push_back(boundsOutlineProperty);
  m_BoundsOutlineMapper.push_back(boundsOutlineMapper);
  m_BoundsOutlineActor.push_back(boundsOutlineActor);

}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::VmeSelect(mafVME *vme,bool select)
//----------------------------------------------------------------------------
{
	assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
  if(select && m_Gui)
  {
    
    m_SelectedVolume = m_Sg->Vme2Node(vme);
		mafPipeVolumeSlice * volSlicePipe = (mafPipeVolumeSlice *)m_SelectedVolume->GetPipe();
    if (volSlicePipe)
    {
      //m_Gui->Enable(ID_LUT,true);
			m_Gui->Enable(ID_POS_SLIDER,true);
      if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
      {
        m_Opacity = volSlicePipe->GetSliceOpacity();
        m_Gui->Enable(ID_OPACITY_SLIDER,true);
        (volSlicePipe)->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
        m_Gui->Enable(ID_TRILINEAR_INTERPOLATION_ON,true);
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
void mafViewGlobalSlice::VmeCreatePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  std::pair<mafID,int> pair(vme->GetId(),m_NumberOfVmeInstantiated);
  m_MapID.insert(pair);
  
  m_NumberOfVmeInstantiated++;

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->GetPipe());

  m_GlobalBoundsValid = false; // new VME is shown into the view => Update the Global Bounds
	if (!m_GlobalBoundsInitialized || !m_GlobalBoundsValid)
  {
    vme->GetOutput()->GetVME4DBounds(m_GlobalBounds);
    UpdateSliceParameters();
		InizializePlane();
		//CameraUpdate();
  }

  double *new_point;
  double applied_origin[3];
  float applied_xVector[3];
  float applied_yVector[3];
  float applied_normal[3];
	
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
 
		m_NumberOfVisibleVme++;

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

			if (pipe_name.Equals("mafPipeVolumeSlice"))
			{
				((mafPipeVolumeSlice *)pipe)->HideSlider();
			}

			if (m_SelectedVolume == n && m_SelectedVolume->GetVme()->GetOutput()->IsA("mafVMEOutputVolume"))
			{
				m_Gui->Enable(ID_OPACITY_SLIDER,true);
				mafPipeVolumeSlice * volPipe = (mafPipeVolumeSlice *)m_SelectedVolume->GetPipe();
				m_Opacity=volPipe->GetSliceOpacity();
        m_Gui->Enable(ID_TRILINEAR_INTERPOLATION_ON,true);
        volPipe->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
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
void mafViewGlobalSlice::VmeDeletePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  m_GlobalBoundsValid = false;
  mafSceneNode *n = m_Sg->Vme2Node(vme);

	m_NumberOfVisibleVme--;

  if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
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
  if (m_Rwi && m_BoundsOutlineActor.size()>0)
  {
    int j = m_MapID[vme->GetId()];
    if (j!=-1 && j<m_BoundsOutlineActor.size() && m_BoundsOutlineActor.at(j))
    {
      m_Rwi->m_RenFront->RemoveActor2D(m_BoundsOutlineActor.at(j));
      m_MapID.erase(vme->GetId());
    }
  }
  assert(n && n->GetPipe());
	n->DeletePipe();
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
				mafPipeVolumeSlice* pipe = (mafPipeVolumeSlice *)m_SelectedVolume->GetPipe();
				if (pipe)
				{
					pipe->SetSliceOpacity(m_Opacity);
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
					m_OpacitySlider->SetValue(m_Opacity);
				}
			}
      break;
      case ID_TRILINEAR_INTERPOLATION_ON:
      {
        mafPipeVolumeSlice* pipe = (mafPipeVolumeSlice *)m_SelectedVolume->GetPipe();
        if (pipe)
        {
          pipe->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
          mafEventMacro(mafEvent(this, CAMERA_UPDATE));
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
				Superclass::OnEvent(maf_event);
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
mafGUI* mafViewGlobalSlice::CreateGui()
//----------------------------------------------------------------------------
{
	wxString Views[3] = {"XY","XZ","YZ"};

	assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();
	
	m_GlobalSlider = m_Gui->FloatSlider(ID_POS_SLIDER,"pos.",&m_SliderOrigin,m_GlobalBounds[4],m_GlobalBounds[5]);
	m_Gui->Combo(ID_CHANGE_VIEW,"view",&m_ViewIndex,3,Views);
	m_OpacitySlider = m_Gui->FloatSlider(ID_OPACITY_SLIDER,"opacity",&m_Opacity,0.1,1.0);

	bool Enable = false;
  mafVME *selVME = m_Sg->GetSelectedVme();
  if (selVME && selVME->GetOutput()->IsA("mafVMEOutputVolume") )
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

  m_Gui->Bool(ID_TRILINEAR_INTERPOLATION_ON,"Interpolation",&m_TrilinearInterpolationOn,1);
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

	for(mafSceneNode *node = m_Sg->GetNodeList(); node; node=node->GetNext())
	{
    if(node->GetPipe())
    {
			double *new_point;
			double applied_origin[3];
			float applied_xVector[3];
			float applied_yVector[3];
			float applied_normal[3];

			mafVME *vme = node->GetVme();
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

			mafVMEOutput * output = vme->GetOutput();

		  if(vme->IsA("mafVMESurface")||vme->IsA("mafVMESurfaceParametric")||vme->IsA("mafVMELandmarkCloud")||vme->IsA("mafVMELandmark"))
		  {
				mafPipeSurfaceSlice * pipe = (mafPipeSurfaceSlice *)node->GetPipe();
        pipe->SetSlice(m_SliceOrigin);
				double DoubleNormal[3];
				DoubleNormal[0]=(double)m_SliceNormal[0];
				DoubleNormal[1]=(double)m_SliceNormal[1];
				DoubleNormal[2]=(double)m_SliceNormal[2];
				pipe->SetNormal(DoubleNormal);
      }
      if(vme->IsA("mafVMEMesh"))
      {
				mafPipeMeshSlice * pipe = (mafPipeMeshSlice *)node->GetPipe();
        pipe->SetSlice(m_SliceOrigin);
        double DoubleNormal[3];
        DoubleNormal[0]=(double)m_SliceNormal[0];
        DoubleNormal[1]=(double)m_SliceNormal[1];
        DoubleNormal[2]=(double)m_SliceNormal[2];
        pipe->SetNormal(DoubleNormal);
      }
      else if (output->IsA("mafVMEOutputVolume"))
			{
				mafPipeVolumeSlice * pipe = (mafPipeVolumeSlice *)node->GetPipe();
				pipe->SetSlice(applied_origin, applied_xVector, applied_yVector);
				pipe->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
				//pipe->UpdateSlice();
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
  vtkPlaneSource* boundsPlane;
  if (m_BoundsPlane.size()!=0)
  {
    boundsPlane = m_BoundsPlane.at(m_BoundsPlane.size()-1);
  }
  else
  {
    boundsPlane = NULL;
  }
  vtkActor* boundsOutlineActor;
  if (m_BoundsOutlineActor.size()!=0)
  {
    boundsOutlineActor = m_BoundsOutlineActor.at(m_BoundsOutlineActor.size()-1);
  }
  else
  {
    boundsOutlineActor = NULL;
  }
  if(boundsPlane &&  boundsOutlineActor && m_Rwi)
	{
		switch(m_ViewIndex)
		{
		  case ID_XY:
			  boundsPlane->SetOrigin(m_GlobalBounds[0],m_GlobalBounds[2],m_SliceOrigin[2]);
			  boundsPlane->SetPoint1(m_GlobalBounds[1],m_GlobalBounds[2],m_SliceOrigin[2]);
			  boundsPlane->SetPoint2(m_GlobalBounds[0],m_GlobalBounds[3],m_SliceOrigin[2]);
			  boundsPlane->Update();
			break;
		  case ID_XZ:
			  boundsPlane->SetOrigin(m_GlobalBounds[0],m_SliceOrigin[1],m_GlobalBounds[4]);
			  boundsPlane->SetPoint1(m_GlobalBounds[1],m_SliceOrigin[1],m_GlobalBounds[4]);
			  boundsPlane->SetPoint2(m_GlobalBounds[0],m_SliceOrigin[1],m_GlobalBounds[5]);
			  boundsPlane->Update();
			break;
		  case ID_YZ:
			  boundsPlane->SetOrigin(m_SliceOrigin[0],m_GlobalBounds[2],m_GlobalBounds[4]);
			  boundsPlane->SetPoint1(m_SliceOrigin[0],m_GlobalBounds[3],m_GlobalBounds[4]);
			  boundsPlane->SetPoint2(m_SliceOrigin[0],m_GlobalBounds[2],m_GlobalBounds[5]);
			  boundsPlane->Update();
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
	if(m_SliceMode != mafPipeVolumeSlice::SLICE_ARB)
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
    for(mafSceneNode *n = m_Sg->GetNodeList(); n; n = n->GetNext())
    {
      if(n->GetPipe())
      {
        mafOBB b;
        n->GetVme()->GetOutput()->GetVME4DBounds(b);
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

}
//----------------------------------------------------------------------------
void mafViewGlobalSlice::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
	Superclass::VmeShow(vme,show);

  mafSceneNode *sceneNode = m_Sg->Vme2Node(vme);
	mafPipeVolumeSlice * selVolPipe = (mafPipeVolumeSlice *)m_SelectedVolume->GetPipe();
  if (sceneNode == m_SelectedVolume && selVolPipe)
  {
    //m_Gui->Enable(ID_LUT,true);
    m_Gui->Enable(ID_POS_SLIDER,true);
    if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
    {
      m_Opacity   = selVolPipe->GetSliceOpacity();
      selVolPipe->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
      m_Gui->Enable(ID_OPACITY_SLIDER,true);
      m_Gui->Enable(ID_TRILINEAR_INTERPOLATION_ON,true);
    }
    m_Gui->Update();
  }
  else if (sceneNode == m_SelectedVolume)
  {
    m_Gui->Enable(ID_POS_SLIDER,false);
    m_Gui->Update();
  }
}