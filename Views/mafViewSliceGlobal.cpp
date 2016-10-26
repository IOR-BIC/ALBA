/*=========================================================================

 Program: MAF2
 Module: mafViewSliceGlobal
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = false;

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUI.h"
#include "mafIndent.h"
#include "mafViewSliceGlobal.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafPipeMeshSlice.h"

#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafPipeMeshSlice.h"

#include "mafVMEVolumeGray.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include "mafAttachCamera.h"
#include "mafAbsMatrixPipe.h"
#include "mafGUIFloatSlider.h"
#include "mafTransform.h"

#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCamera.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSliceGlobal);
//----------------------------------------------------------------------------

enum AXIS_ID
{
  ID_Z = 0,
  ID_Y,
  ID_X,
};

enum TEXT_MODE
{
  SLICES_AND_FRAMES_LABEL = 0,
  COORDS_LABEL,
};

//----------------------------------------------------------------------------
mafViewSliceGlobal::mafViewSliceGlobal(wxString label /* =  */, int camera_position /* = CAMERA_CT */, bool show_axes /* = false */, bool show_grid /* = false */, int stereo /* = 0 */,bool showTICKs/* =false */,bool textureInterpolate/* =true */)
:mafViewSlice(label,camera_position,show_axes,show_grid, stereo,showTICKs,textureInterpolate)
//----------------------------------------------------------------------------
{
  m_GlobalBounds[0] = m_GlobalBounds[2] = m_GlobalBounds[4] = -1000.0;
  m_GlobalBounds[1] = m_GlobalBounds[3] = m_GlobalBounds[5] = 1000.0;

  m_SliderOldOrigin = 0.0; 
  m_SliderOrigin    = 0.0;

  m_Dn = 0.0;

  m_Opacity = 1.0;
  m_ViewIndex = ID_Z;

  m_GlobalSlider = NULL;
  m_OpacitySlider = NULL;

  m_GlobalBoundsInitialized = false;
  m_GlobalBoundsValid = true;

  m_NumberOfSlices[0] = m_NumberOfSlices[1] = m_NumberOfSlices[2] = 0;
  m_SliceIndex = 0;

  m_NumberOfFrames    = 1;
  m_FrameIndex        = 0;
  m_CurrentTimeStamp  = 0.0;

  m_TextMode = COORDS_LABEL;
  m_ShowText = 0;

  m_TextureInterpolate = textureInterpolate;

}
//----------------------------------------------------------------------------
mafViewSliceGlobal::~mafViewSliceGlobal()
//----------------------------------------------------------------------------
{
  BorderDelete();
  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
  m_CurrentSurface.clear();
  m_CurrentPolyline.clear();
  m_CurrentPolylineGraphEditor.clear();
  m_CurrentMesh.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewSliceGlobal::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSliceGlobal *v = new mafViewSliceGlobal(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType,m_ShowVolumeTICKs,m_TextureInterpolate);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::UpdateText(int ID)
//----------------------------------------------------------------------------
{
  m_Text = "";

  if (ID==1 && m_ShowText && m_CurrentVolume)
  {
    //UpdateSliceIndex();

    switch(m_CameraPositionId)
    {
    case CAMERA_OS_X:
         m_Text = "X = ";
      break;
    case CAMERA_OS_Y:
      m_Text = "Y = ";
      break;
    case CAMERA_OS_P:
      m_Text = "Z = ";
      break;
    case CAMERA_PERSPECTIVE:
      m_Text = "";
      break;
    case CAMERA_ARB:
      m_Text = "";
      break;
    default:
      m_Text = "Z = ";
    }

    mafTimeStamp time(0.0);
    int numberOfSlices = 0;

    if(m_ViewIndex == 0)
      numberOfSlices = m_NumberOfSlices[2];
    if(m_ViewIndex == 1)
      numberOfSlices = m_NumberOfSlices[1];
    if(m_ViewIndex == 2)
      numberOfSlices = m_NumberOfSlices[0];

    mafVME *vme = m_CurrentVolume->GetVme();
    if(vme)
    {
      if(m_TextMode == SLICES_AND_FRAMES_LABEL)
      {

      }
      std::vector<mafTimeStamp> frames;
      vme->GetLocalTimeStamps(frames);
      m_CurrentTimeStamp = vme->GetTimeStamp();

      if(frames.size() == 1 || m_CurrentTimeStamp < frames[1])
      {
        m_FrameIndex = 0;
      }

      else {
        for(int i=m_FrameIndex;i<frames.size();i++)
        {
          if(m_CurrentTimeStamp<=frames[i])
          {
            m_FrameIndex = i;
            break;
          }
        }
      }

    }

    if(m_TextMode == SLICES_AND_FRAMES_LABEL)
    {
      m_Text = wxString::Format("SLICE %d of %d FRAME: %d of %d",m_SliceIndex+1, numberOfSlices, m_FrameIndex+1, m_NumberOfFrames);
    }
    else
    {
      if((m_CameraPositionId != CAMERA_ARB) && (m_CameraPositionId != CAMERA_PERSPECTIVE))
        m_Text = wxString::Format("o = [%.1f %.1f %.1f]  n = [%.1f %.1f %.1f]",m_Slice[0],m_Slice[1],m_Slice[2],m_SliceNormal[0],m_SliceNormal[1],m_SliceNormal[2]);
    }
  }
  else
  {
    m_Text="";
  }

  m_TextMapper->SetInput(m_Text.c_str());
  m_TextMapper->Modified();
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::VmeCreatePipe(mafVME *vme)
//----------------------------------------------------------------------------
{ 
  Superclass::VmeCreatePipe(vme);

  m_GlobalBoundsValid = false; // new VME is shown into the view => Update the Global Bounds
  if (!m_GlobalBoundsInitialized)
  {
    UpdateBounds();
    UpdateSliceParameters();
  }
 
  CameraUpdate();

}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::VmeDeletePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);

	m_NumberOfVisibleVme--;

  if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (m_CurrentVolume == n)
    {
      m_Gui->Enable(ID_OPACITY_SLIDER,false);
      m_Gui->Update();
      m_CurrentVolume = NULL;
      if (m_AttachCamera)
      {
        m_AttachCamera->SetVme(NULL);
      }
    }
  }
  assert(n && n->GetPipe());
	n->DeletePipe();

  if(vme->IsMAFType(mafVMELandmark))
    UpdateSurfacesList(vme);

  m_GlobalBoundsValid = false;

}

