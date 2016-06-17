/*=========================================================================

 Program: MAF2
 Module: mafViewSlice
 Authors: Paolo Quadrani , Stefano Perticoni , Josef Kohout
 
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
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice_BES.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafPipeMeshSlice_BES.h"

#include "mafPipeSurfaceSlice.h"
#include "mafPipePolylineSlice.h"
#include "mafPipeMeshSlice.h"

#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"
#include "mafPipePolylineGraphEditor.h"
#include "mafTransform.h"
#include "mafAbsMatrixPipe.h"

#include "vtkDataSet.h"
#include "vtkMAFRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSlice);
//----------------------------------------------------------------------------

#include "mafMemDbg.h"

const int LAST_SLICE_ORIGIN_VALUE_NOT_INITIALIZED = 0;

//----------------------------------------------------------------------------
mafViewSlice::mafViewSlice(wxString label /* =  */, int camera_position /* = CAMERA_CT */, bool show_axes /* = false */, bool show_grid /* = false */,  int stereo /* = 0 */,bool showTICKs/* =false */,bool textureInterpolate/* =true */)
:mafViewVTK(label,camera_position,show_axes,show_grid, stereo)
//----------------------------------------------------------------------------
{
  m_CurrentVolume = NULL;
  m_Border        = NULL;
  
  m_Slice[0] = m_Slice[1] = m_Slice[2] = 0.0;
  InitializeSlice(m_Slice);     //to correctly set the normal  
  m_SliceInitialized = false;   //reset initialized to false 

  m_TextActor=NULL;
  m_TextMapper=NULL;
  m_TextColor[0]=1;
  m_TextColor[1]=0;
  m_TextColor[2]=0;

  m_CurrentSurface.clear();
	m_CurrentPolyline.clear();
	m_CurrentPolylineGraphEditor.clear();
  m_CurrentMesh.clear();

	m_ShowVolumeTICKs = showTICKs;

  m_LastSliceOrigin[0] = m_LastSliceOrigin[1] = m_LastSliceOrigin[2] = LAST_SLICE_ORIGIN_VALUE_NOT_INITIALIZED;
  m_LastSliceNormal[0] = m_LastSliceNormal[1] = m_LastSliceNormal[2] = 0.0;

  // Added by Losi 11.25.2009
  m_EnableGPU=FALSE;
  m_TrilinearInterpolationOn = TRUE;
  m_TextureInterpolate = textureInterpolate;

}
//----------------------------------------------------------------------------
mafViewSlice::~mafViewSlice()
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
mafView *mafViewSlice::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSlice *v = new mafViewSlice(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType,m_ShowVolumeTICKs,m_TextureInterpolate);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewSlice::Create()
//----------------------------------------------------------------------------
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  RWI_LAYERS num_layers = m_CameraPositionId != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack,m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();

  // text stuff
  m_Text = "";
  m_TextMapper = vtkTextMapper::New();
  m_TextMapper->SetInput(m_Text.c_str());
  m_TextMapper->GetTextProperty()->AntiAliasingOff();

  m_TextActor = vtkActor2D::New();
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->SetPosition(3,3);
  m_TextActor->GetProperty()->SetColor(m_TextColor);

  m_Rwi->m_RenFront->AddActor(m_TextActor);
}



//----------------------------------------------------------------------------
void mafViewSlice::SetTextColor(double color[3])
//----------------------------------------------------------------------------
{
  m_TextColor[0]=color[0];
  m_TextColor[1]=color[1];
  m_TextColor[2]=color[2];
  m_TextActor->GetProperty()->SetColor(m_TextColor);
  m_TextMapper->Modified();
}
//----------------------------------------------------------------------------
void mafViewSlice::UpdateText(int ID)
//----------------------------------------------------------------------------
{
  m_Text = "";

  if (ID==1)
  {
    int slice_mode;
    switch(m_CameraPositionId)
    {
    case CAMERA_OS_X:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_X;
      break;
    case CAMERA_OS_Y:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_Y;
      break;
    case CAMERA_OS_P:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_ORTHO;
      break;
    case CAMERA_PERSPECTIVE:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_ARB;
      break;
		case CAMERA_ARB:
			slice_mode = mafPipeVolumeSlice_BES::SLICE_ARB;
			break;
    default:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_Z;
    }
    //set the init coordinates value
    if(slice_mode == mafPipeVolumeSlice_BES::SLICE_X)
      m_Text = "X = ";
    else if(slice_mode == mafPipeVolumeSlice_BES::SLICE_Y)
      m_Text = "Y = ";
    else if(slice_mode == mafPipeVolumeSlice_BES::SLICE_Z)
      m_Text = "Z = ";

    if((slice_mode != mafPipeVolumeSlice_BES::SLICE_ORTHO) && (slice_mode != mafPipeVolumeSlice_BES::SLICE_ARB))
      m_Text += wxString::Format("%.1f",m_Slice[slice_mode]);

    m_TextMapper->SetInput(m_Text.c_str());
    m_TextMapper->Modified();
  }
  else
  {
    m_Text="";
    m_TextMapper->SetInput(m_Text.c_str());
    m_TextMapper->Modified();
  }
}

