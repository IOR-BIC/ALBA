/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterVolume
 Authors: Paolo Quadrani
 
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

#include "albaOpFilterVolume.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMEVolumeGray.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageMedian3D.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkRectilinearGrid.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpFilterVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpFilterVolume::albaOpFilterVolume(const wxString &label) 
:albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

  m_PreviewResultFlag	= false;
	m_ClearInterfaceFlag= false;
  
	m_ResultImageData	  = NULL;
	m_OriginalImageData = NULL;
  m_InputData         = NULL;

  m_Dimensionality  = 3;
  m_SmoothRadius[0] = m_SmoothRadius[1] = m_SmoothRadius[2] = 1.5;
  m_StandardDeviation[0] = m_StandardDeviation[1] = m_StandardDeviation[2] = 2.0;

  m_KernelSize[0] = m_KernelSize[1] = m_KernelSize[2] = 1;

  m_ApplyDirectlyOnInput = true;
}
//----------------------------------------------------------------------------
albaOpFilterVolume::~albaOpFilterVolume()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ResultImageData);
	vtkDEL(m_OriginalImageData);
}
//----------------------------------------------------------------------------
bool albaOpFilterVolume::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVMEVolumeGray));
}
//----------------------------------------------------------------------------
albaOp *albaOpFilterVolume::Copy()
//----------------------------------------------------------------------------
{
  return (new albaOpFilterVolume(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_SMOOTH = MINID,
	ID_STANDARD_DEVIATION,
	ID_RADIUS_FACTOR,
	ID_MEDIAN,
	ID_KERNEL_SIZE,
	ID_REPLACE_MIN,
	ID_REPLACE_MAX,
	ID_REPLACE_VALUE,
	ID_REPLACE,
	ID_PREVIEW,
	ID_RESET_ALL,
  ID_APPLY_ON_INPUT,
};
//----------------------------------------------------------------------------
void albaOpFilterVolume::OpRun()   
//----------------------------------------------------------------------------
{ 
	m_Input->GetOutput()->Update();
  m_InputData = (vtkImageData*)m_Input->GetOutput()->GetVTKData();
  if (!m_ApplyDirectlyOnInput)
  {
	  vtkNEW(m_ResultImageData);
		m_ResultImageData->DeepCopy(m_InputData);
		
		vtkNEW(m_OriginalImageData);
		m_OriginalImageData->DeepCopy(m_InputData);
  }
	
	if (!m_TestMode)
	{
		CreateGui();
	  ShowGui();
	}
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new albaGUI(this);
		
  m_Gui->Bool(ID_APPLY_ON_INPUT,_("Apply on input"),&m_ApplyDirectlyOnInput,1,_("Check this flag for big volumes to save memory"));
 
	m_Gui->Divider(2);
	m_Gui->Label(_("Smooth"),true);
  m_Gui->Vector(ID_STANDARD_DEVIATION,_("Sd: "),m_StandardDeviation,0.1,100,2,_("standard deviation for smooth filter"));
  m_Gui->Vector(ID_RADIUS_FACTOR,_("Radius: "),m_SmoothRadius,1,10,2,_("radius for smooth filter"));
  m_Gui->Button(ID_SMOOTH,_("Apply smooth"));

	m_Gui->Divider(2);
  m_Gui->Label(_("Median"),true);
  m_Gui->Vector(ID_KERNEL_SIZE,_("Kernel: "),m_KernelSize,1,10,_("Size of kernel"));
  m_Gui->Button(ID_MEDIAN,_("Apply median"));
	
	double *dataRange = m_InputData->GetScalarRange();
	m_ReplaceRange[0] = dataRange[0];
	m_ReplaceRange[1] = dataRange[1];
	m_ReplaceValue = 0;

	m_Gui->Divider(2);
	m_Gui->Label("Replace values",true);
	m_Gui->FloatSlider(ID_REPLACE_MIN, _("From: "), &m_ReplaceRange[0], dataRange[0],dataRange[1]);
	m_Gui->FloatSlider(ID_REPLACE_MAX, _("To: "), &m_ReplaceRange[1], dataRange[0], dataRange[1]);
	m_Gui->Double(ID_REPLACE_VALUE, _("Value: "), &m_ReplaceValue);
	m_Gui->Button(ID_REPLACE, _("Apply replace"));
	
  m_Gui->Divider(2);
  m_Gui->Button(ID_PREVIEW,_("Preview"));
  m_Gui->Button(ID_RESET_ALL,_("Clear"));
  m_Gui->Enable(ID_PREVIEW,false);
  m_Gui->Enable(ID_RESET_ALL,false);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(wxOK, false);
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OpDo()
//----------------------------------------------------------------------------
{
  if (m_ResultImageData)
  {
    ((albaVMEVolumeGray *)m_Input)->SetData(m_ResultImageData,m_Input->GetTimeStamp());
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_OriginalImageData)
  {
    ((albaVMEVolumeGray *)m_Input)->SetData(m_OriginalImageData,m_Input->GetTimeStamp());
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	
      case ID_APPLY_ON_INPUT:
        if (m_ApplyDirectlyOnInput)
        {
          wxMessageBox(_("Filters are applied directly to the input volume. No undo can be done to retrieve previous volume."),_("Warning"));
          vtkDEL(m_ResultImageData);
          vtkDEL(m_OriginalImageData);
        }
        else
        {
          m_InputData = (vtkImageData*)m_Input->GetOutput()->GetVTKData();
          vtkNEW(m_ResultImageData);
          m_ResultImageData->DeepCopy(m_InputData);

          vtkNEW(m_OriginalImageData);
          m_OriginalImageData->DeepCopy(m_InputData);
        }
      break;
      case ID_SMOOTH:
        OnSmooth();
      break;
      case ID_MEDIAN:
        OnMedian();
      break;
			case ID_REPLACE_MIN:
			{
				m_ReplaceRange[1] = MAX(m_ReplaceRange[0], m_ReplaceRange[1]);
				m_Gui->Update();
			}
			break;
			case ID_REPLACE_MAX:
			{
				m_ReplaceRange[0] = MIN(m_ReplaceRange[0], m_ReplaceRange[1]);
				m_Gui->Update();
			}
			break;
			case ID_REPLACE:
			{
				OnReplace();
			}
			break;
      case ID_PREVIEW:
        OnPreview(); 
      break;
      case ID_RESET_ALL:
        OnClear(); 
      break;
      case wxOK:
        if(m_PreviewResultFlag)
          OnPreview();
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
        if(m_ClearInterfaceFlag)
          OnClear();
        OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OnSmooth()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
		m_Gui->Enable(ID_SMOOTH,false);
	  m_Gui->Enable(ID_RADIUS_FACTOR,false);
	  m_Gui->Enable(ID_RADIUS_FACTOR,false);
		m_Gui->Update();
	}

  vtkALBASmartPointer<vtkImageGaussianSmooth> smoothFilter;
	if (m_ApplyDirectlyOnInput)
    smoothFilter->SetInputData(m_InputData);
  else
    smoothFilter->SetInputData(m_ResultImageData);
  smoothFilter->SetDimensionality(m_Dimensionality);
  smoothFilter->SetRadiusFactors(m_SmoothRadius);
  smoothFilter->SetStandardDeviations(m_StandardDeviation);
	smoothFilter->Update();

  if (m_ApplyDirectlyOnInput)
  {
    ((albaVMEVolumeGray *)m_Input)->SetData(smoothFilter->GetOutput(),m_Input->GetTimeStamp());
		m_InputData = (vtkImageData*)m_Input->GetOutput()->GetVTKData();
		GetLogicManager()->CameraUpdate();
  }
  else
    m_ResultImageData->DeepCopy(smoothFilter->GetOutput());

  m_PreviewResultFlag = !m_ApplyDirectlyOnInput;

  if (!m_TestMode)
  {
	  m_Gui->Enable(ID_SMOOTH,true);
		m_Gui->Enable(ID_RADIUS_FACTOR,true);
	  m_Gui->Enable(ID_RADIUS_FACTOR,true);
	
		m_Gui->Enable(ID_PREVIEW,m_PreviewResultFlag);
		m_Gui->Enable(ID_RESET_ALL,m_PreviewResultFlag);
		m_Gui->Enable(wxOK,true);
	  m_Gui->Enable(wxCANCEL,m_PreviewResultFlag);
  }
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OnMedian()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
	  wxBusyCursor wait;
	  m_Gui->Enable(ID_MEDIAN,false);
	  m_Gui->Enable(ID_KERNEL_SIZE,false);
	  m_Gui->Update();
  }

  vtkALBASmartPointer<vtkImageMedian3D> medianFilter;
  if (m_ApplyDirectlyOnInput)
    medianFilter->SetInputData(m_InputData);
  else
    medianFilter->SetInputData(m_ResultImageData);
  medianFilter->SetKernelSize(m_KernelSize[0],m_KernelSize[1],m_KernelSize[2]);
  medianFilter->Update();

  if (m_ApplyDirectlyOnInput)
  {
    ((albaVMEVolumeGray *)m_Input)->SetData(medianFilter->GetOutput(),m_Input->GetTimeStamp());
		m_InputData = (vtkImageData*)m_Input->GetOutput()->GetVTKData();
		GetLogicManager()->CameraUpdate();
  }
  else
    m_ResultImageData->DeepCopy(medianFilter->GetOutput());

  m_PreviewResultFlag = !m_ApplyDirectlyOnInput;

  if (!m_TestMode)
  {
	  m_Gui->Enable(ID_MEDIAN,true);
	  m_Gui->Enable(ID_KERNEL_SIZE,true);
	
	  m_Gui->Enable(ID_PREVIEW,m_PreviewResultFlag);
	  m_Gui->Enable(ID_RESET_ALL,m_PreviewResultFlag);
	  m_Gui->Enable(wxOK,true);
	  m_Gui->Enable(wxCANCEL,m_PreviewResultFlag);
  }
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OnPreview()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;
	
  ((albaVMEVolumeGray *)m_Input)->SetData(m_ResultImageData,m_Input->GetTimeStamp());

	m_Gui->Enable(ID_PREVIEW,false);
	m_Gui->Enable(ID_RESET_ALL,true);
	m_Gui->Enable(wxOK,true);

	m_PreviewResultFlag   = false;
	m_ClearInterfaceFlag	= true;

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OnClear()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		wxBusyCursor wait;
	}

  ((albaVMEVolumeGray *)m_Input)->SetData(m_OriginalImageData,m_Input->GetTimeStamp());
	
  if (!m_ApplyDirectlyOnInput)
  {
    m_ResultImageData->DeepCopy(m_OriginalImageData);
  }

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_SMOOTH,true);
		m_Gui->Enable(ID_RADIUS_FACTOR,true);
	  m_Gui->Enable(ID_STANDARD_DEVIATION,true);
	
		m_Gui->Enable(ID_PREVIEW,false);
		m_Gui->Enable(ID_RESET_ALL,false);
		m_Gui->Enable(wxOK,false);
		m_Gui->Update();
	}

	m_PreviewResultFlag = false;
	m_ClearInterfaceFlag= false;

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OnReplace()
{
	vtkImageData *inputData;
	if (m_ApplyDirectlyOnInput)
		inputData = m_InputData;
	else
		inputData = m_ResultImageData;

	vtkDataArray *inputScalars = inputData->GetPointData()->GetScalars();
	vtkDataArray *outputScalars;

	vtkALBASmartPointer<vtkRectilinearGrid> outputDataRG;
	vtkALBASmartPointer<vtkImageData> outputDataSP;

	if (inputData->IsA("vtkRectilinearGrid"))
	{
		outputDataRG->DeepCopy(inputData);
		outputScalars = outputDataRG->GetPointData()->GetScalars();
	}
	else
	{
		outputDataSP->DeepCopy(inputData);
		outputScalars = outputDataSP->GetPointData()->GetScalars();
	}

	int nTuples=outputScalars->GetNumberOfTuples();

	for (int i = 0; i < nTuples; i++)
	{
		double value;
		inputScalars->GetTuple(i, &value);

		if (value != 0)
			value++;

		if (value >= m_ReplaceRange[0] && value <= m_ReplaceRange[1])
			outputScalars->SetTuple(i, &m_ReplaceValue);
	}

	if (inputData->IsA("vtkRectilinearGrid"))
	{
		outputDataRG->GetPointData()->SetScalars(outputScalars);

		if (m_ApplyDirectlyOnInput)
		{
			((albaVMEVolumeGray *)m_Input)->SetData(outputDataRG, m_Input->GetTimeStamp());
			m_InputData = (vtkImageData*)m_Input->GetOutput()->GetVTKData();
			albaEventMacro(albaEvent(this, CAMERA_UPDATE));
		}
		else
			m_ResultImageData->DeepCopy(outputDataRG);
	}
	else
	{
		outputDataSP->GetPointData()->SetScalars(outputScalars);

		if (m_ApplyDirectlyOnInput)
		{
			((albaVMEVolumeGray *)m_Input)->SetData(outputDataSP, m_Input->GetTimeStamp());
			m_InputData = (vtkImageData*)m_Input->GetOutput()->GetVTKData();
			albaEventMacro(albaEvent(this, CAMERA_UPDATE));
		}
		else
			m_ResultImageData->DeepCopy(outputDataSP);
	}


	m_PreviewResultFlag = !m_ApplyDirectlyOnInput;

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_PREVIEW, m_PreviewResultFlag);
		m_Gui->Enable(ID_RESET_ALL, m_PreviewResultFlag);
		m_Gui->Enable(wxOK, true);
		m_Gui->Enable(wxCANCEL, m_PreviewResultFlag);
	}
}