//-------------------------------------------------------------------------
mafGUI *mafViewSliceGlobal::CreateGui()
//-------------------------------------------------------------------------
{
  wxString Views[3] = {"Z","Y","X"};

  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

  m_GlobalSlider = m_Gui->FloatSlider(ID_POS_SLIDER,"pos.",&m_SliderOrigin,m_GlobalBounds[4],m_GlobalBounds[5]);
  m_Gui->Combo(ID_CHANGE_VIEW,"view",&m_ViewIndex,3,Views);
  m_OpacitySlider = m_Gui->FloatSlider(ID_OPACITY_SLIDER,"opacity",&m_Opacity,0.1,1.0);

  m_Gui->Bool(ID_SHOW_TEXT, "Show text", &m_ShowText);

  bool Enable = false;
  mafVME *selVME = m_Sg->GetSelectedVme();
  if (m_CurrentVolume)
  {
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
void mafViewSliceGlobal::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(maf_event->GetId()) 
    { 
    case ID_OPACITY_SLIDER:
      {
          mafPipeVolumeSlice::SafeDownCast(m_CurrentVolume->GetPipe())->SetSliceOpacity(m_Opacity);
          mafEventMacro(mafEvent(this, CAMERA_UPDATE));
          m_OpacitySlider->SetValue(m_Opacity);
      }
      break;
    case ID_POS_SLIDER:
      {
        m_Dn = m_SliderOrigin - m_SliderOldOrigin;
        UpdateSlice();
        m_SliderOldOrigin = m_SliderOrigin;
        UpdateSliceIndex();
        break;
      }
    case ID_CHANGE_VIEW:
      {
        ChangeView(m_ViewIndex);
        break;
      }
    case ID_SHOW_TEXT:
      {
        CameraUpdate();
      }
      break;
    default:
      mafViewSlice::OnEvent(maf_event);
    }
  }
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::UpdateSlice()
//----------------------------------------------------------------------------
{
  if(!mafEquals(m_Dn, 0.0))
  {
    double origin[3];
    origin[0] = m_Slice[0] + abs(m_SliceNormal[0]) * m_Dn;
    origin[1] = m_Slice[1] + abs(m_SliceNormal[1]) * m_Dn;
    origin[2] = m_Slice[2] + abs(m_SliceNormal[2]) * m_Dn;

    if(origin[0] > m_GlobalBounds[1])
      origin[0] = m_GlobalBounds[1];
    if(origin[0] < m_GlobalBounds[0])
      origin[0] = m_GlobalBounds[0];
    if(origin[1] > m_GlobalBounds[3])
      origin[1] = m_GlobalBounds[3];
    if(origin[1] < m_GlobalBounds[2])
      origin[1] = m_GlobalBounds[2];
    if(origin[2] > m_GlobalBounds[5])
      origin[2] = m_GlobalBounds[5];
    if(origin[2] < m_GlobalBounds[4])
      origin[2] = m_GlobalBounds[4];

    SetSlice(origin, NULL);

    CameraUpdate();
  }

  m_Dn = 0.0;
}

//----------------------------------------------------------------------------
double mafViewSliceGlobal::UpdateSlicePos(double pos)
//----------------------------------------------------------------------------
{
  //pos = [0;1]
  m_SliderOrigin = pos*(m_GlobalSlider->GetMax() - m_GlobalSlider->GetMin()) + m_GlobalSlider->GetMin();
  m_Dn = m_SliderOrigin - m_SliderOldOrigin;
  UpdateSlice();
  m_SliderOldOrigin = m_SliderOrigin;
  m_GlobalSlider->SetValue(m_SliderOrigin);

  if(m_ViewIndex == 0)
    m_SliceIndex = pos*(m_NumberOfSlices[2]-1);
  if(m_ViewIndex == 1)
    m_SliceIndex = pos*(m_NumberOfSlices[1]-1);
  if(m_ViewIndex == 2)
    m_SliceIndex = pos*(m_NumberOfSlices[0]-1);

  return m_SliderOrigin;
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::UpdateSliceIndex()
//----------------------------------------------------------------------------
{
  double pos = (m_SliderOrigin - m_GlobalSlider->GetMin()) / (m_GlobalSlider->GetMax() - m_GlobalSlider->GetMin());

  if(m_ViewIndex == 0)
    m_SliceIndex = pos* (m_NumberOfSlices[2]-1);
  if(m_ViewIndex == 1)
    m_SliceIndex = pos* (m_NumberOfSlices[1]-1);
  if(m_ViewIndex == 2)
    m_SliceIndex = pos* (m_NumberOfSlices[0]-1);
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::ChangeView(int viewIndex)
//----------------------------------------------------------------------------
{
  m_ViewIndex = viewIndex;

  double normals[3];
  normals[0] = normals[1] = normals[2] = 0.0;
  
  UpdateSliceParameters();

  int newCameraPositionId = m_CameraPositionId;

  if(m_ViewIndex == ID_Z)
  {
    normals[2] = 1.0;
    newCameraPositionId = CAMERA_OS_Z;
  }
  else if(m_ViewIndex == ID_Y)
  {
    normals[1] = 1.0;
    newCameraPositionId = CAMERA_OS_Y;
  }
  else if(m_ViewIndex == ID_X)
  {
    normals[0] = 1.0;
    newCameraPositionId = CAMERA_OS_X;
  }

  if(m_CameraPositionId == CAMERA_OS_X ||
    m_CameraPositionId == CAMERA_OS_Y ||
    m_CameraPositionId == CAMERA_OS_Z)
    m_CameraPositionId = newCameraPositionId;


  CameraSet(m_CameraPositionId);
  SetSlice(m_Slice, normals);
  mafMatrix m;
  m_OldABSPose = m; // forcing parallel camera position in case of rotated volumes
  CameraReset();
  CameraUpdate();

}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::SetTextMode(int textMode)
//----------------------------------------------------------------------------
{
  m_TextMode = textMode;

  if(m_TextMode<0)
    m_TextMode = 0;
  if(m_TextMode>1)
    m_TextMode = 1;
}
//----------------------------------------------------------------------------
void mafViewSliceGlobal::ShowText(bool show)
//----------------------------------------------------------------------------
{
  m_ShowText = show;
  CameraUpdate();
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::VmeSelect(mafVME *vme,bool select)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  Superclass::VmeShow(vme,show);

  m_CurrentVolume = m_Sg->Vme2Node(vme);

  if(m_CurrentVolume && m_CurrentVolume->GetVme())
  {
    vtkDataSet *dataSet =m_CurrentVolume->GetVme()->GetOutput()->GetVTKData();

    if(NULL!=dataSet)
    {
      if(dataSet->IsA("vtkRectilinearGrid")) {
        vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::SafeDownCast(dataSet);
        rectilinearGrid->GetDimensions(m_NumberOfSlices);
      }
      else if (dataSet->IsA("vtkImageData")) {
        vtkImageData *imageData=vtkImageData::SafeDownCast(dataSet);
        imageData->GetDimensions(m_NumberOfSlices);
      }
    }

    m_NumberOfFrames = m_CurrentVolume->GetVme()->GetNumberOfLocalTimeStamps();


		mafPipe * curVolPipe = m_CurrentVolume->GetPipe();
    if (curVolPipe)
    {
      m_Gui->Enable(ID_POS_SLIDER,true);
      if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
      {
        m_Opacity   = mafPipeVolumeSlice::SafeDownCast(curVolPipe)->GetSliceOpacity();
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
  m_FrameIndex = 0;

}

//-------------------------------------------------------------------------
void mafViewSliceGlobal::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "mafViewSliceGlobal" << '\t' << this << std::endl;
  os << indent << "Name" << '\t' << m_Label << std::endl;
  os << std::endl;
  m_Sg->Print(os,1);
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::CameraUpdate()
//----------------------------------------------------------------------------
{  
  if (m_CurrentVolume &&  mafVMEVolumeGray::SafeDownCast(m_CurrentVolume->GetVme()))
  {
    mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(m_CurrentVolume->GetVme());

    std::ostringstream stringStream;
    stringStream << "VME " << volume->GetName() << " ABS matrix:" << std::endl;

    volume->GetAbsMatrixPipe()->GetMatrixPointer()->Print(stringStream);

    if (!m_GlobalBoundsValid)
    {
      UpdateBounds();
      UpdateSliceParameters();
      UpdateSlice();

      m_GlobalBoundsValid = true;
    }

    m_NewABSPose = volume->GetAbsMatrixPipe()->GetMatrix();

    // Fix bug #2192: Added by Losi 07/07/2010
    // Avoid pan & zoom reset while changing timestamp
    mafMatrix oldABSPoseForEquals;
    oldABSPoseForEquals.DeepCopy(&m_OldABSPose);
    oldABSPoseForEquals.SetTimeStamp(m_NewABSPose.GetTimeStamp());

    if (m_NewABSPose.Equals(&oldABSPoseForEquals))
    { 
      if (DEBUG_MODE == true)
        mafLogMessage("Calling Superclass Camera Update ");

      mafViewVTK::CameraUpdate();
    }
    else
    {
      if (DEBUG_MODE == true)
        mafLogMessage("Calling Rotated Volumes Camera Update ");
      m_OldABSPose = m_NewABSPose;
      CameraUpdateForRotatedVolumes();
    }
  }
  else
  {

    if (DEBUG_MODE == true)
      mafLogMessage("Calling Superclass Camera Update ");

    mafViewVTK::CameraUpdate();
  }

  UpdateText(1);

}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::UpdateSliceParameters()
//----------------------------------------------------------------------------
{
  if(!m_GlobalBoundsInitialized)
  {
    m_Slice[0] = (m_GlobalBounds[0] + m_GlobalBounds[1]) * .5;
    m_Slice[1] = (m_GlobalBounds[2] + m_GlobalBounds[3]) * .5;
    m_Slice[2] = (m_GlobalBounds[4] + m_GlobalBounds[5]) * .5;
  }

  double new_bounds[3] = {0.0,0.0,0.0};
  int index;
  switch(m_ViewIndex)
  {
  case ID_Z:
    index = 2;
    new_bounds[0] = m_GlobalBounds[4];
    new_bounds[1] = m_GlobalBounds[5];
    new_bounds[2] = (m_GlobalBounds[4] + m_GlobalBounds[5]) * .5;
    if(!m_GlobalBoundsInitialized)
      m_Slice[2] = new_bounds[2];
    break;
  case ID_Y:
    index = 1;
    new_bounds[0] = m_GlobalBounds[2];
    new_bounds[1] = m_GlobalBounds[3];
    new_bounds[2] = (m_GlobalBounds[2] + m_GlobalBounds[3]) * .5;
    if(!m_GlobalBoundsInitialized)
      m_Slice[1] = new_bounds[2];
    break;
  case ID_X:
    index = 0;
    new_bounds[0] = m_GlobalBounds[0];
    new_bounds[1] = m_GlobalBounds[1];
    new_bounds[2] = (m_GlobalBounds[0] + m_GlobalBounds[1]) * .5;
    if(!m_GlobalBoundsInitialized)
      m_Slice[0] = new_bounds[2];
    break;
  }
  if(m_GlobalSlider)
  {
    m_GlobalSlider->SetNumberOfSteps((new_bounds[1] - new_bounds[0]) * 10);
    m_GlobalSlider->SetRange(new_bounds[0], new_bounds[1], m_Slice[index]);
    m_GlobalSlider->SetValue(m_Slice[index]);
    m_GlobalSlider->Update();

    m_SliderOldOrigin = m_GlobalSlider->GetValue();
    m_SliderOrigin = m_GlobalSlider->GetValue();
  }

  m_GlobalBoundsInitialized = true;

  UpdateText();
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::UpdateBounds()
//----------------------------------------------------------------------------
{
  mafOBB globalBounds;
  for(mafSceneNode *n = m_Sg->GetNodeList(); n; n = n->GetNext())
  {
    if(n->GetPipe())
    {
      double b[6];
      n->GetVme()->GetOutput()->GetVTKData()->GetBounds(b);
      globalBounds.MergeBounds(mafOBB(b));
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

}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::SetSlice(double origin[3], float xVect[3], float yVect[3])
//----------------------------------------------------------------------------
{
  double X[3];
  double Y[3];

  for(int i=0;i<3;i++)
  {
    X[i] = xVect[i];
    Y[i] = yVect[i];
  }

  vtkMath::Normalize(X);
  vtkMath::Normalize(Y);
  vtkMath::Cross(Y, X, m_SliceNormal);
  vtkMath::Normalize(m_SliceNormal);

  Superclass::SetSlice(origin);

}

//-------------------------------------------------------------------------
int mafViewSliceGlobal::GetNodeStatus(mafVME *vme)
//-------------------------------------------------------------------------
{
   return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}

//----------------------------------------------------------------------------
void mafViewSliceGlobal::SetSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{

  //set slice origin and normal
  if (Origin != NULL)
  {
    memcpy(m_Slice,Origin,sizeof(m_Slice));
    m_LastSliceOrigin[0] = m_Slice[0];
    m_LastSliceOrigin[1] = m_Slice[1];
    m_LastSliceOrigin[2] = m_Slice[2];

  }

  if (Normal != NULL)
  {
    memcpy(m_SliceNormal,Normal,sizeof(m_Slice));
    m_LastSliceNormal[0] = m_SliceNormal[0];
    m_LastSliceNormal[1] = m_SliceNormal[1];
    m_LastSliceNormal[2] = m_SliceNormal[2];
  }

  for(mafSceneNode *node = m_Sg->GetNodeList(); node; node=node->GetNext())
  {
		mafPipe * pipe = node->GetPipe();
    if(pipe && mafPipeVolumeSlice::SafeDownCast(pipe) )
    {
      mafPipeVolumeSlice::SafeDownCast(pipe)->SetSlice(Origin, Normal); 
    }
  }

  double normal[3];
  if (Normal != NULL)
    memcpy(normal,Normal,sizeof(m_Slice));
  else
    this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);

  double coord[3];
  coord[0]=Origin[0];
  coord[1]=Origin[1];
  coord[2]=Origin[2];
  MultiplyPointByInputVolumeABSMatrix(coord);

  for(int i = 0; i < m_CurrentSurface.size(); i++)
  {
		mafPipe * curSurfPipe = m_CurrentSurface.at(i)->GetPipe();
    if (curSurfPipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(curSurfPipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }
  }	


  for(int i = 0;i < m_CurrentPolyline.size();i++)
  {
		mafPipe * curPolylinePipe = m_CurrentPolyline.at(i)->GetPipe();
    if(curPolylinePipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(curPolylinePipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }
  }	

  for(int i = 0; i < m_CurrentPolylineGraphEditor.size();i++)
  {
		mafPipe * curPGEPipe = m_CurrentPolylineGraphEditor.at(i)->GetPipe();
    if (curPGEPipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(curPGEPipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }   
  }

  for(int i = 0; i < m_CurrentMesh.size();i++)
  {
		mafPipe * curMeshPipe = m_CurrentMesh.at(i)->GetPipe();
    if (curMeshPipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(curMeshPipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }   
  }

  // update text
  this->UpdateText();
}