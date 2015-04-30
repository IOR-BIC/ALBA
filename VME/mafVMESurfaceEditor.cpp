/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceEditor
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

#include "mafVMESurfaceEditor.h"
#include "mafObject.h"
#include "mafVMEOutputSurfaceEditor.h"
#include "mafTagArray.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESurfaceEditor)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMESurfaceEditor::mafVMESurfaceEditor()
//-------------------------------------------------------------------------
{
	m_VisualPipeName = "mafPipeSurfaceEditor";

	GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	SetVisibleToTraverse(false);
}

//-------------------------------------------------------------------------
mafVMESurfaceEditor::~mafVMESurfaceEditor()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
int mafVMESurfaceEditor::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
	assert(data);
	vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

	if (polydata)
		polydata->Update();

	return Superclass::SetData(polydata,t,mode);
}
//-------------------------------------------------------------------------
mafVMEOutput *mafVMESurfaceEditor::GetOutput()
//-------------------------------------------------------------------------
{
	// allocate the right type of output on demand
	if (m_Output==NULL)
	{
		SetOutput(mafVMEOutputSurfaceEditor::New()); // create the output
	}
	return m_Output;
}