//----------------------------------------------------------------------------
//Give an initial origin and normal (optionally) for the slice.
void mafViewSlice::InitializeSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{
  memcpy(m_Slice,Origin,sizeof(m_Slice));
  if (Normal != NULL) {
    memcpy(m_SliceNormal,Normal,sizeof(m_SliceNormal));
  }
  else
  {
    if (m_CameraPositionId == CAMERA_ARB)
      this->GetRWI()->GetCamera()->GetViewPlaneNormal(m_SliceNormal);
    else
    {
      m_SliceNormal[0] = m_SliceNormal[1] = m_SliceNormal[2] = 0.0;

      switch(m_CameraPositionId)
      {    
      case CAMERA_OS_X:
        m_SliceNormal[0] = 1;      
        break;
      case CAMERA_OS_Y:      
        m_SliceNormal[1] = 1;      
        break;
        //case CAMERA_OS_Z:  
      default:
        m_SliceNormal[2] = 1;
        break;
      }
    }
  }

  m_SliceInitialized = true;
}
//----------------------------------------------------------------------------
void mafViewSlice::VmeCreatePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->GetPipe());

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;

		mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe != NULL)
    {
      pipe->SetListener(this);
      if (pipe->IsA("mafPipeVolumeSlice_BES"))  //BES: 3.4.2009 - changed to support inheritance
      {
        m_CurrentVolume = n;

        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->GetVme());
        int slice_mode;
        vtkDataSet *data = vme->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        switch(m_CameraPositionId)
        {
        case CAMERA_OS_X:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_X;
          break;
        case CAMERA_OS_Y:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_Y;
          break;
        case CAMERA_OS_P:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_ORTHO;
          break;
        case CAMERA_PERSPECTIVE:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_ARB;
          break;
        default:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_Z;
        }
        if (m_SliceInitialized)
        {
          ((mafPipeVolumeSlice_BES *)pipe)->InitializeSliceParameters(slice_mode, m_Slice, false,false,m_TextureInterpolate);
          ((mafPipeVolumeSlice_BES *)pipe)->SetNormal(m_SliceNormal);
        }
        else
        {
          ((mafPipeVolumeSlice_BES *)pipe)->InitializeSliceParameters(slice_mode,false,false,m_TextureInterpolate);
        }

        if(m_ShowVolumeTICKs)
          ((mafPipeVolumeSlice_BES *)pipe)->ShowTICKsOn();
        else
          ((mafPipeVolumeSlice_BES *)pipe)->ShowTICKsOff();
        ((mafPipeVolumeSlice_BES *)pipe)->SetEnableGPU(m_EnableGPU);
        ((mafPipeVolumeSlice_BES *)pipe)->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
        UpdateText();
      }
      else 
      {
        //not a VolumeSlice pipe, check, if it is some slicer
        mafPipeSlice* spipe = mafPipeSlice::SafeDownCast(pipe);
        if (spipe != NULL)
        { 
          //it is slicing pipe
          //initialize supported slicing pipes
          if(pipe->IsA("mafPipeSurfaceSlice_BES")){
            m_CurrentSurface.push_back(n);        
          }
          else if(pipe->IsA("mafPipePolylineSlice_BES")){
            m_CurrentPolyline.push_back(n);        
          }
          else if(pipe->IsA("mafPipePolylineGraphEditor"))
          {
            m_CurrentPolylineGraphEditor.push_back(n);

            if(m_CameraPositionId==CAMERA_OS_P)
              ((mafPipePolylineGraphEditor *)pipe)->SetModalityPerspective();
            else
              ((mafPipePolylineGraphEditor *)pipe)->SetModalitySlice();				
          }
          else if(pipe->IsA("mafPipeMeshSlice_BES"))  {
            m_CurrentMesh.push_back(n);        
          }

          //common stuff
          double positionSlice[3];
          positionSlice[0] = m_Slice[0];
          positionSlice[1] = m_Slice[1];
          positionSlice[2] = m_Slice[2];
          MultiplyPointByInputVolumeABSMatrix(positionSlice);
          spipe->SetSlice(positionSlice, m_SliceNormal);
        }
        else
        {
          //BES: 12.6.2009 - TODO: this code should be removed when
          //mafPipeSurfaceSlice_BES, mafPipePolylineSlice_BES and mafPipeMeshSlice_BES
          //are committed down and instead of it, the code above should work (after _BES suffices are stripped)

          if(pipe_name.Equals("mafPipeSurfaceSlice"))
          {
            double normal[3];
            switch(m_CameraPositionId)
            {
            case CAMERA_OS_X:
              normal[0] = 1;
              normal[1] = 0;
              normal[2] = 0;
              break;
            case CAMERA_OS_Y:
              normal[0] = 0;
              normal[1] = 1;
              normal[2] = 0;
              break;
            case CAMERA_OS_Z:
              normal[0] = 0;
              normal[1] = 0;
              normal[2] = 1;
              break;
            case CAMERA_OS_P:
              break;
              //case CAMERA_OS_REP:
              //	this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
            case CAMERA_PERSPECTIVE:
              break;
            default:
              normal[0] = 0;
              normal[1] = 0;
              normal[2] = 1;
            }
            m_CurrentSurface.push_back(n);

            double positionSlice[3];
            positionSlice[0] = m_Slice[0];
            positionSlice[1] = m_Slice[1];
            positionSlice[2] = m_Slice[2];
            MultiplyPointByInputVolumeABSMatrix(positionSlice);
            ((mafPipeSurfaceSlice *)pipe)->SetSlice(positionSlice);
            ((mafPipeSurfaceSlice *)pipe)->SetNormal(normal);

          }
          else if(pipe_name.Equals("mafPipePolylineSlice"))
          {
            double normal[3];
            switch(m_CameraPositionId)
            {
            case CAMERA_OS_X:
              normal[0] = 1;
              normal[1] = 0;
              normal[2] = 0;
              break;
            case CAMERA_OS_Y:
              normal[0] = 0;
              normal[1] = 1;
              normal[2] = 0;
              break;
            case CAMERA_OS_Z:
              normal[0] = 0;
              normal[1] = 0;
              normal[2] = 1;
              break;
            case CAMERA_OS_P:
              break;
            case CAMERA_ARB:
              this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
              break;
            case CAMERA_PERSPECTIVE:
              //this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
              break;
            default:
              normal[0] = 0;
              normal[1] = 0;
              normal[2] = 1;
            }
            m_CurrentPolyline.push_back(n);
            double positionSlice[3];
            positionSlice[0] = m_Slice[0];
            positionSlice[1] = m_Slice[1];
            positionSlice[2] = m_Slice[2];
            MultiplyPointByInputVolumeABSMatrix(positionSlice);
            ((mafPipePolylineSlice *)pipe)->SetSlice(positionSlice);
            ((mafPipePolylineSlice *)pipe)->SetNormal(normal);
          }          
          else if(pipe_name.Equals("mafPipeMeshSlice"))
          {
            double normal[3];
            switch(m_CameraPositionId)
            {
            case CAMERA_OS_X:
              normal[0] = 1;
              normal[1] = 0;
              normal[2] = 0;
              break;
            case CAMERA_OS_Y:
              normal[0] = 0;
              normal[1] = 1;
              normal[2] = 0;
              break;
            case CAMERA_OS_Z:
              normal[0] = 0;
              normal[1] = 0;
              normal[2] = 1;
              break;
            case CAMERA_OS_P:
              break;
            case CAMERA_ARB:
              this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
              break;
            case CAMERA_PERSPECTIVE:
              //this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
              break;
            default:
              normal[0] = 0;
              normal[1] = 0;
              normal[2] = 1;
            }
            m_CurrentMesh.push_back(n);
            double positionSlice[3];
            positionSlice[0] = m_Slice[0];
            positionSlice[1] = m_Slice[1];
            positionSlice[2] = m_Slice[2];
            MultiplyPointByInputVolumeABSMatrix(positionSlice);
            ((mafPipeMeshSlice *)pipe)->SetSlice(positionSlice);
            ((mafPipeMeshSlice *)pipe)->SetNormal(normal);
          }
        }
      } //end else [it is not volume slicing]                     

      pipe->Create(n);
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}

