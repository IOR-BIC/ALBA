/*=========================================================================

 Program: MAF2
 Module: medVMEPolylineGraph
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

#include "medVMEPolylineGraph.h"
#include "mafObject.h"
#include "mafVMEOutputPolyline.h"
#include "mafPolylineGraph.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEPolylineGraph)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEPolylineGraph::medVMEPolylineGraph()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
medVMEPolylineGraph::~medVMEPolylineGraph()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
int medVMEPolylineGraph::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
	assert(data);

	vtkPolyData *polydata=data;

	if (polydata)
		polydata->Update();

  int result=MAF_ERROR;
	
  mafPolylineGraph *utility = new mafPolylineGraph;
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
	mafErrorMacro("Trying to set the wrong type of fata inside a VME Polyline :"<< (data?data->GetClassName():"NULL"));
	return MAF_ERROR;
}
//-------------------------------------------------------------------------
mafVMEOutput *medVMEPolylineGraph::GetOutput()
//-------------------------------------------------------------------------
{
	// allocate the right type of output on demand
	if (m_Output==NULL)
	{
		SetOutput(mafVMEOutputPolyline::New()); // create the output
	}
	return m_Output;
}
//-------------------------------------------------------------------------
int medVMEPolylineGraph::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  if (data->IsA("vtkPolyData"))
  {
    return SetData((vtkPolyData*)data,t,mode);
  }

  mafErrorMacro("Trying to set the wrong type of data inside a "<<(const char *)GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}