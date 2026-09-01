/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineGraph
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

#include "albaVMEPolylineGraph.h"
#include "albaObject.h"
#include "albaVMEOutputPolyline.h"
#include "albaPolylineGraph.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEPolylineGraph)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEPolylineGraph::albaVMEPolylineGraph()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
albaVMEPolylineGraph::~albaVMEPolylineGraph()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
int albaVMEPolylineGraph::SetData(vtkPolyData *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
	assert(data);

	vtkPolyData *polydata=data;

  int result=ALBA_ERROR;
	
  albaPolylineGraph *utility = new albaPolylineGraph;
	utility->CopyFromPolydata(polydata); 
  // check this is a polydata containing only lines
  if (polydata && polydata->GetPolys()->GetNumberOfCells()==0 && \
    polydata->GetStrips()->GetNumberOfCells()==0 && \
    polydata->GetVerts()->GetNumberOfCells()==0 && \
    utility->SelfCheck())
	{
		result = Superclass::SetData(data,t,mode);
    delete utility;
    return result;
	}

  delete utility;
	albaErrorMacro("Trying to set the wrong type of fata inside a VME Polyline :"<< (data?data->GetClassName():"NULL"));
	return ALBA_ERROR;
}
//-------------------------------------------------------------------------
albaVMEOutput *albaVMEPolylineGraph::GetOutput()
//-------------------------------------------------------------------------
{
	// allocate the right type of output on demand
	if (m_Output==NULL)
	{
		SetOutput(albaVMEOutputPolyline::New()); // create the output
	}
	return m_Output;
}
//-------------------------------------------------------------------------
int albaVMEPolylineGraph::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkPolyData"))
  {
    return SetData((vtkPolyData*)data,t,mode);
  }

  albaErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return ALBA_ERROR;
}