/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointCloud
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

#include "albaPipePointCloud.h"
#include "albaVMEOutputSurface.h"
#include "albaVME.h"
#include "vtkPolyData.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipePointCloud);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipePointCloud::albaPipePointCloud()
:albaPipeGenericPolydata()
//----------------------------------------------------------------------------
{
	m_Representation = POINTS_REP;
	m_SkipNormalFilter = true;
}

//----------------------------------------------------------------------------
albaPipePointCloud::~albaPipePointCloud()
//----------------------------------------------------------------------------
{
}


//----------------------------------------------------------------------------
albaGUI *albaPipePointCloud::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
	
	m_Gui->Divider();
	m_Gui->FloatSlider(ID_THICKNESS, _("Size:"), &m_Border, 1.0, 10.0);
	SetRepresentation((REPRESENTATIONS)m_Representation);
	m_Gui->Divider(2);
	m_Gui->Bool(ID_USE_VTK_PROPERTY, "Property", &m_UseVTKProperty, 1);
	m_MaterialButton = new albaGUIMaterialButton(m_Vme, this);
	m_Gui->AddGui(m_MaterialButton->GetGui());
	m_MaterialButton->Enable(m_UseVTKProperty != 0);

	CreateScalarsGui(m_Gui);

	m_Gui->Divider();
	m_Gui->Label("");
	m_Gui->Update();
	return m_Gui;
}

//----------------------------------------------------------------------------
void albaPipePointCloud::SetRepresentation(REPRESENTATIONS rep)
{
	Superclass::SetRepresentation(POINTS_REP);
}

//----------------------------------------------------------------------------
void albaPipePointCloud::UpdateProperty(bool fromTag /*= false*/)
{
	SetRepresentation((REPRESENTATIONS)m_Representation);
	Superclass::UpdateProperty(fromTag);
}

//----------------------------------------------------------------------------
vtkPolyData *albaPipePointCloud::GetInputAsPolyData()
{
	if (!m_InputAsPolydata)
	{
		assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface));
		albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
		assert(surface_output);
		surface_output->Update();
		m_InputAsPolydata = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
		assert(m_InputAsPolydata);
		m_InputAsPolydata->Update();
	}

	return m_InputAsPolydata;
}