//----------------------------------------------------------------------------
void mafViewSlice::VmeDeletePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);

	m_NumberOfVisibleVme--;
  
  if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    m_CurrentVolume = NULL;
    if (m_AttachCamera)
    {
      m_AttachCamera->SetVme(NULL);
    }
  }
  assert(n && n->GetPipe());
	n->DeletePipe();

  if(vme->IsMAFType(mafVMELandmark))
    UpdateSurfacesList(vme);
}
//-------------------------------------------------------------------------
int mafViewSlice::GetNodeStatus(mafVME *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    n = m_Sg->Vme2Node(vme);
     if (vme->GetOutput()->IsA("mafVMEOutputVolume") || 
         vme->IsMAFType(mafVMESlicer))
    {
      if (n != NULL)
      {
      	n->SetMutex(true);
      }
    }
    else if (vme->IsMAFType(mafVMEImage))
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
mafGUI *mafViewSlice::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());

  // Added by Losi 11.25.2009
  if (m_CurrentVolume)
  {
    mafPipeVolumeSlice_BES *p = NULL;
    p = mafPipeVolumeSlice_BES::SafeDownCast(this->GetNodePipe(m_CurrentVolume->GetVme())); // m_CurrentVolume->m_Pipe is better?
    if (p) // Is this required?
    {
      p->SetEnableGPU(m_EnableGPU);
      p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
    }
  };
   m_Gui->Divider(1);
  //m_Gui->Bool(ID_ENABLE_GPU,"Enable GPU",&m_EnableGPU,1);
   m_Gui->Bool(ID_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1);

	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // Added by Losi 11.25.2009
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(maf_event->GetId()) 
    {
      case ID_ENABLE_GPU:
        {
          if (m_CurrentVolume)
          {
            mafPipeVolumeSlice_BES *p = NULL;
            p = mafPipeVolumeSlice_BES::SafeDownCast(this->GetNodePipe(m_CurrentVolume->GetVme()));
            if(p)
            {
              p->SetEnableGPU(m_EnableGPU);
              this->CameraUpdate();
            }
          }
        }
        break;
      case ID_TRILINEAR_INTERPOLATION:
        {
          if (m_CurrentVolume)
          {
            mafPipeVolumeSlice_BES *p = NULL;
            p = mafPipeVolumeSlice_BES::SafeDownCast(this->GetNodePipe(m_CurrentVolume->GetVme()));
            if(p)
            {
              p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
              this->CameraUpdate();
            }
          }
        }
        break;
    }
  }

	Superclass::OnEvent(maf_event);
}

