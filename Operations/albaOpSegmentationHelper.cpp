/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentationHelper
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaOpSegmentationHelper.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "vtkRectilinearGrid.h"
#include <vector>

#define ROUND(X) floor((X)+0.5)

int neighboors[6][3] = { {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} };

//----------------------------------------------------------------------------
albaOpSegmentationHelper::albaOpSegmentationHelper()
{
	m_Segmentation = m_Volume = NULL;
	m_SegmetationSlice = m_SegmetationSlice = NULL;
	m_IsDrawing = false;
}

//----------------------------------------------------------------------------
albaOpSegmentationHelper::~albaOpSegmentationHelper()
{
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::SetVolumes(albaVMEVolumeGray *volume, albaVMEVolumeGray *segmentation)
{
	m_Volume = volume;
	m_Segmentation = segmentation;
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::SetSlices(vtkImageData *volumeSlice, vtkImageData *segmentationSlice)
{
	m_VolumeSlice = volumeSlice;
	m_SegmetationSlice = segmentationSlice;
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::SliceThreshold(double *threshold)
{
	vtkDataArray 			*inputScalars = m_VolumeSlice->GetPointData()->GetScalars();
	void *inputPointer = inputScalars->GetVoidPointer(0);
	vtkDataArray *outputScalars = m_SegmetationSlice->GetPointData()->GetScalars();
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);
	int nTuples = inputScalars->GetNumberOfTuples();
	int dataType = inputScalars->GetDataType();
	
	InternalTheshold(dataType, threshold, nTuples, inputPointer, outputPointer);

	outputScalars->Modified();
}
//----------------------------------------------------------------------------
void albaOpSegmentationHelper::VolumeThreshold(double *threshold)
{
	vtkDataSet * segmentation = m_Segmentation->GetOutput()->GetVTKData();
	vtkDataArray 	*inputScalars = m_Volume->GetOutput()->GetVTKData()->GetPointData()->GetScalars();
	void *inputPointer = inputScalars->GetVoidPointer(0);
	vtkDataArray *outputScalars = segmentation->GetPointData()->GetScalars();
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);
	int nTuples = inputScalars->GetNumberOfTuples();
	int dataType = inputScalars->GetDataType();

	InternalTheshold(dataType,threshold, nTuples, inputPointer, outputPointer);

	outputScalars->Modified();
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::VolumeThreshold(std::vector<AutomaticInfoRange> *rangesVector)
{
	vtkRectilinearGrid *volRG;
	vtkDataSet * segmentation = m_Segmentation->GetOutput()->GetVTKData();
	vtkDataSet * volumeVtkData = m_Volume->GetOutput()->GetVTKData();
	vtkDataArray 	*inputScalars =volumeVtkData->GetPointData()->GetScalars();
	void *inputPointer = inputScalars->GetVoidPointer(0);
	int nTuples = inputScalars->GetNumberOfTuples();
	int dataType = inputScalars->GetDataType();


	//TODO UPDATE no new scalars
	vtkUnsignedCharArray *outputScalars;
	vtkNEW(outputScalars);
	outputScalars->SetNumberOfTuples(nTuples);
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);

	int dims[3];
	if (volRG = vtkRectilinearGrid::SafeDownCast(volumeVtkData))
	{
		dims[0] = volRG->GetXCoordinates()->GetNumberOfTuples();
		dims[1] = volRG->GetYCoordinates()->GetNumberOfTuples();
		dims[2] = volRG->GetZCoordinates()->GetNumberOfTuples();
	}
	else
	{
		((vtkImageData *)volumeVtkData)->GetDimensions(dims);
	}

	int sliceSize = dims[0] * dims[1];

	for (int i = 0; i < rangesVector->size(); i++)
	{
		AutomaticInfoRange range = (*rangesVector)[i];
		int areaStart = (range.m_StartSlice - 1)*sliceSize;
		int areaSize = (range.m_EndSlice+1 - range.m_StartSlice)*sliceSize;

		InternalTheshold(dataType, range.m_Threshold, areaSize, inputPointer, outputPointer, areaStart);
	}

	segmentation->GetPointData()->SetScalars(outputScalars);

	vtkDEL(outputScalars);
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::InternalTheshold(int dataType, double *threshold, int n, void * inputPointer, unsigned char * outputPointer, int offset)
{
	switch (dataType)
	{
		case VTK_CHAR:
			InternalThreshold(threshold, n, (char*)inputPointer, outputPointer, offset);
			break;
		case VTK_UNSIGNED_CHAR:
			InternalThreshold(threshold, n, (unsigned char*)inputPointer, outputPointer, offset);
			break;
		case VTK_SHORT:
			InternalThreshold(threshold, n, (short*)inputPointer, outputPointer, offset);
			break;
		case VTK_UNSIGNED_SHORT:
			InternalThreshold(threshold, n, (unsigned short*)inputPointer, outputPointer, offset);
			break;
		case VTK_INT:
			InternalThreshold(threshold, n, (int *)inputPointer, outputPointer, offset);
			break;
		case VTK_UNSIGNED_INT:
			InternalThreshold(threshold, n, (unsigned int*)inputPointer, outputPointer, offset);
			break;
		case VTK_FLOAT:
			InternalThreshold(threshold, n, (float*)inputPointer, outputPointer, offset);
			break;
		case VTK_DOUBLE:  //NOTE: GPU is not allowed
			InternalThreshold(threshold, n, (double*)inputPointer, outputPointer, offset);
			break;
		default:
			return;
	}
}

// ----------------------------------------------------------------------------
template<typename DataType>
void albaOpSegmentationHelper::InternalThreshold(double *threshold, int n, DataType *inputScalars, unsigned char *outputScalars, int offset)
{
	for (int i = offset; i < offset+n; i++)
	{
		if (inputScalars[i] >= threshold[0] && inputScalars[i] <= threshold[1])
			outputScalars[i] = FULL_VALUE;
		else
			outputScalars[i] = EMPTY_VALUE;
	}
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::ApplySliceChangesToVolume(int slicePlane, int sliceIndex)
{
	vtkDataSet *segVolume = vtkDataSet::SafeDownCast(m_Segmentation->GetOutput()->GetVTKData());

	if (segVolume && m_SegmetationSlice)
	{
		int *volDims;
		volDims=((vtkImageData*)segVolume)->GetDimensions();
		vtkDataArray* volScalars = segVolume->GetPointData()->GetScalars();
		unsigned char *volScalarsPointer = (unsigned char *)volScalars->GetVoidPointer(0);
		unsigned char *segScalarPointer = (unsigned char *)m_SegmetationSlice->GetPointData()->GetScalars()->GetVoidPointer(0);;

		int numberOfSlices = 1;

		if (slicePlane == XY)
		{
			int z = (sliceIndex - 1);
			for (int x = 0; x < volDims[0]; x++)
				for (int y = 0; y < volDims[1]; y++)
				{
					int volPos = x + y*volDims[0] + z*volDims[0] * volDims[1];
					int slicePos = x + y*volDims[0] + 0;
					volScalarsPointer[volPos] = segScalarPointer[slicePos];
				}
		}
		else if (slicePlane == YZ)
		{
			int x = (sliceIndex - 1);
			for (int y = 0; y < volDims[1]; y++)
				for (int z = 0; z < volDims[2]; z++)
				{
					int volPos = x + y*volDims[0] + z*volDims[0] * volDims[1];
					int slicePos = 0 + y*numberOfSlices + z*numberOfSlices*volDims[1];
					volScalarsPointer[volPos] = segScalarPointer[slicePos];
				}
		}
		else if (slicePlane == XZ)
		{
			int y = (sliceIndex - 1);
			for (int z = 0; z < volDims[2]; z++)
				for (int x = 0; x < volDims[0]; x++)
				{
					int volPos = x + y*volDims[0] + z*volDims[0] * volDims[1];
					int slicePos = x + 0 + z*volDims[0] * numberOfSlices;
					volScalarsPointer[volPos] = segScalarPointer[slicePos];
				}
		}

		volScalars->Modified();
	}
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::CopyVolumeDataToSlice(int slicePlane, int sliceIndex)
{
	vtkDataSet *segVolume = vtkDataSet::SafeDownCast(m_Segmentation->GetOutput()->GetVTKData());

	if (segVolume && m_SegmetationSlice)
	{
		int *volDims;
		volDims = ((vtkImageData*)segVolume)->GetDimensions();
		vtkDataArray* volScalars = segVolume->GetPointData()->GetScalars();
		unsigned char *volScalarsPointer = (unsigned char *)volScalars->GetVoidPointer(0);
		vtkDataArray* sliceScalars = m_SegmetationSlice->GetPointData()->GetScalars();
		unsigned char *segScalarPointer = (unsigned char *)sliceScalars->GetVoidPointer(0);;

		int numberOfSlices = 1;

		if (slicePlane == XY)
		{
			int z = (sliceIndex - 1);
			for (int x = 0; x < volDims[0]; x++)
				for (int y = 0; y < volDims[1]; y++)
				{
					int volPos = x + y*volDims[0] + z*volDims[0] * volDims[1];
					int slicePos = x + y*volDims[0] + 0;
					segScalarPointer[slicePos] = volScalarsPointer[volPos];
				}
		}
		else if (slicePlane == YZ)
		{
			int x = (sliceIndex - 1);
			for (int y = 0; y < volDims[1]; y++)
				for (int z = 0; z < volDims[2]; z++)
				{
					int volPos = x + y*volDims[0] + z*volDims[0] * volDims[1];
					int slicePos = 0 + y*numberOfSlices + z*numberOfSlices*volDims[1];
					segScalarPointer[slicePos] = volScalarsPointer[volPos];
				}
		}
		else if (slicePlane == XZ)
		{
			int y = (sliceIndex - 1);
			for (int z = 0; z < volDims[2]; z++)
				for (int x = 0; x < volDims[0]; x++)
				{
					int volPos = x + y*volDims[0] + z*volDims[0] * volDims[1];
					int slicePos = x + 0 + z*volDims[0] * numberOfSlices;
					segScalarPointer[slicePos] = volScalarsPointer[volPos];
				}
		}

		sliceScalars->Modified();
	}
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::DrawBrush(double *pos, int slicePlane, int brushSize, int brushShape, bool erase)
{

	int sliceDim[3], point[2];
	m_SegmetationSlice->GetDimensions(sliceDim);
	if (pos != NULL)
		GetSlicePoint(slicePlane, pos, point);
	else
	{
		point[0] = m_LastPoint[0];
		point[1] = m_LastPoint[1];
	}

	if (m_IsDrawing)
	{
		//create a continuous line using Bresenham's algorithm 
		int x1 = point[0], y1 = point[1], x2 = m_LastPoint[0], y2 = m_LastPoint[1];
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1;
		dy = y2 - y1;
		dx1 = fabs(dx);
		dy1 = fabs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			DrawBrushPointer(x, y, sliceDim, slicePlane, brushSize, brushShape, erase);
			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}
					px = px + 2 * (dy1 - dx1);
				}
				DrawBrushPointer(x, y, sliceDim, slicePlane, brushSize, brushShape, erase);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			DrawBrushPointer(x, y, sliceDim, slicePlane, brushSize, brushShape, erase);
			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}
					py = py + 2 * (dx1 - dy1);
				}
				DrawBrushPointer(x, y, sliceDim, slicePlane, brushSize, brushShape, erase);
			}
		}
	}
	else
	{
		DrawBrushPointer(point[0], point[1], sliceDim, slicePlane, brushSize, brushShape, erase);
	}
	m_LastPoint[0] = point[0];
	m_LastPoint[1] = point[1];
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::DrawBrushPointer(int x, int y, int *sliceDim, int slicePlane, int brushSize, int brushShape, bool erase)
{
	int fillValue;
	double  distX, distY;

	vtkDataArray *outputScalars = m_SegmetationSlice->GetPointData()->GetScalars();
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);
		
	brushSize /= 2;
	fillValue = erase ? ERASER_VALUE : PENCIL_VALUE;


	int startX = MAX(0, x - brushSize);
	int endX   = MIN(sliceDim[0]-1,x + brushSize);
	int startY = MAX(0, y - brushSize);
	int endY   = MIN(sliceDim[1] - 1,y + brushSize);
	int brushSizeToTheSecond = brushSize * brushSize;

	if (brushShape == SQUARE_BRUSH_SHAPE)
	{
		for (int i = startY; i <= endY; i++)
			for (int j = startX; j <= endX; j++)
				outputPointer[i*sliceDim[0] + j] = fillValue;
	}
	else //CIRCLE_BRUSH_SHAPE
	{
		for (int i = startY; i <= endY; i++)
		{
			distY = i - y;
			for (int j = startX; j <= endX; j++)
			{
				distX = j -x;
				if ((distX*distX) + (distY*distY) <= brushSizeToTheSecond)
					outputPointer[i*sliceDim[0] + j] = fillValue;
			}
		}
	}
	outputScalars->Modified();
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::GetSlicePoint(int slicePlane, double * pos, int * sclicePoint)
{
	double bounds[6], slicePos[2], sliceSpacing[3];
	m_Segmentation->GetOutput()->GetVTKData()->GetBounds(bounds);
	m_SegmetationSlice->GetSpacing(sliceSpacing);

	if (slicePlane == XY)
	{
		slicePos[0] = pos[0] - bounds[0];
		slicePos[1] = pos[1] - bounds[2];
	}
	else if (slicePlane == YZ)
	{
		slicePos[0] = pos[1] - bounds[2];
		slicePos[1] = pos[2] - bounds[4];
	}
	else if (slicePlane == XZ)
	{
		slicePos[0] = pos[0] - bounds[0];
		slicePos[1] = pos[2] - bounds[4];
	}

	sclicePoint[0] = ROUND(slicePos[0] / sliceSpacing[0]);
	sclicePoint[1] = ROUND(slicePos[1] / sliceSpacing[1]);
}

 



