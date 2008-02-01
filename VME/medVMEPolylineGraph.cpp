/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEPolylineGraph.cpp,v $
Language:  C++
Date:      $Date: 2008-02-01 10:06:36 $
Version:   $Revision: 1.4 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
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