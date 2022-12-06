/*=========================================================================
Program:   Albedo
Module:    albaOpInteractorSliderSample.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpInteractorSliderSample.h"
#include "albaDecl.h"
#include "albaInteractorSlider.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaTagArray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEImage.h"
#include "albaVMEItem.h"
#include "albaVMEOutput.h"
#include "albaVMEVolumeGray.h"
#include "albaViewGlobalSlice.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"
#include "vtkImageFlip.h"
#include "vtkImageResample.h"
#include "vtkPointData.h"
#include "vtkDataSetToDataSetFilter.h"
#include "albaVMEIterator.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpInteractorSliderSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpInteractorSliderSample::albaOpInteractorSliderSample(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

	m_View = NULL;
	m_Volume = NULL;

  m_InteractorSlider = NULL;
}
//----------------------------------------------------------------------------
albaOpInteractorSliderSample::~albaOpInteractorSliderSample()
{ }

//----------------------------------------------------------------------------
albaOp* albaOpInteractorSliderSample::Copy()
{
	return (new albaOpInteractorSliderSample(m_Label));
}

//----------------------------------------------------------------------------
bool albaOpInteractorSliderSample::Accept(albaVME *node)
{
  return true;
}

//----------------------------------------------------------------------------
void albaOpInteractorSliderSample::OpRun()
{
	// Open View if necessary
// 	albaEvent e(this, VIEW_SELECTED);
// 	albaEventMacro(e);
// 	if (!e.GetBool())
// 	{
// 		albaEventMacro(albaEvent(this, ID_SHOW_IMAGE_VIEW));
// 		//albaEventMacro(albaEvent(this, ID_SHOW_SLICE_VIEW));
// 	}
	
	// Find Volume
	albaVMEIterator *iter = m_Input->GetRoot()->NewIterator();
	for (albaVME *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
	{
		if (vme->IsA("albaVMEVolumeGray")) // Find Volume
		{
			m_Volume = (albaVMEVolumeGray*)vme;
		}
	}
	iter->Delete();


	if (!m_TestMode)
	{
		m_InteractorSlider = NULL;

		albaEvent e(this, VIEW_SELECTED);
		albaEventMacro(e);

		if (e.GetBool())
		{
			// Create Interactor

			m_SliderValue = 0.0;
			m_SliderMin = -1.0;
			m_SliderMax = 1.0;
			m_SliderSteps = 10;

			m_SliderPosX = 100;
			m_SliderPosY = 100;
			m_SliderLenght = 600;

			m_SliderOrientation = 0; // 0 = HORIZONTAL | 1 = VERTICAL 

			m_SliderShow = true;
			m_SliderLabelsShow = true;

			m_View = e.GetView();
			
			m_InteractorSlider = new albaInteractorSlider(m_View, m_SliderOrientation, m_SliderValue, m_SliderMin, m_SliderMax, m_SliderPosX, m_SliderPosY, m_SliderLenght);

			//m_InteractorSlider = new albaInteractorSlider(m_View);
			albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorSlider));
			m_InteractorSlider->SetListener(this);

			CreateGui();
			ShowGui();
		}
		else
			OpStop(OP_RUN_CANCEL);
	}
	else
		OpStop(OP_RUN_CANCEL);
}

//----------------------------------------------------------------------------
void albaOpInteractorSliderSample::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Divider(2);

	m_Gui->Label("Slider Props", true);

	m_Gui->Divider(1);
	m_Gui->Label("Logic:");
	m_Gui->Double(ID_SLIDER_EDIT, "Value", &m_SliderValue);
	m_Gui->Double(ID_SLIDER_EDIT, "Min", &m_SliderMin);
	m_Gui->Double(ID_SLIDER_EDIT, "Max", &m_SliderMax);
	m_Gui->Integer(ID_SLIDER_EDIT, "Steps", &m_SliderSteps);

	m_Gui->Divider(1);
	m_Gui->Label("Rendering:");

	m_Gui->Integer(ID_SLIDER_INIT, "X", &m_SliderPosX);
	m_Gui->Integer(ID_SLIDER_INIT, "Y", &m_SliderPosY);
	m_Gui->Integer(ID_SLIDER_INIT, "Lenght", &m_SliderLenght);
	

	wxString choises[2] = { _("HORIZONTAL"),_("VERTICAL") };
	m_Gui->Radio(ID_SLIDER_INIT, "Orient", &m_SliderOrientation, 2, choises);

	m_Gui->Divider(1);
	m_Gui->Label("Options:");
	m_Gui->Bool(ID_SLIDER_SHOW, "Show", &m_SliderShow);
	m_Gui->Bool(ID_SLIDER_LABELS_SHOW, "Show Lab", &m_SliderLabelsShow);

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(ID_NONE, false);
}

//----------------------------------------------------------------------------
void albaOpInteractorSliderSample::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}
	
	// Remove Interactor
	m_InteractorSlider->ShowSlider(false);
	albaEventMacro(albaEvent(this, PER_POP));
	albaDEL(m_InteractorSlider);

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void albaOpInteractorSliderSample::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			case ID_SLIDER_SHOW:
				m_InteractorSlider->ShowSlider(m_SliderShow);
				break;

			case ID_SLIDER_LABELS_SHOW:
				m_InteractorSlider->ShowSlider(false);
				m_InteractorSlider->ShowText(m_SliderLabelsShow);
				m_InteractorSlider->ShowSlider(m_SliderShow);
				break;

			case ID_SLIDER_EDIT:
			{
				m_InteractorSlider->SetValue(m_SliderValue);
				m_InteractorSlider->SetRange(m_SliderMin, m_SliderMax);
				m_InteractorSlider->SetSteps(m_SliderSteps);
			}
			break;

			case ID_SLIDER_INIT:
			{
				m_InteractorSlider->ShowSlider(false);

				albaEventMacro(albaEvent(this, PER_POP));
				albaDEL(m_InteractorSlider);

				m_InteractorSlider = new albaInteractorSlider(m_View, m_SliderOrientation, m_SliderValue, m_SliderMin, m_SliderMax, m_SliderPosX, m_SliderPosY, m_SliderLenght);
				albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorSlider));
				m_InteractorSlider->SetListener(this);

				m_Gui->Update();
			}
			break;
			}
		}
		else
		{
			switch (e->GetId())
			{
			case albaInteractorSlider::ID_VALUE_CHANGED:
			{
				m_SliderValue = m_InteractorSlider->GetValue();
				m_Gui->Update();

				UpdateVolumeSlice();
			}
			break;
			}
		}
	}
}

//----------------------------------------------------------------------------
void albaOpInteractorSliderSample::UpdateVolumeSlice()
{
	albaViewGlobalSlice *viewSlice = albaViewGlobalSlice::SafeDownCast(m_View);
	 
	if (viewSlice && m_Volume)
	{
		int sliceIndex = m_SliderValue;
		m_SlicePlane = XY;

		//viewSlice->SetSliceAxis(m_SlicePlane);

		double origin[3];

		if (m_Volume->GetOutput()->GetVTKData()->IsA("vtkImageData"))
		{
			vtkImageData *sp = vtkImageData::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
			sp->Update();
			double spc[3];
			sp->GetSpacing(spc);
			sp->GetOrigin(origin);

			if (m_SlicePlane == XY)
				origin[2] = (sliceIndex - 1)*spc[2] + origin[2];
			else if (m_SlicePlane == XZ)
				origin[1] = (sliceIndex - 1)*spc[1] + origin[1];
			else if (m_SlicePlane == YZ)
				origin[0] = (sliceIndex - 1)*spc[0] + origin[0];

		}
		else
		{
			vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
			rg->Update();
			origin[0] = rg->GetXCoordinates()->GetTuple1(0);
			origin[1] = rg->GetYCoordinates()->GetTuple1(0);
			origin[2] = rg->GetZCoordinates()->GetTuple1(0);

			if (m_SlicePlane == XY)
				origin[2] = rg->GetZCoordinates()->GetTuple1(sliceIndex - 1);
			else if (m_SlicePlane == XZ)
				origin[1] = rg->GetYCoordinates()->GetTuple1(sliceIndex - 1);
			if (m_SlicePlane == YZ)
				origin[0] = rg->GetXCoordinates()->GetTuple1(sliceIndex - 1);
		}

		viewSlice->SetSlice(origin);
	}
}
