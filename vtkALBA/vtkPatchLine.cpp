/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkTriangleQualityRatio.cxx,v $
Language:  C++
Date:      $Date: 2011-05-26 08:33:31 $
Version:   $Revision: 1.4.2.2 $
Authors:   Gabriel Shanahan
==========================================================================
Copyright (c) 2012 University of West Bohemia (www.zcu.cz)
See the COPYINGS file for license details 
=========================================================================
*/
#pragma warning(push)
#pragma warning(disable: 4996)
#include "vtkPatchLine.h"
#include "vtkSetGet.h"
#include "vtkObjectFactory.h"
#include "vtkCleanPolyData.h"
#include "vtkPolyData.h"
#include "vtkALBASmartPointer.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkPolyLine.h"
#include <list>
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#pragma warning(pop)


vtkStandardNewMacro(vtkPatchLine);

int vtkPatchLine::RequestData(vtkInformation *vtkNotUsed(request),	vtkInformationVector **inputVector,	vtkInformationVector *outputVector)
{	
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkDataObject *output = vtkDataObject::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	
	//Remove duplicate points etc.
	vtkALBASmartPointer<vtkCleanPolyData> cleaner;
	cleaner->SetInputData(input);
	cleaner->Update();  
	vtkPolyData* source = cleaner->GetOutput();
	
	//source->Register(NULL); - not needed, reference is added by SmartPointer
	cleaner->SetOutput(NULL);
	
	if(source->GetNumberOfPoints() <= 2)
	{
		vtkSmartPointer<vtkPolyData> newLine = vtkPolyData::SafeDownCast(output);
		newLine->SetPoints(source->GetPoints());
		newLine->SetLines(source->GetLines());
		return 1;
	}
	
	vtkSmartPointer<vtkCellArray> origLines = source->GetLines();

	int nNumPoints = source->GetNumberOfPoints();
	int nNumLines = source->GetNumberOfLines();

	//Contains id of the starting point of each segment making up the polyline. Array will be used for constant time random access
	std::list<vtkIdType> beginVertices(nNumPoints);
	std::list<vtkIdType>::iterator* beginVerticesArray = new std::list<vtkIdType>::iterator [nNumPoints];

	//Contains id of the end point of each segment making up the polyline. Array will be used for constant time random access
	std::list<vtkIdType> endVertices(nNumPoints);
	std::list<vtkIdType>::iterator* endVerticesArray = new std::list<vtkIdType>::iterator [nNumPoints];

	//Will contain id's making up the connected polyline, excluding the starting point of the polyline
	vtkIdType* segments = new vtkIdType[nNumPoints];
	//Id of starting point of the polyline
	vtkIdType startingPoint;

	//Initialize iterators in arrays to point to respective id's
	int i = 0;
	for(std::list<vtkIdType>::iterator beg_it = beginVertices.begin(), end_it = endVertices.begin(); beg_it != beginVertices.end() || end_it != endVertices.end(); beg_it++, end_it++)
	{
		*beg_it = i;
		*end_it = i++;
		beginVerticesArray[*beg_it] = beg_it;
		endVerticesArray[*end_it] = end_it;
	}
	
	//Remove first and last (no points will be connect to the first and no lines will start in the last)
	beginVertices.erase(beginVerticesArray[nNumPoints-1]);
	endVertices.erase(endVerticesArray[0]);
	
	//Erase id's of points that are already starting/end points and set the value of 'segments'
	for(int i = 0; i < nNumLines; i++)
	{
		vtkIdType npts;
		vtkIdType* pts;
		origLines->GetNextCell(npts, pts);

		if(i == 0)
			startingPoint = pts[0];

		for(int i = 0; i < npts-1; i++)
		{
			beginVertices.erase(beginVerticesArray[pts[i]]);
			endVertices.erase(endVerticesArray[pts[i+1]]);

			segments[pts[i]] = pts[i+1];
		}
	}
	
	//Find apropriate endpoints for leftover starting points
	for(std::list<vtkIdType>::iterator beg_it = beginVertices.begin(); beg_it != beginVertices.end(); beg_it++)
	{
		double minDistance = DBL_MAX;
		double begPtCoords[3];
		source->GetPoint(*beg_it, begPtCoords);
		std::list<vtkIdType>::iterator bestEnd_it;
		for(std::list<vtkIdType>::iterator end_it = endVertices.begin(); end_it != endVertices.end(); end_it++)
		{
			double endPtCoords[3];
			source->GetPoint(*end_it, endPtCoords);
			double dDistance = vtkMath::Distance2BetweenPoints(begPtCoords, endPtCoords);
			bestEnd_it = (dDistance < minDistance) ? end_it : bestEnd_it;
			minDistance = (dDistance < minDistance) ? dDistance : minDistance;
		}
		
		segments[*beg_it] = *bestEnd_it;
		endVertices.erase(bestEnd_it);
	}

	vtkALBASmartPointer<vtkPolyLine> polyLine;
	polyLine->GetPointIds()->SetNumberOfIds(nNumPoints);

	//Set starting point of polyline
	polyLine->GetPointIds()->SetId(0, startingPoint);
	
	//Shift i by one so as to accommodate 'startingPoint'
	for(int i = 0; i < nNumPoints-1; i++)
		polyLine->GetPointIds()->SetId(i+1, segments[polyLine->GetPointIds()->GetId(i)]); //so as to leave out last points in 'segments', which isn't connected to anything

	vtkALBASmartPointer<vtkCellArray> cells;
	cells->InsertNextCell(polyLine->GetPointIds());

	vtkSmartPointer<vtkPolyData> newLine = vtkPolyData::SafeDownCast(output);
	newLine->SetPoints(source->GetPoints());
	newLine->SetLines(cells.GetPointer());

	delete[] beginVerticesArray;
	delete[] endVerticesArray;
	delete[] segments;

	return 1;
}