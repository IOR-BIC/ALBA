/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSliceSegmentation
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = false;

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGUI.h"
#include "albaIndent.h"
#include "albaViewSliceSegmentation.h"
#include "albaPipeSurfaceSlice.h"
#include "albaPipePolylineSlice.h"
#include "albaPipeMeshSlice.h"

#include "albaPipeSurfaceSlice.h"
#include "albaPipePolylineSlice.h"

#include "albaVME.h"
#include "albaVMEVolume.h"
#include "albaVMESlicer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaPipeFactory.h"
#include "albaPipe.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaAttachCamera.h"
#include "albaPipePolylineGraphEditor.h"
#include "albaTransform.h"
#include "albaAbsMatrixPipe.h"

#include "vtkDataSet.h"
#include "vtkALBARayCast3DPicker.h"
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
albaCxxTypeMacro(albaViewSliceSegmentation);


#include "albaPipeVolumeOrthoSlice.h"
#include "albaPipeVolumeArbSlice.h"

const int LAST_SLICE_ORIGIN_VALUE_NOT_INITIALIZED = 0;

//----------------------------------------------------------------------------
albaViewSliceSegmentation::albaViewSliceSegmentation(wxString label /* =  */, int camera_position /* = CAMERA_CT */, bool show_axes /* = false */, bool show_grid /* = false */,  int stereo /* = 0 */,bool showTICKs/* =false */,bool textureInterpolate/* =true */)
:albaViewSlice(label,camera_position,show_axes,show_grid, stereo)
{
	m_CurrentSegmentation = NULL;
	m_IsShowingSegmentation = false;
}
//----------------------------------------------------------------------------
albaViewSliceSegmentation::~albaViewSliceSegmentation()
{
  
}
//----------------------------------------------------------------------------
albaView *albaViewSliceSegmentation::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewSliceSegmentation *v = new albaViewSliceSegmentation(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType,m_ShowVolumeTICKs,m_TextureInterpolate);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}


//----------------------------------------------------------------------------
void albaViewSliceSegmentation::VmeShow(albaVME *vme, bool show)
{
	if (m_IsShowingSegmentation)
		return;
	else
		Superclass::VmeShow(vme, show);
}

//----------------------------------------------------------------------------
void albaViewSliceSegmentation::VmeCreatePipe(albaVME *vme)
{
	if (m_IsShowingSegmentation)
		VmeCreatePipeForSegmentation(vme);
	else
		Superclass::VmeCreatePipe(vme);
}

//----------------------------------------------------------------------------
void albaViewSliceSegmentation::VmeCreatePipeForSegmentation(albaVME *vme)
{
  
  albaString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  albaSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->GetPipe());

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;

		albaPipeFactory *pipe_factory  = albaPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    albaObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    albaPipe *pipe = (albaPipe*)obj;
    if (pipe != NULL)
    {
      pipe->SetListener(this);
			albaPipeVolumeOrthoSlice *orthoPipe;
      if (orthoPipe=albaPipeVolumeOrthoSlice::SafeDownCast(pipe))
      {
				m_CurrentSegmentation = n;

        int slice_mode;
        vtkDataSet *data = vme->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        switch(m_CameraPositionId)
        {
        case CAMERA_OS_X:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_X;
          break;
        case CAMERA_OS_Y:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_Y;
          break;
        case CAMERA_OS_P:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_ORTHO;
          break;
        default:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_Z;
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
      albaErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void albaViewSliceSegmentation::VmeDeletePipe(albaVME *vme)
{
  albaSceneNode *n = m_Sg->Vme2Node(vme);

	m_NumberOfVisibleVme--;
  
	if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
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

  if(vme->IsALBAType(albaVMELandmark))
    RemoveFromSlicingList(vme);
}

//----------------------------------------------------------------------------
void albaViewSliceSegmentation::SetSlice(double* Origin, double* Normal)
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
		albaPipeSlice* pipe = albaPipeSlice::SafeDownCast(m_CurrentSegmentation->GetPipe());
		if (pipe != NULL)
			pipe->SetSlice(newOrigin, NULL);
	}
 }
//----------------------------------------------------------------------------
void albaViewSliceSegmentation::SetSliceAxis(int sliceAxis)
{
	Superclass::SetSliceAxis(sliceAxis);
			
	if (m_CurrentSegmentation)
	{
		albaPipeVolumeOrthoSlice* pipe = albaPipeVolumeOrthoSlice::SafeDownCast(m_CurrentSegmentation->GetPipe());
		if (pipe != NULL)
			pipe->SetSliceDirection(sliceAxis);
	}
}

//----------------------------------------------------------------------------
void albaViewSliceSegmentation::VmeSegmentationShow(albaVME *vme, bool show)
{
	m_IsShowingSegmentation = true;
	Superclass::VmeShow(vme, show);
	m_IsShowingSegmentation = false;
}
