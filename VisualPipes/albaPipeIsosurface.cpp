/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurface
 Authors: Gianluigi Crimi
 
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

#include "albaPipeIsosurface.h"
#include "albaVMEOutputVolume.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "vtkPolyData.h"
#include "vtkFlyingEdges3D.h"
#include "vtkContourFilter.h"
#include "albaGUI.h"
#include "vtkImageData.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeIsosurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeIsosurface::albaPipeIsosurface()
:albaPipeGenericPolydata()
//----------------------------------------------------------------------------
{
	m_ContourFilter = NULL;
	m_RGContourFilter = NULL;
	m_SkipNormalFilter = true;
}

//----------------------------------------------------------------------------
albaPipeIsosurface::~albaPipeIsosurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_ContourFilter);
	vtkDEL(m_RGContourFilter);
}

//----------------------------------------------------------------------------
bool albaPipeIsosurface::SetContourValue(float value)
{
	if (m_ContourFilter == NULL && m_RGContourFilter == NULL)
		return false;

	m_ContourValue = value;

	if (m_OldContourValue == m_ContourValue)
		return true;

	if (m_ContourFilter)
	{
		m_ContourFilter->SetValue(0, m_ContourValue);
		m_ContourFilter->Modified();
	}
	else
	{
		m_RGContourFilter->SetValue(0, m_ContourValue);
		m_RGContourFilter->Modified();
	}

	m_OldContourValue = m_ContourValue;
	return true;
}

//----------------------------------------------------------------------------
float albaPipeIsosurface::GetContourValue()
{
	return m_ContourValue;
}

//----------------------------------------------------------------------------
void albaPipeIsosurface::SetAlphaValue(double value)
{

}


//----------------------------------------------------------------------------
void albaPipeIsosurface::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId()) 
		{
		  case ID_CONTOUR_VALUE:
			{
				SetContourValue((float)m_ContourValue);
				m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
				m_Gui->Update();
			}
			break;
		  case ID_GENERATE_ISOSURFACE:
			{
				ExctractIsosurface();	
			}
			break;
		  default:
				Superclass::OnEvent(alba_event);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void albaPipeIsosurface::ExctractIsosurface(albaVMESurface* isoSurfaceVME /* = NULL */)
{
	if (m_ContourFilter == NULL && m_RGContourFilter == NULL)
		return;

	vtkPolyDataAlgorithm* filter = (m_ContourFilter!=NULL) ? (vtkPolyDataAlgorithm *)m_ContourFilter : m_RGContourFilter;

	
	filter->Update();
	vtkPolyData* surface = filter->GetOutput();
	

	if (isoSurfaceVME == NULL)
	{
		albaString name;
		name.Printf("%s Isosurface %g", m_Vme->GetName(), m_ContourValue);

		albaNEW(isoSurfaceVME);
		isoSurfaceVME->SetName(name.GetCStr());
	}

	
	isoSurfaceVME->SetData(surface,0);
	isoSurfaceVME->ReparentTo(m_Vme);
	
}

void albaPipeIsosurface::ExctractIsosurface(vtkPolyData* isoSurfacePD)
{
	if (m_ContourFilter == NULL && m_RGContourFilter == NULL)
		return;

	vtkPolyDataAlgorithm* filter = (m_ContourFilter != NULL) ? (vtkPolyDataAlgorithm*)m_ContourFilter : m_RGContourFilter;
	filter->Update();
	vtkPolyData* surface = filter->GetOutput();

	isoSurfacePD->DeepCopy(surface);
}

//----------------------------------------------------------------------------
albaGUI* albaPipeIsosurface::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);

	double range[2] = { 0, 0 };
	m_Vme->GetOutput()->GetVTKData()->GetScalarRange(range);


	m_Gui->FloatSlider(ID_CONTOUR_VALUE, _("Contour"), &m_ContourValue, range[0], range[1]);

	CreateGenericPolydataGui(m_Gui);
	
	m_Gui->Label("");
	m_Gui->Button(ID_GENERATE_ISOSURFACE, "Extract Isosurface");
	m_Gui->Divider();
	m_Gui->Update();

	return m_Gui;
}


//----------------------------------------------------------------------------
vtkAlgorithmOutput* albaPipeIsosurface::GetPolyDataOutputPort()
{
	if (!m_PolydataConnection)
	{
		assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputVolume));
		albaVMEOutputVolume *vol_output = albaVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
		assert(vol_output);
		vol_output->Update();

		vtkDataSet* volData = vol_output->GetVTKData();
		volData->GetScalarRange(m_DataRange);

		m_OldContourValue = m_ContourValue = m_DataRange[0] * 0.25 + m_DataRange[1] * 0.75;

		if (vtkImageData::SafeDownCast(volData))
		{
			vtkNEW(m_ContourFilter);
			m_ContourFilter->SetInputData(volData);
			m_ContourFilter->SetComputeScalars(false);
			m_ContourFilter->SetComputeGradients(false);
			m_ContourFilter->SetComputeNormals(false);
			m_ContourFilter->SetValue(0, m_ContourValue);

			m_PolydataConnection = m_ContourFilter->GetOutputPort();

		}
		else //Rectilinear Grid
		{
			vtkNEW(m_RGContourFilter);
			m_RGContourFilter->SetInputData(volData);
			m_RGContourFilter->SetComputeScalars(false);
			m_RGContourFilter->SetComputeGradients(false);
			m_RGContourFilter->SetComputeNormals(false);
			m_RGContourFilter->SetValue(0, m_ContourValue);

			m_PolydataConnection = m_RGContourFilter->GetOutputPort();
		}
	}

	return m_PolydataConnection;
}