//----------------------------------------------------------------------------
void albaOpSegmentationHelper::Connectivity3d(double * pos, int slicePlane, int currentSlice)
{
	unsigned char background, fillValue;
	int point[2],  dims[3];

	vtkImageData * segmentation = vtkImageData::SafeDownCast(m_Segmentation->GetOutput()->GetVTKData());
	segmentation->GetDimensions(dims);

	GetSlicePoint(slicePlane, pos, point);
	
	std::vector<volPoint> points, newPoints;
	volPoint firstPoint;

	if (slicePlane == 2)
	{
		firstPoint.x = point[0];
		firstPoint.y = point[1];
		firstPoint.z = currentSlice;
	}
	else if (slicePlane == 1)
	{
		firstPoint.x = point[0];
		firstPoint.y = currentSlice;
		firstPoint.z = point[1];
	}
	else
	{
		firstPoint.x = currentSlice;
		firstPoint.y = point[0];
		firstPoint.z = point[1];
	}
	points.push_back(firstPoint);
	int firstPointId = firstPoint.x + firstPoint.y*dims[0] + firstPoint.z*dims[0]*dims[1];

	vtkUnsignedCharArray *inputScalars = (vtkUnsignedCharArray*)segmentation->GetPointData()->GetScalars();
	unsigned char *inputPointer = (unsigned char*)inputScalars->GetVoidPointer(0);



	//click outside segmentation do not create output scalars
	if (inputPointer[firstPointId])
	{
		background = EMPTY_VALUE;
		fillValue = FULL_VALUE;
	}
	else
	{
		background = FULL_VALUE;
		fillValue = EMPTY_VALUE;
	}
		
	
	vtkUnsignedCharArray 	*outputScalars = inputScalars->NewInstance();
	outputScalars->SetNumberOfTuples(inputScalars->GetNumberOfTuples());
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);
	memset(outputPointer, background, sizeof(unsigned char)*inputScalars->GetNumberOfTuples());

	outputPointer[firstPointId] = fillValue;
	int x, y, z;
		
	while (points.size() > 0)
	{
		for (int i = 0; i < points.size(); i++)
		{
			volPoint p = points[i];

			for (int j = 0; j < 6; j++)
			{
				x = p.x + neighboors[j][0];
				y = p.y + neighboors[j][1];
				z = p.z + neighboors[j][2];


				if ((z < 0 || z >= dims[2]) || (y < 0 || y >= dims[1]) || (x < 0 || x >= dims[0]))
					continue;
				
				int id = z*dims[0] * dims[1] + y*dims[0] + x;

				if (outputPointer[id]!=fillValue && inputPointer[id]==fillValue)
				{
					outputPointer[id] = fillValue;
					volPoint newPoint = { x,y,z };
					newPoints.push_back(newPoint);
				}
			}
		}
		points.clear();
		points = newPoints;
		newPoints.clear();
	}

	segmentation->GetPointData()->SetScalars(outputScalars);
	vtkDEL(outputScalars);

	segmentation->Modified();
	m_Segmentation->Modified();
	m_SegmetationSlice->Modified();
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::EndDrawing(int autofill)
{
	m_IsDrawing = false;
	if (m_SegmetationSlice)
	{
		unsigned char *scalars = (unsigned char *) m_SegmetationSlice->GetScalarPointer();
		unsigned char drawValue;
		int  dims[3],currIndex;
		m_SegmetationSlice->GetDimensions(dims);

		if (autofill) //Autofill procedure
		{
			int minP[2] = { VTK_INT_MAX, VTK_INT_MAX }, maxP[2] = { VTK_INT_MIN, VTK_INT_MIN };

			//////////////////////////////////
			//Phase 1 getting drawing area.
			for (int x = 0; x < dims[0]; x++)
				for (int y = 0; y < dims[1]; y++) {
					currIndex = y*dims[0] + x;
					if (scalars[currIndex] == PENCIL_VALUE || scalars[currIndex] == ERASER_VALUE)
					{
						drawValue = scalars[currIndex];
						minP[0] = MIN(minP[0], x);
						minP[1] = MIN(minP[1], y);
						maxP[0] = MAX(maxP[0], x);
						maxP[1] = MAX(maxP[1], y);
					}
				}

			////////////////////////////////////
			//Phase 2 reverse fill from contours
			unsigned char *tmpScalars = new unsigned char[dims[0] * dims[1]];
			memset(tmpScalars, 0, sizeof(unsigned char)*dims[0] * dims[1]);

			//top contour
			for (int x = minP[0]; x <= maxP[0]; x++)
				InternalReverseFill(x, minP[1], dims, minP, maxP, scalars, tmpScalars, drawValue);
			//bottom contour
			for (int x = minP[0]; x <= maxP[0]; x++)
				InternalReverseFill(x, maxP[1], dims, minP, maxP, scalars, tmpScalars, drawValue);
			//left contour
			for (int y = minP[1]; y <= maxP[1]; y++)
				InternalReverseFill(minP[0], y, dims, minP, maxP, scalars, tmpScalars, drawValue);
			//right contour
			for (int y = minP[1]; y <= maxP[1]; y++)
				InternalReverseFill(maxP[0], y, dims, minP, maxP, scalars, tmpScalars, drawValue);

			////////////////////////////////////
			//Phase 3 apply unfilled values in tmp scalars to scalars
			unsigned char applyValue;

			if (drawValue == PENCIL_VALUE)
				applyValue = FULL_VALUE;
			else
				applyValue = EMPTY_VALUE;

			for (int y = minP[1]; y <= maxP[1]; y++)
				for (int x = minP[0]; x <= maxP[0]; x++)
				{
					currIndex = y*dims[0] + x;
					if (tmpScalars[currIndex] == 0)
						scalars[currIndex] = applyValue;
				}
				

			delete[] tmpScalars;
		}
		else //No autofill just update eraser values to empty or pencil values to full
		{
			int scalSize = dims[0] * dims[1];
			for (int i = 0; i < scalSize; i++)
				if (scalars[i] == ERASER_VALUE)
					scalars[i] = EMPTY_VALUE;
				else if (scalars[i] == PENCIL_VALUE)
					scalars[i] = FULL_VALUE;
		}
		
		m_SegmetationSlice->Modified();
	}
}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::InternalReverseFill(int x, int y, int *dims, int minP[2], int maxP[2], unsigned char *drawArea, unsigned char * tmpArea, unsigned char drawValue)
{
	int currIndex = y*dims[0] + x;

	//do not fill drawed area and already filled areas
	if (drawArea[currIndex] == drawValue || tmpArea[currIndex] == 1)
		return;

	tmpArea[currIndex] = 1;
	
	//Recursive fill up
	if (y - 1 >= minP[1])
		InternalReverseFill(x, y - 1, dims, minP, maxP, drawArea, tmpArea, drawValue);
	//Recursive fill down
	if (y + 1 <= maxP[1])
		InternalReverseFill(x, y + 1, dims, minP, maxP, drawArea, tmpArea, drawValue);
	//Recursive fill left
	if (x - 1 >= minP[0])
		InternalReverseFill(x - 1, y, dims, minP, maxP, drawArea, tmpArea, drawValue);
	//Recursive fill right
	if (x + 1 <= maxP[0])
		InternalReverseFill(x + 1, y, dims, minP, maxP, drawArea, tmpArea, drawValue);

}

