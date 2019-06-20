/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPolylineEditor
 Authors: Matteo Giacomoni
 
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

#include "albaVMEOutputPolylineEditor.h"
#include "albaVME.h"
#include "albaGUI.h"

#include "vtkPolyData.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputPolylineEditor)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputPolylineEditor::albaVMEOutputPolylineEditor()
//-------------------------------------------------------------------------
{

}

//-------------------------------------------------------------------------
albaVMEOutputPolylineEditor::~albaVMEOutputPolylineEditor()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *albaVMEOutputPolylineEditor::GetPolylineData()
//-------------------------------------------------------------------------
{
	return (vtkPolyData *)GetVTKData();
}
//-------------------------------------------------------------------------
albaGUI* albaVMEOutputPolylineEditor::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = albaVMEOutput::CreateGui();

	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputPolylineEditor::Update()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	m_VME->Update();

	if (m_Gui)
	{
		m_Gui->Update();
	}
}
