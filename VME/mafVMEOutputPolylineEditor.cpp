/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputPolylineEditor
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEOutputPolylineEditor.h"
#include "mafVME.h"
#include "mafGUI.h"

#include "vtkPolyData.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputPolylineEditor)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputPolylineEditor::mafVMEOutputPolylineEditor()
//-------------------------------------------------------------------------
{

}

//-------------------------------------------------------------------------
mafVMEOutputPolylineEditor::~mafVMEOutputPolylineEditor()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMEOutputPolylineEditor::GetPolylineData()
//-------------------------------------------------------------------------
{
	return (vtkPolyData *)GetVTKData();
}
//-------------------------------------------------------------------------
mafGUI* mafVMEOutputPolylineEditor::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = mafVMEOutput::CreateGui();

	return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputPolylineEditor::Update()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	m_VME->Update();

	if (m_Gui)
	{
		m_Gui->Update();
	}
}
