/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputImage
 Authors: Marco Petrone
 
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


#include "albaVMEOutputImage.h"
#include "albaVMEImage.h"
#include "albaVME.h"
#include "albaGUI.h"
#include "albaDataPipe.h"

#include "vtkImageData.h"
#include "vtkWindowLevelLookupTable.h"
#include "mmaMaterial.h"
#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputImage)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputImage::albaVMEOutputImage()
{
  m_Material = NULL;
}

//-------------------------------------------------------------------------
albaVMEOutputImage::~albaVMEOutputImage()
{
}

//-------------------------------------------------------------------------
vtkImageData *albaVMEOutputImage::GetImageData()
{
  return (vtkImageData *)GetVTKData();
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEOutputImage::GetMaterial()
{
	// if the VME set the material directly in the output return it
	if (m_Material)
		return  m_Material;

	// search for a material attribute in the VME connected to this output
	return GetVME() ? ((albaVMEImage *)GetVME())->GetMaterial(): NULL;
}
//-------------------------------------------------------------------------
void albaVMEOutputImage::SetMaterial(mmaMaterial *material)
{
  m_Material = material;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputImage::CreateGui()
{
  //This method is used only to load data
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  wxBusyCursor wait;

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
    this->Update();
  }

	m_Gui->Label(_("Size:"), true);
	m_Gui->Label(&m_ImageSize);

	m_Gui->Label(_("Bounds:"), true);
	m_Gui->Label(&m_ImageBounds[0]);
	m_Gui->Label(&m_ImageBounds[1]);
	m_Gui->Label(&m_ImageBounds[2]);
  
	return m_Gui;
}

//-------------------------------------------------------------------------
void albaVMEOutputImage::Update()
{
	assert(m_VME);
	m_VME->Update();
	if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
	{
		double b[6];
		m_VME->GetOutput()->GetVMELocalBounds(b);
		m_ImageBounds[0] = "";
		m_ImageBounds[0] << " xmin: " << wxString::Format("%g", RoundValue(b[0])).c_str() << "   xmax: " << wxString::Format("%g", RoundValue(b[1])).c_str();
		m_ImageBounds[1] = "";
		m_ImageBounds[1] << " ymin: " << wxString::Format("%g", RoundValue(b[2])).c_str() << "   ymax: " << wxString::Format("%g", RoundValue(b[3])).c_str();
		m_ImageBounds[2] = "";
		m_ImageBounds[2] << " zmin: " << wxString::Format("%g", RoundValue(b[4])).c_str() << "   zmax: " << wxString::Format("%g", RoundValue(b[5])).c_str();

		m_ImageSize = wxString::Format("%g x %g", RoundValue(b[1] - b[0]), RoundValue(b[3] - b[2]));
	}

	if (m_Gui)
	{
		m_Gui->Update();
	}
}
