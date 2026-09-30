/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceEditor
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

#include "albaVMESurfaceEditor.h"
#include "albaObject.h"
#include "albaVMEOutputSurfaceEditor.h"
#include "albaTagArray.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMESurfaceEditor)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMESurfaceEditor::albaVMESurfaceEditor()
//-------------------------------------------------------------------------
{
	m_VisualPipeName = "albaPipeSurfaceEditor";

	GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
	SetVisibleToTraverse(false);
}

//-------------------------------------------------------------------------
albaVMESurfaceEditor::~albaVMESurfaceEditor()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
int albaVMESurfaceEditor::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
	assert(data);
	vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

	return Superclass::SetData(polydata,t,mode);
}
//-------------------------------------------------------------------------
albaVMEOutput *albaVMESurfaceEditor::GetOutput()
//-------------------------------------------------------------------------
{
	// allocate the right type of output on demand
	if (m_Output==NULL)
	{
		SetOutput(albaVMEOutputSurfaceEditor::New()); // create the output
	}
	return m_Output;
}