//----------------------------------------------------------------------------
void mafViewSlice::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  
  mafPipeVolumeSlice_BES *pipe = mafPipeVolumeSlice_BES::SafeDownCast(m_CurrentVolume->GetPipe());
  if (pipe != NULL) {    
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewSlice::SetSlice(double* Origin, double* Normal)
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
  
  //and now set it for every VME
  if (m_CurrentVolume)
	{
    mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(m_CurrentVolume->GetPipe());
		if (pipe != NULL)
		{
			pipe->SetSlice(Origin, Normal); 

			// update text
			this->UpdateText();
		}
	}
 
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
        pipe->SetSlice(coord, m_SliceNormal); 
      }
      else
      {
        //BES: 12.6.2009 - TODO: this branch should be removed when mafPipeSurfaceSlice_BES committed down
        mafPipeSurfaceSlice* pipe = mafPipeSurfaceSlice::SafeDownCast(curSurfPipe);
        if (pipe != NULL) 
        {
          pipe->SetSlice(coord); 
          pipe->SetNormal(m_SliceNormal); 
        }
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
        pipe->SetSlice(coord, m_SliceNormal); 
      }
      else
      {
        //BES: 12.6.2009 - TODO: this branch should be removed when mafPipeSurfaceSlice_BES committed down
        mafPipePolylineSlice* pipe = mafPipePolylineSlice::SafeDownCast(curPolylinePipe);
        if (pipe != NULL) 
        {
          pipe->SetSlice(coord); 
          pipe->SetNormal(m_SliceNormal); 
        }
      }
    }
  }	

	for(int i = 0; i < m_CurrentPolylineGraphEditor.size();i++)
	{
		mafPipe * curPGEpipe = m_CurrentPolylineGraphEditor.at(i)->GetPipe();
    if (curPGEpipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(curPGEpipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, m_SliceNormal); 
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
        pipe->SetSlice(coord, m_SliceNormal); 
      }
      else
      {
        //BES: 12.6.2009 - TODO: this branch should be removed when mafPipeSurfaceSlice_BES committed down
        mafPipeMeshSlice* pipe = mafPipeMeshSlice::SafeDownCast(curMeshPipe);
        if (pipe != NULL) 
        {
          pipe->SetSlice(coord); 
          pipe->SetNormal(m_SliceNormal); 
        }
      }
    }   
  }

  // update text
  this->UpdateText();
}
//----------------------------------------------------------------------------
//Get the slice origin coordinates and normal.
void mafViewSlice::GetSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{
  if (Origin != NULL)
    memcpy(Origin, m_Slice, sizeof(m_Slice));

  if (Normal != NULL)
    memcpy(Normal, m_SliceNormal, sizeof(m_SliceNormal));
}

