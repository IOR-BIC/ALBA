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
// Constants:
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
	ID_RESET,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpFilterVolume);



//----------------------------------------------------------------------------
albaOpFilterVolume::albaOpFilterVolume(const wxString &label) 
:albaOp(label)
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_OutputVolumeRegistered = false;
  
  m_InputData         = NULL;

  m_Dimensionality  = 3;
  m_SmoothRadius[0] = m_SmoothRadius[1] = m_SmoothRadius[2] = 1.5;
  m_StandardDeviation[0] = m_StandardDeviation[1] = m_StandardDeviation[2] = 2.0;

  m_KernelSize[0] = m_KernelSize[1] = m_KernelSize[2] = 1;

}
//----------------------------------------------------------------------------
albaOpFilterVolume::~albaOpFilterVolume()
{
}
//----------------------------------------------------------------------------
bool albaOpFilterVolume::InternalAccept(albaVME*node)
{
  return (node && node->IsALBAType(albaVMEVolumeGray));
}
//----------------------------------------------------------------------------
albaOp *albaOpFilterVolume::Copy()
{
  return (new albaOpFilterVolume(m_Label));
}
//----------------------------------------------------------------------------
void albaOpFilterVolume::OpRun()
{
	m_Input->GetOutput()->Update();
	m_InputData = (vtkImageData *)m_Input->GetOutput()->GetVTKData();

	albaNEW(m_OutputVolume);
	m_OutputVolume->DeepCopy(m_Input);

	albaString name = m_Input->GetName();
	name << " filtered";
	m_OutputVolume->SetName(name);
	m_OutputVolume->ReparentTo(m_Input);
	
	GetLogicManager()->VmeShow(m_OutputVolume, true);

	if (!m_TestMode)
	{
		CreateGui();
		ShowGui();
	}
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::CreateGui()
{
  // interface:
  m_Gui = new albaGUI(this);
		 
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
  m_Gui->Button(ID_RESET,_("Reset"));

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(ID_RESET, false);
	m_Gui->Enable(wxOK, false);
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OpDo()
{
  if (m_OutputVolume->GetParent()!=m_Input)
  {
		//if the output volume is not a child of the input volume, it means that the operation has been undone and we have to reparent the output volume to the input volume.
		if(m_OutputVolumeRegistered)
		{
			m_OutputVolume->UnRegister(this);
			m_OutputVolumeRegistered = false;
		}
		m_OutputVolume->ReparentTo(m_Input);
		GetLogicManager()->CameraUpdate();
  }
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OpUndo()
{
	if (m_OutputVolume->GetParent() == m_Input)
	{
		//Register the output volume to this operation to avoid that it will be destroyed until the operation is destroyed.
		m_OutputVolumeRegistered = true;
		m_OutputVolume->Register(this);
		m_OutputVolume->ReparentTo(NULL);
		GetLogicManager()->CameraUpdate();
	}
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
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
				OnReplace();
			break;
      case ID_RESET:
        OnReset(); 
      break;
      case wxOK:
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
         OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OpStop(int result)
{
	HideGui();
	albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OnSmooth()
{
	wxBusyCursor *wait_cursor = NULL;

	if (!m_TestMode)
	{
		wait_cursor = new wxBusyCursor();
		m_Gui->Enable(ID_SMOOTH,false);
	  m_Gui->Enable(ID_MEDIAN,false);
	  m_Gui->Enable(ID_REPLACE,false);
		m_Gui->Update();
	}

  vtkALBASmartPointer<vtkImageGaussianSmooth> smoothFilter;
	smoothFilter->SetInputData(m_OutputVolume->GetOutput()->GetVTKData());
  smoothFilter->SetDimensionality(m_Dimensionality);
  smoothFilter->SetRadiusFactors(m_SmoothRadius);
  smoothFilter->SetStandardDeviations(m_StandardDeviation);

	albaEventMacro(albaEvent(this, BIND_TO_PROGRESSBAR, smoothFilter));
	
	smoothFilter->Update();

  m_OutputVolume->SetData(smoothFilter->GetOutput(),m_Input->GetTimeStamp());
	m_OutputVolume->GetOutput()->Update();
	GetLogicManager()->CameraUpdate();

  if (!m_TestMode)
  {
	  m_Gui->Enable(ID_SMOOTH,true);
		m_Gui->Enable(ID_MEDIAN,true);
	  m_Gui->Enable(ID_REPLACE,true);
	
		m_Gui->Enable(ID_RESET,true);
		m_Gui->Enable(wxOK,true);
		m_Gui->Update();

		
		cppDEL(wait_cursor);
  }
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OnMedian()
{
	wxBusyCursor *wait_cursor = NULL;

	if (!m_TestMode)
	{
		wait_cursor = new wxBusyCursor();
		m_Gui->Enable(ID_SMOOTH, false);
		m_Gui->Enable(ID_MEDIAN, false);
		m_Gui->Enable(ID_REPLACE, false);
		m_Gui->Update();
	}
  vtkALBASmartPointer<vtkImageMedian3D> medianFilter;
  medianFilter->SetInputData(m_OutputVolume->GetOutput()->GetVTKData());
  medianFilter->SetKernelSize(m_KernelSize[0],m_KernelSize[1],m_KernelSize[2]);

	albaEventMacro(albaEvent(this, BIND_TO_PROGRESSBAR, medianFilter));

  medianFilter->Update();

	m_OutputVolume->SetData(medianFilter->GetOutput(), m_Input->GetTimeStamp());
	m_OutputVolume->GetOutput()->Update();
	GetLogicManager()->CameraUpdate();

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_SMOOTH, true	);
		m_Gui->Enable(ID_MEDIAN, true);
		m_Gui->Enable(ID_REPLACE, true);

	  m_Gui->Enable(ID_RESET,true);
	  m_Gui->Enable(wxOK,true);
		m_Gui->Update();

		cppDEL(wait_cursor);
  }
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OnReplace()
{
	wxBusyCursor *wait_cursor = NULL;

	if (!m_TestMode)
	{
		wait_cursor = new wxBusyCursor();
		m_Gui->Enable(ID_SMOOTH, false);
		m_Gui->Enable(ID_MEDIAN, false);
		m_Gui->Enable(ID_REPLACE, false);
		m_Gui->Update();
	}

	vtkDataSet *inputData = m_OutputVolume->GetOutput()->GetVTKData();

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
		m_OutputVolume->SetData(outputDataRG, m_Input->GetTimeStamp());
	}
	else
	{
		outputDataSP->GetPointData()->SetScalars(outputScalars);
		m_OutputVolume->SetData(outputDataSP, m_Input->GetTimeStamp());
	}
	m_OutputVolume->GetOutput()->Update();
	GetLogicManager()->CameraUpdate();


	if (!m_TestMode)
	{
		m_Gui->Enable(ID_SMOOTH, true);
		m_Gui->Enable(ID_MEDIAN, true);
		m_Gui->Enable(ID_REPLACE, true);

		m_Gui->Enable(ID_RESET, true);
		m_Gui->Enable(wxOK, true);
		m_Gui->Update();

		cppDEL(wait_cursor);
	}
}

//----------------------------------------------------------------------------
void albaOpFilterVolume::OnReset()
{

	m_OutputVolume->SetData(m_InputData, m_Input->GetTimeStamp());
	m_OutputVolume->GetOutput()->Update();

	if (!m_TestMode)
	{
		m_Gui->Enable(ID_SMOOTH, true);
		m_Gui->Enable(ID_MEDIAN, true);
		m_Gui->Enable(ID_REPLACE, true);

		m_Gui->Enable(ID_RESET, false);
		m_Gui->Enable(wxOK, false);
		m_Gui->Update();
	}

	GetLogicManager()->CameraUpdate();
}
