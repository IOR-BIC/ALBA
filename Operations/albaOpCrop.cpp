/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCrop
 Authors: Matteo Giacomoni & Paolo Quadrani
 
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

#include "albaOpCrop.h"
#include <albaGUIBusyInfo.h>
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaGizmoROI.h"

#include "albaString.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"

#include "vtkALBADistanceFilter.h"
#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkProbeFilter.h"
#include "vtkExtractRectilinearGrid.h"
#include "vtkDoubleArray.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCrop);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCrop::albaOpCrop(const wxString &label, bool showShadingPlane)
: albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_InputPreserving = false;
	m_Canundo	= true;
  m_ShowHandles = 1;
  m_ShowROI = 1;

	m_InputRG = NULL;
	m_InputSP = NULL;	
  m_GizmoROI = NULL;
	m_OutputRG = NULL;
	m_OutputSP = NULL;

  m_ShowShadingPlane = showShadingPlane;
}
//----------------------------------------------------------------------------
albaOpCrop::~albaOpCrop()
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputRG);
	vtkDEL(m_InputSP);
	vtkDEL(m_OutputRG);
	vtkDEL(m_OutputSP);
}

//----------------------------------------------------------------------------
albaOp *albaOpCrop::Copy()
//----------------------------------------------------------------------------
{
	return new albaOpCrop(m_Label,m_ShowShadingPlane);
}
//----------------------------------------------------------------------------
bool albaOpCrop::InternalAccept(albaVME* node)
//----------------------------------------------------------------------------
{
	albaEvent e(this,VIEW_SELECTED);
	albaEventMacro(e);
	return (node && node->IsA("albaVMEVolumeGray") /*&& e.GetBool()*/);
}
//----------------------------------------------------------------------------
void albaOpCrop::OpRun()   
//----------------------------------------------------------------------------
{
	//m_Input->Modified();
  albaEvent e(this,VIEW_SELECTED);
  albaEventMacro(e);

	albaVME* volume = m_Input;
	volume->Update();
	// create gizmo roi
	if(!m_TestMode)
	{
		m_GizmoROI = new albaGizmoROI(volume, this,albaGizmoHandle::BOUNDS,NULL,m_ShowShadingPlane);
		m_GizmoROI->Show(e.GetBool());
	}

	if (volume->GetOutput()->GetVTKData()->IsA("vtkImageData"))
  {
		vtkNEW(m_InputSP);
		m_InputSP->DeepCopy(vtkImageData::SafeDownCast(volume->GetOutput()->GetVTKData()));
    m_InputSP->Update();
		m_InputSP->GetBounds(m_InputBounds);	
		if(!m_TestMode)
			m_GizmoROI->SetBounds(m_InputBounds);
    double spc[3];	
    m_InputSP->GetSpacing(spc);
    m_XSpacing = spc[0];
    m_YSpacing = spc[1];
    m_ZSpacing = spc[2];
  }	
	else if (volume->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
	{
		vtkNEW(m_InputRG);
    m_InputRG->DeepCopy(vtkRectilinearGrid::SafeDownCast(volume->GetOutput()->GetVTKData()));		
    m_InputRG->Update();
		m_InputRG->GetBounds(m_InputBounds);
		if(!m_TestMode)
			m_GizmoROI->SetBounds(m_InputBounds);
	}
	if(!m_TestMode)
	{
		CreateGui();
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpCrop::Crop()
//----------------------------------------------------------------------------
{
	if(!m_TestMode)
		m_GizmoROI->GetBounds(m_CroppingBoxBounds);
		
  albaVMEOutput *output = m_Input->GetOutput();
	if (output->GetVTKData()->IsA("vtkRectilinearGrid"))	
	{
		vtkRectilinearGrid *rgData = vtkRectilinearGrid::SafeDownCast(output->GetVTKData());

		// get cropping bounds
		double gizmoBounds[6] = {0,0,0,0,0,0};
		if(!m_TestMode)
			m_GizmoROI->GetBounds(gizmoBounds);
		else
		{
			for(int i = 0; i < 6; i++)
				gizmoBounds[i] = m_CroppingBoxBounds[i];
		}

		// convert from bounds to index
		int boundsIndexArray[6] = {0,0,0,0,0,0};

		vtkDataArray *coordArray[3] = {NULL, NULL, NULL};
		coordArray[0] = rgData->GetXCoordinates();
		coordArray[1] = rgData->GetYCoordinates();
		coordArray[2] = rgData->GetZCoordinates();

		// for each coordinate array
		for (int numArray = 0; numArray < 3; numArray++)
		{
			int coordId = 0;
			int minId = 0, maxId = 0;      

			while (coordArray[numArray]->GetComponent(coordId,0) < gizmoBounds[2*numArray])
			{
				minId = coordId + 1;
				coordId++;
			}
			//coordId++;
			while (coordArray[numArray]->GetComponent(coordId,0) < gizmoBounds[2*numArray + 1])
			{
				maxId = coordId + 1;
				coordId++;
			}

			boundsIndexArray[2*numArray] = minId;
			boundsIndexArray[2*numArray + 1] = maxId; 
		}
		vtkExtractRectilinearGrid *extractRG = vtkExtractRectilinearGrid::New();
		extractRG->SetInput(rgData);
		extractRG->SetVOI(boundsIndexArray); 
		extractRG->Update();  
			
		vtkNEW(m_OutputRG);
		m_OutputRG->DeepCopy(extractRG->GetOutput());

		extractRG->Delete();
		extractRG = NULL;
	}
	else if (output->GetVTKData()->IsA("vtkImageData"))
	{
		int voi_dim[6];
        int original_vol_dim[6];
		m_InputSP->GetExtent(original_vol_dim);
		
		double xBoundLength, yBoundLength, zBoundLength;
		xBoundLength = fabs(m_InputBounds[1] - m_InputBounds[0]);
		yBoundLength = fabs(m_InputBounds[3] - m_InputBounds[2]);
		zBoundLength = fabs(m_InputBounds[5] - m_InputBounds[4]);
		
		double tolerance = 1e-3; //base tolerance
        

		if(fabs(m_CroppingBoxBounds[0] - m_InputBounds[0])/xBoundLength < tolerance)
		{
          voi_dim[0] = original_vol_dim[0];
		}
		else
		{
          voi_dim[0] = ceil((m_CroppingBoxBounds[0] - m_InputBounds[0])/(m_XSpacing));
		}
		
		if(fabs(m_CroppingBoxBounds[1] - m_InputBounds[1])/xBoundLength < tolerance)
		{
			voi_dim[1] = original_vol_dim[1];
		}
		else
		{
			voi_dim[1] = floor((m_CroppingBoxBounds[1] - m_InputBounds[0])/ m_XSpacing);
		}

		if(fabs(m_CroppingBoxBounds[2] - m_InputBounds[2])/yBoundLength < tolerance)
		{
			voi_dim[2] = original_vol_dim[2];
		}
		else
		{
		    voi_dim[2] = ceil((m_CroppingBoxBounds[2] - m_InputBounds[2])/(m_YSpacing));
		}
	 
		if(fabs(m_CroppingBoxBounds[3] - m_InputBounds[3])/yBoundLength < tolerance)
		{
			voi_dim[3] = original_vol_dim[3];
		}
		else
		{
			voi_dim[3] = floor((m_CroppingBoxBounds[3] - m_InputBounds[2])/ m_YSpacing);
		}
		
		if(fabs(m_CroppingBoxBounds[4] - m_InputBounds[4])/zBoundLength < tolerance)
		{
			voi_dim[4] = original_vol_dim[4];
		}
		else
		{
			voi_dim[4] = ceil((m_CroppingBoxBounds[4] - m_InputBounds[4])/(m_ZSpacing));
		}	
		
		if(fabs(m_CroppingBoxBounds[5] - m_InputBounds[5])/zBoundLength < tolerance)
		{
			voi_dim[5] = original_vol_dim[5];
		}
		else
		{
			voi_dim[5] = floor((m_CroppingBoxBounds[5] - m_InputBounds[4])/ m_ZSpacing);
		}
		
			
		double in_org[3];
		m_InputSP->GetOrigin(in_org);

		// using the vtkALBASmartPointer allows you to don't mind the object Delete
		vtkALBASmartPointer<vtkImageData> v_esp;
		v_esp->SetOrigin(in_org[0] + voi_dim[0] * m_XSpacing,
										 in_org[1] + voi_dim[2] * m_YSpacing,
										 in_org[2] + voi_dim[4] * m_ZSpacing);
		v_esp->SetSpacing(m_XSpacing, m_YSpacing, m_ZSpacing);
		v_esp->SetDimensions(voi_dim[1] - voi_dim[0] + 1,
												 voi_dim[3] - voi_dim[2] + 1,
												 voi_dim[5] - voi_dim[4] + 1);
		v_esp->Modified();

		// I'm using probe filter instead of ExtractVOI (see why before...)
		albaGUIBusyInfo wait(_("Please wait, cropping..."),m_TestMode);
    
		vtkALBASmartPointer<vtkProbeFilter> probeFilter;
		albaEventMacro(albaEvent(this, BIND_TO_PROGRESSBAR, probeFilter));

		probeFilter->SetInput(v_esp);
		probeFilter->SetSource(m_InputSP);
		probeFilter->Update();

		vtkNEW(m_OutputSP);
		m_OutputSP->DeepCopy(probeFilter->GetOutput());

	}
}
//----------------------------------------------------------------------------
void albaOpCrop::OpDo()
//----------------------------------------------------------------------------
{
	if (m_OutputSP)
		((albaVMEVolume*)m_Input)->SetData(m_OutputSP, m_Input->GetTimeStamp());
	else if (m_OutputRG)
		((albaVMEVolume*)m_Input)->SetData(m_OutputRG, m_Input->GetTimeStamp());

	((albaVMEVolume*)m_Input)->GetOutput()->Update();
	((albaVMEVolume*)m_Input)->Update();

	// bug# 2628: gizmos do not update after cropping (workaround code)
	GetLogicManager()->VmeVisualModeChanged(m_Input);
	///////

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpCrop::OpUndo()
//----------------------------------------------------------------------------
{
	if(m_InputSP)
		((albaVMEVolume*)m_Input)->SetData(m_InputSP,m_Input->GetTimeStamp());
	else if(m_InputRG)
		((albaVMEVolume*)m_Input)->SetData(m_InputRG,m_Input->GetTimeStamp());

	((albaVMEVolume*)m_Input)->GetOutput()->Update();
	((albaVMEVolume*)m_Input)->Update();

	// bug# 2628: gizmos do not update after cropping (workaround code)
	GetLogicManager()->VmeVisualModeChanged(m_Input);
	///////

	GetLogicManager()->CameraUpdate();;
}
//----------------------------------------------------------------------------
void albaOpCrop::UpdateGui()
//----------------------------------------------------------------------------
{
  double bounds[6];
  m_GizmoROI->GetBounds(bounds);
  m_XminXmax[0] = bounds[0];
  m_XminXmax[1] = bounds[1];
  m_YminYmax[0] = bounds[2];
  m_YminYmax[1] = bounds[3];
  m_ZminZmax[0] = bounds[4];
  m_ZminZmax[1] = bounds[5];
  m_Gui->Update();
}

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum CROP_WIDGET_ID
{
  ID_SHOW_HANDLES,
  ID_SHOW_ROI,
  ID_FIRST = MINID,	
  ID_CROP_DIR_X,
  ID_CROP_DIR_Y,
  ID_CROP_DIR_Z,
  ID_RESET_CROPPING_AREA,
};

//----------------------------------------------------------------------------
void albaOpCrop::CreateGui() 
//----------------------------------------------------------------------------
{
	double bounds[6];
	m_Input->GetOutput()->GetVTKData()->GetBounds(bounds);
	m_XminXmax[0] = bounds[0];
	m_XminXmax[1] = bounds[1];
	m_YminYmax[0] = bounds[2];
	m_YminYmax[1] = bounds[3];
	m_ZminZmax[0] = bounds[4];
	m_ZminZmax[1] = bounds[5];

	m_Gui = new albaGUI(this);

  m_Gui->Label("");
  m_Gui->Bool(ID_SHOW_HANDLES, _("handles"), &m_ShowHandles, 0, _("toggle gizmo handles visibility"));
  m_Gui->Bool(ID_SHOW_ROI, "ROI", &m_ShowROI, 0, _("toggle region of interest visibility"));
	m_Gui->Label("");
	m_Gui->VectorN(ID_CROP_DIR_X, _("range x"), m_XminXmax, 2, bounds[0], bounds[1]);
	m_Gui->VectorN(ID_CROP_DIR_Y, _("range y"), m_YminYmax, 2, bounds[2], bounds[3]);
	m_Gui->VectorN(ID_CROP_DIR_Z, _("range z"), m_ZminZmax, 2, bounds[4], bounds[5]);

  m_Gui->Button(ID_RESET_CROPPING_AREA, _("reset"), "", _("reset the cropping area"));
	m_Gui->Label("");
	m_Gui->OkCancel();

	m_Gui->Divider();

  ShowGui();
}

//----------------------------------------------------------------------------
void albaOpCrop::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  double bb[6];
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
			case ID_SHOW_HANDLES:
			{
				m_GizmoROI->ShowHandles(m_ShowHandles != 0);
				GetLogicManager()->CameraUpdate();
			}
			break;
			case ID_SHOW_ROI:      
			{
				m_GizmoROI->ShowROI(m_ShowROI != 0);
				GetLogicManager()->CameraUpdate();
			}
			break;
			case ID_CROP_DIR_X:
				m_GizmoROI->GetBounds(bb);
				bb[0] = m_XminXmax[0];
				bb[1] = m_XminXmax[1];
				m_Gui->Update();
				m_GizmoROI->SetBounds(bb);
				GetLogicManager()->CameraUpdate();
			break;
			case ID_CROP_DIR_Y:
				m_GizmoROI->GetBounds(bb);
				bb[2] = m_YminYmax[0];
				bb[3] = m_YminYmax[1];
				m_Gui->Update();
				m_GizmoROI->SetBounds(bb);
				GetLogicManager()->CameraUpdate();
			break;
			case ID_CROP_DIR_Z:
				m_GizmoROI->GetBounds(bb);
				bb[4] = m_ZminZmax[0];
				bb[5] = m_ZminZmax[1];
				m_Gui->Update();
				m_GizmoROI->SetBounds(bb);
				GetLogicManager()->CameraUpdate();
			break;
			case ID_RESET_CROPPING_AREA:
				m_GizmoROI->Reset();
				UpdateGui();
				GetLogicManager()->CameraUpdate();
			break;    
			case wxOK:
				Crop();
				OpStop(OP_RUN_OK);
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
			break;
			case ID_TRANSFORM:
				UpdateGui();
			break;
			default:
				albaEventMacro(*e);
			break;
		}	
	}
}

//----------------------------------------------------------------------------
void albaOpCrop::OpStop(int result)
//----------------------------------------------------------------------------
{  
  HideGui();
  m_GizmoROI->Show(false);

	cppDEL(m_GizmoROI);
	m_GizmoROI = NULL;
  albaEventMacro(albaEvent(this,result));  
}
//----------------------------------------------------------------------------
void albaOpCrop::SetCroppingBoxBounds(double bounds[])
//----------------------------------------------------------------------------
{
	for(int i = 0; i < 6; i++)
		m_CroppingBoxBounds[i] = bounds[i];
}
