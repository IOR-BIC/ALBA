/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputLandmarkCloud
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

#include "albaVMEOutputLandmarkCloud.h"
#include "albaGUI.h"
#include "albaDataPipe.h"

#include "albaVME.h"
#include "albaVMELandmarkCloud.h"

#include "vtkDataSet.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputLandmarkCloud)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputLandmarkCloud::albaVMEOutputLandmarkCloud()
//-------------------------------------------------------------------------
{
  m_NumLandmarks = "0";
  m_Gui = NULL;
}

//-------------------------------------------------------------------------
albaVMEOutputLandmarkCloud::~albaVMEOutputLandmarkCloud()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkDataSet *albaVMEOutputLandmarkCloud::GetVTKData()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  assert(albaVMELandmarkCloud::SafeDownCast(m_VME));
	
	return Superclass::GetVTKData();
}

//-------------------------------------------------------------------------
albaGUI* albaVMEOutputLandmarkCloud::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  m_NumLandmarks = albaVMELandmarkCloud::SafeDownCast(m_VME)->GetNumberOfLandmarks();
  m_Gui->Label(_("Points:"), &m_NumLandmarks, true);

  m_Gui->Divider();
	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputLandmarkCloud::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();
  m_NumLandmarks = albaVMELandmarkCloud::SafeDownCast(m_VME)->GetNumberOfLandmarks();
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
