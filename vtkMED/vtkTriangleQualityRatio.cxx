/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkTriangleQualityRatio.cxx,v $
Language:  C++
Date:      $Date: 2007-04-17 11:04:26 $
Version:   $Revision: 1.3 $
Authors:   Matteo Giacomoni - Daniele Giunchi
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

#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkTriangle.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkPointSet.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkPolyData.h"

#include "vtkTriangleQualityRatio.h"
//-------------------------------------------------------------------------
vtkTriangleQualityRatio* vtkTriangleQualityRatio::New()
//-------------------------------------------------------------------------
{
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTriangleQualityRatio");
	if(ret)
	{
		return (vtkTriangleQualityRatio*)ret;
	}
	return new vtkTriangleQualityRatio;
}
//-------------------------------------------------------------------------
vtkTriangleQualityRatio::vtkTriangleQualityRatio()
//-------------------------------------------------------------------------
{
	m_Input = NULL;
}
//-------------------------------------------------------------------------
vtkTriangleQualityRatio::~vtkTriangleQualityRatio()
//-------------------------------------------------------------------------
{
	m_Input = NULL;
	m_Output->Delete();

}
//-------------------------------------------------------------------------
void vtkTriangleQualityRatio::Update() 
//-------------------------------------------------------------------------
{
	// check inputs
	if (!m_Input) return;

	m_Output = vtkPolyData::New();
	m_Output->DeepCopy(m_Input);

	vtkDoubleArray *array=vtkDoubleArray::New();
	array->SetName("quality");
	m_Output->GetCellData()->SetScalars(array);
  array->Delete();
	// name variables
	double qualitySum = 0.0;
	double longestEdge;
	double perimeter;

	long cellsNumber = m_Input->GetNumberOfCells();

	double vertex1[3];
	double vertex2[3];
	double vertex3[3];

	double qualityRatioNormalize = 2.0 * sqrt(3.0);

	m_MaxRatio = 0.0;
	m_MeanRatio = 0.0;
	m_MinRatio = 999.0;

	for (long currentCell=0;currentCell<cellsNumber;currentCell++)
	{
		longestEdge = 0.0;
		perimeter = 0.0;

		vtkIdList *ID=m_Output->GetCell(currentCell)->GetPointIds();
		m_Output->GetPoint(ID->GetId(0),vertex1);
		m_Output->GetPoint(ID->GetId(1),vertex2);
		m_Output->GetPoint(ID->GetId(2),vertex3);

		// perimeter calculation
		double tempEdge = sqrt(vtkMath::Distance2BetweenPoints(vertex1,vertex2));
		if(tempEdge > longestEdge) longestEdge = tempEdge;
		perimeter += tempEdge;

		tempEdge = sqrt(vtkMath::Distance2BetweenPoints(vertex1,vertex3));
		if(tempEdge > longestEdge) longestEdge = tempEdge;
		perimeter += tempEdge;

		tempEdge = sqrt(vtkMath::Distance2BetweenPoints(vertex3,vertex2));
		if(tempEdge > longestEdge) longestEdge = tempEdge;
		perimeter += tempEdge;

		double area = vtkTriangle::TriangleArea(vertex1,vertex2,vertex3);
		double qualityLocal;
		if(area>0.000001)
		{
			
			qualityLocal = 0.5 * perimeter * longestEdge / area;
			qualityLocal = qualityRatioNormalize/qualityLocal;
			qualitySum += qualityLocal;
			if(qualityLocal>m_MaxRatio)
				m_MaxRatio = qualityLocal;
			if(qualityLocal<m_MinRatio)
				m_MinRatio = qualityLocal;
		}
		else
		{
			qualitySum+=0.0;
			qualityLocal=0.0;
		}
		
		m_Output->GetCellData()->GetScalars()->InsertNextTuple1(qualityLocal);

	}
	m_MeanRatio = qualitySum / (cellsNumber);
}