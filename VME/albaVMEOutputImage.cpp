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
#include "mmaMaterial.h"
#include "albaDataPipe.h"

#include "vtkImageData.h"
#include "vtkWindowLevelLookupTable.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputImage)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputImage::albaVMEOutputImage()
//-------------------------------------------------------------------------
{
  m_Material = NULL;
}

//-------------------------------------------------------------------------
albaVMEOutputImage::~albaVMEOutputImage()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkImageData *albaVMEOutputImage::GetImageData()
//-------------------------------------------------------------------------
{
  return (vtkImageData *)GetVTKData();
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEOutputImage::GetMaterial()
//-------------------------------------------------------------------------
{
	// if the VME set the material directly in the output return it
	if (m_Material)
		return  m_Material;

	// search for a material attribute in the VME connected to this output
	return GetVME() ? ((albaVMEImage *)GetVME())->GetMaterial(): NULL;
}
//-------------------------------------------------------------------------
void albaVMEOutputImage::SetMaterial(mmaMaterial *material)
//-------------------------------------------------------------------------
{
  m_Material = material;
}

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputImage::CreateGui()
//-------------------------------------------------------------------------
{
  //This method is used only to load data
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  wxBusyCursor wait;

  if (m_VME && m_VME->GetDataPipe() && m_VME->GetDataPipe()->GetVTKData())
  {
    this->Update();
  }
   return m_Gui;
}