//----------------------------------------------------------------------------
void albaOpSegmentationHelper::Fill(double *pos, int slicePlane, double thresholdPerc, bool erase)
{

	int fillValue, point[2];
	fillValue = erase ? EMPTY_VALUE : FULL_VALUE;
	vtkDataSet * vol = m_Volume->GetOutput()->GetVTKData();

	double *scalarRange = vol->GetScalarRange();
	double theshExtention = (scalarRange[1] - scalarRange[0])*thresholdPerc / 100.0;
	double pickedValue=vol->GetPointData()->GetScalars()->GetTuple(vol->FindPoint(pos))[0];
	double minValue = MAX(scalarRange[0], pickedValue - theshExtention);
	double maxValue = MIN(scalarRange[1], pickedValue + theshExtention);

	GetSlicePoint(slicePlane, pos, point);

	slicePoint firstPoint;
	firstPoint.x = point[0];
	firstPoint.y = point[1];

	vtkDataArray *outputScalars = m_SegmetationSlice->GetPointData()->GetScalars();
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);
	vtkDataArray 	*inputScalars = m_VolumeSlice->GetPointData()->GetScalars();
	void *inputPointer = inputScalars->GetVoidPointer(0);
	int dataType = inputScalars->GetDataType();
	
	switch (dataType)
	{
		case VTK_CHAR:
			InternalFill(firstPoint, minValue, maxValue, fillValue, (char*)inputPointer, outputPointer);
			break;
		case VTK_UNSIGNED_CHAR:
			InternalFill(firstPoint, minValue, maxValue, fillValue, (unsigned char*)inputPointer, outputPointer);
			break;
		case VTK_SHORT:
			InternalFill(firstPoint, minValue, maxValue, fillValue, (short*)inputPointer, outputPointer);
			break;
		case VTK_UNSIGNED_SHORT:
			InternalFill(firstPoint, minValue, maxValue, fillValue, (unsigned short*)inputPointer, outputPointer);
			break;
		case VTK_INT:
			InternalFill(firstPoint, minValue, maxValue, fillValue, (int *)inputPointer, outputPointer);
			break;
		case VTK_UNSIGNED_INT:
			InternalFill(firstPoint, minValue, maxValue, fillValue, (unsigned int*)inputPointer, outputPointer);
			break;
		case VTK_FLOAT:
			InternalFill(firstPoint, minValue, maxValue, fillValue, (float*)inputPointer, outputPointer);
			break;
		case VTK_DOUBLE:  //NOTE: GPU is not allowed
			InternalFill(firstPoint, minValue,maxValue, fillValue, (double*)inputPointer, outputPointer);
			break;
		default:
			return;
	}

	outputScalars->Modified();
}

