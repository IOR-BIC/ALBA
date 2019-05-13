/*=========================================================================

 Program: MAF2
 Module: mafViewSliceSegmentation
 Authors: Gianluigi Crimi
 
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
#include "mafViewSliceSegmentation.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipePolylineSlice.h"
#include "mafPipeMeshSlice.h"

#include "mafPipeSurfaceSlice.h"
#include "mafPipePolylineSlice.h"

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
mafCxxTypeMacro(mafViewSliceSegmentation);
//----------------------------------------------------------------------------

#include "mafMemDbg.h"
#include "mafPipeVolumeOrthoSlice.h"
#include "mafPipeVolumeArbSlice.h"

const int LAST_SLICE_ORIGIN_VALUE_NOT_INITIALIZED = 0;

//----------------------------------------------------------------------------
mafViewSliceSegmentation::mafViewSliceSegmentation(wxString label /* =  */, int camera_position /* = CAMERA_CT */, bool show_axes /* = false */, bool show_grid /* = false */,  int stereo /* = 0 */,bool showTICKs/* =false */,bool textureInterpolate/* =true */)
:mafViewSlice(label,camera_position,show_axes,show_grid, stereo)
{
	m_CurrentSegmentation = NULL;
	m_IsShowingSegmentation = false;
}
//----------------------------------------------------------------------------
mafViewSliceSegmentation::~mafViewSliceSegmentation()
{
  
}
//----------------------------------------------------------------------------
mafView *mafViewSliceSegmentation::Copy(mafObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSliceSegmentation *v = new mafViewSliceSegmentation(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType,m_ShowVolumeTICKs,m_TextureInterpolate);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}


//----------------------------------------------------------------------------
void mafViewSliceSegmentation::VmeCreatePipe(mafVME *vme)
{
	if (m_IsShowingSegmentation)
		VmeCreatePipeForSegmentation(vme);
	else
		Superclass::VmeCreatePipe(vme);
}

//----------------------------------------------------------------------------
void mafViewSliceSegmentation::VmeCreatePipeForSegmentation(mafVME *vme)
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
			mafPipeVolumeOrthoSlice *orthoPipe;
      if (orthoPipe=mafPipeVolumeOrthoSlice::SafeDownCast(pipe))
      {
				m_CurrentSegmentation = n;

        int slice_mode;
        vtkDataSet *data = vme->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        switch(m_CameraPositionId)
        {
        case CAMERA_OS_X:
          slice_mode = mafPipeVolumeOrthoSlice::SLICE_X;
          break;
        case CAMERA_OS_Y:
          slice_mode = mafPipeVolumeOrthoSlice::SLICE_Y;
          break;
        case CAMERA_OS_P:
          slice_mode = mafPipeVolumeOrthoSlice::SLICE_ORTHO;
          break;
        default:
          slice_mode = mafPipeVolumeOrthoSlice::SLICE_Z;
        }
        if (m_SliceInitialized)
        {
					orthoPipe->InitializeSliceParameters(slice_mode, m_Slice, false,false,m_TextureInterpolate);
					orthoPipe->SetNormal(m_SliceNormal);
        }
        else
        {
					orthoPipe->InitializeSliceParameters(slice_mode,false,false,m_TextureInterpolate);
        }

        if(m_ShowVolumeTICKs)
					orthoPipe->ShowTICKsOn();
        else
					orthoPipe->ShowTICKsOff();
				orthoPipe->SetInterpolation(m_TrilinearInterpolationOn);
				orthoPipe->SetActorPicking(false);

				UpdateText();
      }
      pipe->Create(n);
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafViewSliceSegmentation::VmeDeletePipe(mafVME *vme)
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);

	m_NumberOfVisibleVme--;
  
	if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
	{
		if (m_CurrentVolume && vme == m_CurrentVolume->GetVme())
		{
			m_CurrentVolume = NULL;
			if (m_AttachCamera)
				m_AttachCamera->SetVme(NULL);
		}
		else if (m_CurrentSegmentation && vme == m_CurrentSegmentation->GetVme())
		{
			m_CurrentSegmentation = NULL;
		}
  }
  assert(n && n->GetPipe());
	n->DeletePipe();

  if(vme->IsMAFType(mafVMELandmark))
    UpdateSurfacesList(vme);
}

//----------------------------------------------------------------------------
void mafViewSliceSegmentation::SetSlice(double* Origin, double* Normal)
{
	Superclass::SetSlice(Origin, Normal);
  
	//now set it for current segmentation
  if (m_CurrentSegmentation)
	{
		//Workaround we add an epsilon to ensure segmentation volume is always visible
		//This compensate the volume shift added on OpSegmentation
		double newOrigin[3];
		newOrigin[0] = Origin[0] + 0.0001;
		newOrigin[1] = Origin[1] - 0.0001;
		newOrigin[2] = Origin[2] - 0.0001;
		mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(m_CurrentSegmentation->GetPipe());
		if (pipe != NULL)
			pipe->SetSlice(newOrigin, NULL);
	}
 }
//----------------------------------------------------------------------------
void mafViewSliceSegmentation::SetSliceAxis(int sliceAxis)
{
	Superclass::SetSliceAxis(sliceAxis);
			
	if (m_CurrentSegmentation)
	{
		mafPipeVolumeOrthoSlice* pipe = mafPipeVolumeOrthoSlice::SafeDownCast(m_CurrentSegmentation->GetPipe());
		if (pipe != NULL)
			pipe->SetSliceDirection(sliceAxis);
	}
}

//----------------------------------------------------------------------------
void mafViewSliceSegmentation::VmeSegmentationShow(mafVME *vme, bool show)
{
	m_IsShowingSegmentation = true;
	VmeShow(vme, show);
	m_IsShowingSegmentation = false;
}
