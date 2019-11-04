/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPolyDataNormals.cxx
 Authors: Josef Kohout (besoft@kiv.zcu.cz)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABo
 ILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAPolyDataNormals.h"
#include "vtkSetGet.h"
#include "vtkPolydata.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

vtkCxxRevisionMacro(vtkALBAPolyDataNormals, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkALBAPolyDataNormals);

vtkALBAPolyDataNormals::vtkALBAPolyDataNormals()
{
	this->m_LastUpdateTime = 0;
}

/*virtual*/ void vtkALBAPolyDataNormals::UpdateData(vtkDataObject *output)
{		
	//strategy: if our settings has not changed, and our input has the same geometry,
	//then reuse already calculated normal vectors, otherwise, perform full calculation
	unsigned long tm = this->GetMTime();	
	
	vtkPolyData* input = this->GetInput();
	if (input->GetPoints() != NULL) {
		tm += input->GetPoints()->GetMTime();
	}

	if (input->GetPolys() != NULL) {
		tm += input->GetPolys()->GetMTime();
	}

	if (input->GetStrips() != NULL) {
		tm += input->GetStrips()->GetMTime();
	}

	vtkPolyData* outp = this->GetOutput();
	if (outp == NULL || tm > this->m_LastUpdateTime || this->FlipNormals)
	{
		//perform full update
		Superclass::UpdateData(output);		
	}
	else
	{
		//store already calculated normals
		vtkDataArray *np, *nc;
		if (outp->GetPointData() == NULL) {
			np = NULL;
		}
		else 
		{
			if (NULL != (np = outp->GetPointData()->GetNormals()))
				np->Register(this);
		}

		if (outp->GetCellData() == NULL) {
			nc = NULL;
		}
		else 
		{
			if (NULL != (nc = outp->GetCellData()->GetNormals()))
				nc->Register(this);
		}

		//perform update that just passes data to output
		int savePN = this->ComputePointNormals;
		this->ComputePointNormals = 0;

		int saveCN = this->ComputeCellNormals;
		this->ComputeCellNormals = 0;

		Superclass::UpdateData(output);		

		this->ComputePointNormals = savePN;
		this->ComputeCellNormals = saveCN;		

		//and restore our normals
		if (np != NULL)
		{
			outp->GetPointData()->SetNormals(np);
			np->UnRegister(this);
		}

		if (nc != NULL)
		{
			outp->GetCellData()->SetNormals(nc);
			nc->UnRegister(this);
		}							
	}
	
	
	this->m_LastUpdateTime = tm;
}