//----------------------------------------------------------------------------
template<typename DataType>
void albaOpSegmentationHelper::InternalFill(slicePoint startPoint, double minValue, double maxValue, int fillValue, DataType *inputScalars, unsigned char *outputScalars)
{
	int  dims[3];
	m_SegmetationSlice->GetDimensions(dims);

	std::vector<slicePoint> points, newPoints;

	points.push_back(startPoint);

	outputScalars[startPoint.y*dims[0] + startPoint.x] = fillValue;
	
	while (points.size() > 0)
	{
		for (int i = 0; i < points.size(); i++)
		{
			slicePoint currPoint = points[i];

			//point on top
			if (currPoint.y - 1 >= 0)
			{
				int newPointID = (currPoint.y - 1)*dims[0] + currPoint.x;
				if (outputScalars[newPointID] != fillValue && inputScalars[newPointID] >= minValue && inputScalars[newPointID] <= maxValue)
				{
					slicePoint newPoint;
					newPoint.x = currPoint.x;
					newPoint.y = currPoint.y - 1;
					newPoints.push_back(newPoint);
					outputScalars[newPointID] = fillValue;
				}
			}

			//point on bottom
			if (currPoint.y + 1 < dims[1])
			{
				int newPointID = (currPoint.y + 1)*dims[0] + currPoint.x;
				if (outputScalars[newPointID] != fillValue && inputScalars[newPointID] >= minValue && inputScalars[newPointID] <= maxValue)
				{
					slicePoint newPoint;
					newPoint.x = currPoint.x;
					newPoint.y = currPoint.y + 1;
					newPoints.push_back(newPoint);
					outputScalars[newPointID] = fillValue;
				}

				//point on left
				if (currPoint.x - 1 >= 0)
				{
					int newPointID = currPoint.y*dims[0] + (currPoint.x - 1);
					if (outputScalars[newPointID] != fillValue && inputScalars[newPointID] >= minValue && inputScalars[newPointID] <= maxValue)
					{
						slicePoint newPoint;
						newPoint.x = currPoint.x - 1;
						newPoint.y = currPoint.y;
						newPoints.push_back(newPoint);
						outputScalars[newPointID] = fillValue;
					}
				}

				//point on left
				if (currPoint.x + 1 < dims[0])
				{
					int newPointID = currPoint.y*dims[0] + (currPoint.x + 1);
					if (outputScalars[newPointID] != fillValue && inputScalars[newPointID] >= minValue && inputScalars[newPointID] <= maxValue)
					{
						slicePoint newPoint;
						newPoint.x = currPoint.x + 1;
						newPoint.y = currPoint.y;
						newPoints.push_back(newPoint);
						outputScalars[newPointID] = fillValue;
					}
				}
			}

		}

		points.clear();
		points = newPoints;
		newPoints.clear();
	}
}