//----------------------------------------------------------------------------
void mafViewSlice::BorderUpdate()
//----------------------------------------------------------------------------
{
  if (NULL != m_Border)
  {
    BorderCreate(m_BorderColor);
  }
}

//----------------------------------------------------------------------------
void mafViewSlice::BorderCreate(double col[3])
//----------------------------------------------------------------------------
{
  m_BorderColor[0] = col[0];
  m_BorderColor[1] = col[1];
  m_BorderColor[2] = col[2];

  if(m_Border) BorderDelete();
  int size[2];
  this->GetWindow()->GetSize(&size[0],&size[1]);
  vtkPlaneSource *ps = vtkPlaneSource::New();
  ps->SetOrigin(0, 0, 0);
  ps->SetPoint1(size[0]-1, 0, 0);
  ps->SetPoint2(0, size[1]-1, 0);

  vtkOutlineFilter *of = vtkOutlineFilter::New();
  of->SetInput((vtkDataSet *)ps->GetOutput());

  vtkCoordinate *coord = vtkCoordinate::New();
  coord->SetCoordinateSystemToDisplay();
  coord->SetValue(size[0]-1, size[1]-1, 0);

  vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
  pdmd->SetInput(of->GetOutput());
  pdmd->SetTransformCoordinate(coord);

  vtkProperty2D *pd = vtkProperty2D::New();
  pd->SetDisplayLocationToForeground();
  pd->SetLineWidth(4);
  pd->SetColor(col[0],col[1],col[2]);

  m_Border = vtkActor2D::New();
  m_Border->SetMapper(pdmd);
  m_Border->SetProperty(pd);
  m_Border->SetPosition(1,1);

  m_Rwi->m_RenFront->AddActor(m_Border);

  vtkDEL(ps);
  vtkDEL(of);
  vtkDEL(coord);
  vtkDEL(pdmd);
  vtkDEL(pd);
}
//----------------------------------------------------------------------------
void mafViewSlice::SetBorderOpacity(double value)
//----------------------------------------------------------------------------
{
  if(m_Border)
  {
    m_Border->GetProperty()->SetOpacity(value);
    m_Border->Modified();
  }

}
//----------------------------------------------------------------------------
void mafViewSlice::BorderDelete()
//----------------------------------------------------------------------------
{
  if(m_Border)
  {
    m_Rwi->m_RenFront->RemoveActor(m_Border);
    vtkDEL(m_Border);
  }  
}

