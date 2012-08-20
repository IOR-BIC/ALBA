/*=========================================================================

 Program: MAF2Medical
 Module: vtkTriangleQualityRatio
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

vtkStandardNewMacro(vtkTriangleQualityRatio);

// //-------------------------------------------------------------------------
// vtkTriangleQualityRatio* vtkTriangleQualityRatio::New()
// //-------------------------------------------------------------------------
// {
// 	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTriangleQualityRatio");
// 	if(ret)
// 	{
// 		return (vtkTriangleQualityRatio*)ret;
// 	}
// 	return new vtkTriangleQualityRatio;
// }
//-------------------------------------------------------------------------
vtkTriangleQualityRatio::vtkTriangleQualityRatio()
//-------------------------------------------------------------------------
{
	Input = NULL;
}
//-------------------------------------------------------------------------
vtkTriangleQualityRatio::~vtkTriangleQualityRatio()
//-------------------------------------------------------------------------
{
	Input = NULL;
	Output->Delete();

}
//-------------------------------------------------------------------------
void vtkTriangleQualityRatio::Update() 
//-------------------------------------------------------------------------
{
	// check inputs
	if (!Input) return;

	Output = vtkPolyData::New();
	Output->DeepCopy(Input);

	vtkDoubleArray *array=vtkDoubleArray::New();
	array->SetName("quality");
	Output->GetCellData()->SetScalars(array);
  array->Delete();
	// name variables
	double qualitySum = 0.0;
	double longestEdge;
	double perimeter;

	long cellsNumber = Input->GetNumberOfCells();

	double vertex1[3];
	double vertex2[3];
	double vertex3[3];

	double qualityRatioNormalize = 2.0 * sqrt(3.0);

	MaxRatio = 0.0;
	MeanRatio = 0.0;
	MinRatio = 999.0;

	for (long currentCell=0;currentCell<cellsNumber;currentCell++)
	{
		longestEdge = 0.0;
		perimeter = 0.0;

		vtkIdList *ID=Output->GetCell(currentCell)->GetPointIds();
		Output->GetPoint(ID->GetId(0),vertex1);
		Output->GetPoint(ID->GetId(1),vertex2);
		Output->GetPoint(ID->GetId(2),vertex3);

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
		qualityLocal = 0.5 * perimeter * longestEdge / area;
		qualityLocal = qualityRatioNormalize/qualityLocal;
		if(qualityLocal>0.00001)
		{
			qualitySum += qualityLocal;
			if(qualityLocal>MaxRatio)
				MaxRatio = qualityLocal;
			if(qualityLocal<MinRatio)
				MinRatio = qualityLocal;
		}
		else
		{
			qualitySum+=0.0;
			qualityLocal=0.0;
		}
		
		Output->GetCellData()->GetScalars()->InsertNextTuple1(qualityLocal);

	}
	MeanRatio = qualitySum / (cellsNumber);
}