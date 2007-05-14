/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkHoleConnectivity.cxx,v $
Language:  C++
Date:      $Date: 2007-05-14 12:18:45 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2007
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

#include "vtkHoleConnectivity.h"

#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
vtkHoleConnectivity::vtkHoleConnectivity(vtkPolyData *input,vtkIdType ID)
//----------------------------------------------------------------------------
{
	this->SetInput(input);
	PointID = ID;
}
//----------------------------------------------------------------------------
vtkHoleConnectivity::~vtkHoleConnectivity()
//----------------------------------------------------------------------------
{  
}
//----------------------------------------------------------------------------
void vtkHoleConnectivity::Execute()
//----------------------------------------------------------------------------
{
	vtkPolyData *output = this->GetOutput();
	vtkPolyData *input = this->GetInput();

	vtkPolyDataConnectivityFilter *connectivityFilter = vtkPolyDataConnectivityFilter::New();
	connectivityFilter->SetInput(input);
	connectivityFilter->SetExtractionModeToClosestPointRegion ();
	connectivityFilter->SetClosestPoint(point);
	connectivityFilter->Modified();
	connectivityFilter->Update();

	output->DeepCopy(vtkPolyData::SafeDownCast(connectivityFilter->GetOutput()));

	//connectivityFilter->Delete();
}
//----------------------------------------------------------------------------
void vtkHoleConnectivity::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
	vtkPolyDataToPolyDataFilter::PrintSelf(os,indent);
}