//----------------------------------------------------------------------------
void mafViewSlice::UpdateSurfacesList(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *sceneNode = m_Sg->Vme2Node(vme);
  for(int i=0;i<m_CurrentSurface.size();i++)
    if (sceneNode==m_CurrentSurface[i])
      m_CurrentSurface.erase(m_CurrentSurface.begin()+i);

	for(int i=0;i<m_CurrentPolyline.size();i++)
		if (sceneNode==m_CurrentPolyline[i])
			m_CurrentPolyline.erase(m_CurrentPolyline.begin()+i);

	for(int i=0;i<m_CurrentPolylineGraphEditor.size();i++)
		if (sceneNode==m_CurrentPolylineGraphEditor[i])
			m_CurrentPolylineGraphEditor.erase(m_CurrentPolylineGraphEditor.begin()+i);

  for(int i=0;i<m_CurrentMesh.size();i++)
    if (sceneNode==m_CurrentMesh[i])
      m_CurrentMesh.erase(m_CurrentMesh.begin()+i);
}

//----------------------------------------------------------------------------
void mafViewSlice::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (show)
    {
			if(m_AttachCamera)
				m_AttachCamera->SetVme(vme);
	  }
    else
    {
			if(m_AttachCamera)
				m_AttachCamera->SetVme(NULL);
      this->UpdateText(0);
    }
  }
	else 
    this->UpdateSurfacesList(vme);

	Superclass::VmeShow(vme, show);
}
//----------------------------------------------------------------------------
void mafViewSlice::VmeRemove(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->UpdateSurfacesList(vme);
  Superclass::VmeRemove(vme);
}

//-------------------------------------------------------------------------
void mafViewSlice::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "mafViewSlice" << '\t' << this << std::endl;
  os << indent << "Name" << '\t' << m_Label << std::endl;
  os << std::endl;
  m_Sg->Print(os,1);
}

//-------------------------------------------------------------------------
void mafViewSlice::MultiplyPointByInputVolumeABSMatrix(double *point)
//-------------------------------------------------------------------------
{
  if(m_CurrentVolume && m_CurrentVolume->GetVme())
  {
    mafMatrix *mat = m_CurrentVolume->GetVme()->GetAbsMatrixPipe()->GetMatrixPointer();
    double coord[4];
    coord[0] = point[0];
    coord[1] = point[1];
    coord[2] = point[2];
    double result[4];

    vtkTransform *newT = vtkTransform::New();
    newT->SetMatrix(mat->GetVTKMatrix());
    newT->TransformPoint(coord, result);
    vtkDEL(newT);

    point[0] = result[0];
    point[1] = result[1];
    point[2] = result[2];
  } 
}


void mafViewSlice::CameraUpdate()
{  
  
  if (m_CurrentVolume)
  {
    mafVME *volume = m_CurrentVolume->GetVme();
    
    std::ostringstream stringStream;
    stringStream << "VME " << volume->GetName() << " ABS matrix:" << std::endl;

    volume->GetAbsMatrixPipe()->GetMatrixPointer()->Print(stringStream);
    
    m_NewABSPose = volume->GetAbsMatrixPipe()->GetMatrix();
    
    if (DEBUG_MODE == true)
      mafLogMessage(stringStream.str().c_str());
    
    // Fix bug #2085: Added by Losi 05.11.2010
    // Avoid pan & zoom reset while changing timestamp
    mafMatrix oldABSPoseForEquals;
    oldABSPoseForEquals.DeepCopy(&m_OldABSPose);
    oldABSPoseForEquals.SetTimeStamp(m_NewABSPose.GetTimeStamp());

    if (m_NewABSPose.Equals(&oldABSPoseForEquals))
    { 
      if (DEBUG_MODE == true)
        mafLogMessage("Calling Superclass Camera Update ");
      
      Superclass::CameraUpdate();
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
  
    Superclass::CameraUpdate();
  }
}



void mafViewSlice::SetCameraParallelToDataSetLocalAxis( int axis )
{
  double oldCameraPosition[3] = {0,0,0};
  double oldCameraFocalPoint[3] = {0,0,0};
  double *oldCameraOrientation;


  this->GetRWI()->GetCamera()->GetFocalPoint(oldCameraFocalPoint);
  this->GetRWI()->GetCamera()->GetPosition(oldCameraPosition);
  oldCameraOrientation = this->GetRWI()->GetCamera()->GetOrientation();

  mafVME *currentVMEVolume = m_CurrentVolume->GetVme();
  assert(currentVMEVolume);

  vtkDataSet *vmeVTKData = currentVMEVolume->GetOutput()->GetVTKData();
  vtkMatrix4x4 *vmeABSMatrix = currentVMEVolume->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();

  double absDataBounds[6] = {0,0,0,0,0,0};

  currentVMEVolume->GetOutput()->GetBounds(absDataBounds);

  double newCameraFocalPoint[3] = {0,0,0};

  newCameraFocalPoint[0] = (absDataBounds[0] + absDataBounds[1]) / 2;
  newCameraFocalPoint[1] = (absDataBounds[2] + absDataBounds[3]) / 2;
  newCameraFocalPoint[2] = (absDataBounds[4] + absDataBounds[5]) / 2;

  double newCameraViewUp[3] = {0,0,0};
  double newCameraPosition[3] = {0,0,0};

  if (axis  == mafTransform::X)
  {
    mafTransform::GetVersor(mafTransform::Z,mafMatrix(vmeABSMatrix),newCameraViewUp );

    double xVersor[3] = {0,0,0};

    mafTransform::GetVersor(mafTransform::X,mafMatrix(vmeABSMatrix),xVersor );
    mafTransform::MultiplyVectorByScalar(100, xVersor, xVersor);
    mafTransform::AddVectors(newCameraFocalPoint, xVersor, newCameraPosition);
  }
  else if (axis == mafTransform::Y)
  {
    mafTransform::GetVersor(mafTransform::Z,mafMatrix(vmeABSMatrix),newCameraViewUp );

    double yVersor[3] = {0,0,0};  


    mafTransform::GetVersor(mafTransform::Y,mafMatrix(vmeABSMatrix),yVersor );
    mafTransform::MultiplyVectorByScalar(-100, yVersor, yVersor);
    mafTransform::AddVectors(newCameraFocalPoint, yVersor, newCameraPosition);
  }
  else if (axis == mafTransform::Z)
  {
    mafTransform::GetVersor(mafTransform::Y,mafMatrix(vmeABSMatrix),newCameraViewUp );
    mafTransform::MultiplyVectorByScalar(-1, newCameraViewUp, newCameraViewUp);

    double zVersor[3] = {0,0,0};

    mafTransform::GetVersor(mafTransform::Z,mafMatrix(vmeABSMatrix),zVersor );
    mafTransform::MultiplyVectorByScalar(-100, zVersor, zVersor);
    mafTransform::AddVectors(newCameraFocalPoint, zVersor, newCameraPosition);
  }

  vtkCamera *camera = this->GetRWI()->GetCamera();
  camera->SetFocalPoint(newCameraFocalPoint);
  camera->SetPosition(newCameraPosition);
  camera->SetViewUp(newCameraViewUp);
  camera->SetClippingRange(0.1,1000);

}

void mafViewSlice::CameraUpdateForRotatedVolumes()
{
  if (m_CameraPositionId == CAMERA_OS_X)
  {
    if (m_CurrentVolume != NULL)
    {
      SetCameraParallelToDataSetLocalAxis(mafTransform::X); 
      
      // needed to update surface slices during camera rotation
      if (m_CurrentSurface.size()  != 0)
      {
        SetSlice(m_LastSliceOrigin, NULL);    
      }

      this->CameraReset(m_CurrentVolume->GetVme());
    }    
  }
  else if (m_CameraPositionId == CAMERA_OS_Y)
  {
    if (m_CurrentVolume != NULL)
    {
      SetCameraParallelToDataSetLocalAxis(mafTransform::Y);

      // needed to update surface slices during camera rotation
      if (m_CurrentSurface.size()  != 0)
      {
        SetSlice(m_LastSliceOrigin, NULL);    
      }
      
      this->CameraReset(m_CurrentVolume->GetVme());
    }    
  }
  else if (m_CameraPositionId == CAMERA_OS_Z  || m_CameraPositionId == CAMERA_CT)
  {
    if (m_CurrentVolume != NULL)
    {
      SetCameraParallelToDataSetLocalAxis(mafTransform::Z);
     
      // needed to update surface slices during camera rotation
      if (m_CurrentSurface.size()  != 0)
      {
        SetSlice(m_LastSliceOrigin, NULL);    
      }

      this->CameraReset(m_CurrentVolume->GetVme());
    }    
  }
  
  Superclass::CameraUpdate();
}
