/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageData.cxx,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFLargeImageData.h"
#include "vtkMAFDataArrayDescriptor.h"
#include "vtkMAFLargeDataProvider.h"

#include "vtkImageData.h"
#include "vtkGenericCell.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkVoxel.h"
#include "vtkPoints.h"
#include "vtkDataArray.h"
#include "vtkSmartPointer.h"


vtkCxxRevisionMacro(vtkMAFLargeImageData, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFLargeImageData);

//----------------------------------------------------------------------------
vtkMAFLargeImageData::vtkMAFLargeImageData()
{
	int idx;

	this->DataDescription = VTK_EMPTY;

	for (idx = 0; idx < 3; ++idx)
	{
		this->Dimensions[idx] = 0;
		this->Extent[idx*2] = 0;
		this->Extent[idx*2+1] = -1;    
		this->Increments[idx] = 0;
		this->Origin[idx] = 0.0;
		this->Spacing[idx] = 1.0;
	}

	this->NumberOfScalarComponents = 1;

	this->DataMask = 0xffff;
	this->DataLowerLeft = false;

	// Making the default double for structured points.
	this->ScalarType = VTK_VOID;
	this->SetScalarType(VTK_DOUBLE);	
	
	this->Snapshot = vtkImageData::New();
}

//----------------------------------------------------------------------------
vtkMAFLargeImageData::~vtkMAFLargeImageData()
{
	if (this->Snapshot != NULL) {
		this->Snapshot->Delete();
		this->Snapshot = NULL;
	}
}


#pragma region vtkLargeDataAPI
// Copy the geometric and topological structure of an object. Note that
// the invoking object and the object pointed to by the parameter ds must
// be of the same type.
// THIS METHOD IS NOT THREAD SAFE.
/*virtual*/ void vtkMAFLargeImageData::CopyStructure(vtkMAFLargeDataSet *ds)
{
	vtkMAFLargeImageData *sPts=(vtkMAFLargeImageData *)ds;

	for (int i = 0; i < 3; i++)
	{
		this->Extent[i] = sPts->Extent[i];
		this->Extent[i+3] = sPts->Extent[i+3];
		this->Dimensions[i] = sPts->Dimensions[i];
		this->Spacing[i] = sPts->Spacing[i];
		this->Origin[i] = sPts->Origin[i];
	}
	this->NumberOfScalarComponents = sPts->NumberOfScalarComponents;
	this->ScalarType = sPts->ScalarType;
	this->DataDescription = sPts->DataDescription;
}

// Description:
// Determine the number of cells composing the dataset.
// THIS METHOD IS THREAD SAFE
/*virtual*/ vtkIdType64 vtkMAFLargeImageData::GetNumberOfCells() 
{
	vtkIdType64 nCells=1;
	int *dims = this->GetDimensions();

	for (int i = 0; i < 3; i++)
	{
		if (dims[i] == 0)
			return 0;

		if (dims[i] > 1)
			nCells *= (dims[i]-1);
	}

	return nCells;
}

// Description:
// Get point coordinates with ptId such that: 0 <= ptId < NumberOfPoints.
// THIS METHOD IS NOT THREAD SAFE.
void vtkMAFLargeImageData::GetPoint(vtkIdType64 ptId, double x[3])
{
	int i, loc[3];
	double *origin = this->GetOrigin();
	double *spacing = this->GetSpacing();
	int *dims = this->GetDimensions();

	x[0] = x[1] = x[2] = 0.0;
	if (dims[0] == 0 || dims[1] == 0 || dims[2] == 0)
	{
		vtkErrorMacro("Requesting a point from an empty image.");
		return;
	}

	switch (this->DataDescription)
	{
	case VTK_EMPTY: 
		return ;

	case VTK_SINGLE_POINT: 
		loc[0] = loc[1] = loc[2] = 0;
		break;

	case VTK_X_LINE:
		loc[1] = loc[2] = 0;
		loc[0] = ptId;
		break;

	case VTK_Y_LINE:
		loc[0] = loc[2] = 0;
		loc[1] = ptId;
		break;

	case VTK_Z_LINE:
		loc[0] = loc[1] = 0;
		loc[2] = ptId;
		break;

	case VTK_XY_PLANE:
		loc[2] = 0;
		loc[0] = ptId % dims[0];
		loc[1] = ptId / dims[0];
		break;

	case VTK_YZ_PLANE:
		loc[0] = 0;
		loc[1] = ptId % dims[1];
		loc[2] = ptId / dims[1];
		break;

	case VTK_XZ_PLANE:
		loc[1] = 0;
		loc[0] = ptId % dims[0];
		loc[2] = ptId / dims[0];
		break;

	case VTK_XYZ_GRID:
		loc[0] = ptId % dims[0];
		loc[1] = (ptId / dims[0]) % dims[1];
		loc[2] = ptId / (dims[0]*dims[1]);
		break;
	}

	for (i=0; i<3; i++)
	{
		x[i] = origin[i] + (loc[i]+this->Extent[i*2]) * spacing[i];
	}
}

//----------------------------------------------------------------------------
// Get cell with cellId such that: 0 <= cellId < NumberOfCells. 	
// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
// THE DATASET IS NOT MODIFIED
// RELEASE NOTE: the current implementation is identical to vtkImageData
void vtkMAFLargeImageData::GetCell(vtkIdType64 cellId, vtkGenericCell *cell)
{
	vtkIdType64 npts, idx;
	int loc[3];
	int iMin, iMax, jMin, jMax, kMin, kMax;
	int *dims = this->GetDimensions();
	int d01 = dims[0]*dims[1];
	double *origin = this->GetOrigin();
	double *spacing = this->GetSpacing();
	double x[3];

	iMin = iMax = jMin = jMax = kMin = kMax = 0;

	if (dims[0] == 0 || dims[1] == 0 || dims[2] == 0)
	{
		vtkErrorMacro("Requesting a cell from an empty image.");
		cell->SetCellTypeToEmptyCell();
		return;
	}

	switch (this->DataDescription)
	{
	case VTK_EMPTY: 
		cell->SetCellTypeToEmptyCell();
		return;

	case VTK_SINGLE_POINT: // cellId can only be = 0
		cell->SetCellTypeToVertex();
		break;

	case VTK_X_LINE:
		iMin = cellId;
		iMax = cellId + 1;
		cell->SetCellTypeToLine();
		break;

	case VTK_Y_LINE:
		jMin = cellId;
		jMax = cellId + 1;
		cell->SetCellTypeToLine();
		break;

	case VTK_Z_LINE:
		kMin = cellId;
		kMax = cellId + 1;
		cell->SetCellTypeToLine();
		break;

	case VTK_XY_PLANE:
		iMin = cellId % (dims[0]-1);
		iMax = iMin + 1;
		jMin = cellId / (dims[0]-1);
		jMax = jMin + 1;
		cell->SetCellTypeToPixel();
		break;

	case VTK_YZ_PLANE:
		jMin = cellId % (dims[1]-1);
		jMax = jMin + 1;
		kMin = cellId / (dims[1]-1);
		kMax = kMin + 1;
		cell->SetCellTypeToPixel();
		break;

	case VTK_XZ_PLANE:
		iMin = cellId % (dims[0]-1);
		iMax = iMin + 1;
		kMin = cellId / (dims[0]-1);
		kMax = kMin + 1;
		cell->SetCellTypeToPixel();
		break;

	case VTK_XYZ_GRID:
		iMin = cellId % (dims[0] - 1);
		iMax = iMin + 1;
		jMin = (cellId / (dims[0] - 1)) % (dims[1] - 1);
		jMax = jMin + 1;
		kMin = cellId / ((dims[0] - 1) * (dims[1] - 1));
		kMax = kMin + 1;
		cell->SetCellTypeToVoxel();
		break;
	}

	// Extract point coordinates and point ids
	for (npts=0,loc[2]=kMin; loc[2]<=kMax; loc[2]++)
	{
		x[2] = origin[2] + (loc[2]+this->Extent[4]) * spacing[2]; 
		for (loc[1]=jMin; loc[1]<=jMax; loc[1]++)
		{
			x[1] = origin[1] + (loc[1]+this->Extent[2]) * spacing[1]; 
			for (loc[0]=iMin; loc[0]<=iMax; loc[0]++)
			{
				x[0] = origin[0] + (loc[0]+this->Extent[0]) * spacing[0]; 

				idx = loc[0] + loc[1]*dims[0] + loc[2]*d01;
				cell->PointIds->SetId(npts,idx);
				cell->Points->SetPoint(npts++,x);
			}
		}
	}
}

//----------------------------------------------------------------------------
// Get the bounds of the cell with cellId such that:
//		 0 <= cellId < NumberOfCells.
// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
// THE DATASET IS NOT MODIFIED
// RELEASE NOTE: the current implementation is identical to vtkImageData
void vtkMAFLargeImageData::GetCellBounds(vtkIdType64 cellId, double bounds[6])
{
	int loc[3], iMin, iMax, jMin, jMax, kMin, kMax;
	double x[3];
	double *origin = this->GetOrigin();
	double *spacing = this->GetSpacing();
	int *dims = this->GetDimensions();

	iMin = iMax = jMin = jMax = kMin = kMax = 0;

	if (dims[0] == 0 || dims[1] == 0 || dims[2] == 0)
	{
		vtkErrorMacro("Requesting cell bounds from an empty image.");
		bounds[0] = bounds[1] = bounds[2] = bounds[3] 
		= bounds[4] = bounds[5] = 0.0;
		return;
	}

	switch (this->DataDescription)
	{
	case VTK_EMPTY:
		return;

	case VTK_SINGLE_POINT: // cellId can only be = 0
		break;

	case VTK_X_LINE:
		iMin = cellId;
		iMax = cellId + 1;
		break;

	case VTK_Y_LINE:
		jMin = cellId;
		jMax = cellId + 1;
		break;

	case VTK_Z_LINE:
		kMin = cellId;
		kMax = cellId + 1;
		break;

	case VTK_XY_PLANE:
		iMin = cellId % (dims[0]-1);
		iMax = iMin + 1;
		jMin = cellId / (dims[0]-1);
		jMax = jMin + 1;
		break;

	case VTK_YZ_PLANE:
		jMin = cellId % (dims[1]-1);
		jMax = jMin + 1;
		kMin = cellId / (dims[1]-1);
		kMax = kMin + 1;
		break;

	case VTK_XZ_PLANE:
		iMin = cellId % (dims[0]-1);
		iMax = iMin + 1;
		kMin = cellId / (dims[0]-1);
		kMax = kMin + 1;
		break;

	case VTK_XYZ_GRID:
		iMin = cellId % (dims[0] - 1);
		iMax = iMin + 1;
		jMin = (cellId / (dims[0] - 1)) % (dims[1] - 1);
		jMax = jMin + 1;
		kMin = cellId / ((dims[0] - 1) * (dims[1] - 1));
		kMax = kMin + 1;
		break;
	}


	// carefully compute the bounds
	if (kMax >= kMin && jMax >= jMin && iMax >= iMin)
	{
		bounds[0] = bounds[2] = bounds[4] =  VTK_DOUBLE_MAX;
		bounds[1] = bounds[3] = bounds[5] = -VTK_DOUBLE_MAX;

		// Extract point coordinates
		for (loc[2]=kMin; loc[2]<=kMax; loc[2]++)
		{
			x[2] = origin[2] + (loc[2]+this->Extent[4]) * spacing[2]; 
			bounds[4] = (x[2] < bounds[4] ? x[2] : bounds[4]);
			bounds[5] = (x[2] > bounds[5] ? x[2] : bounds[5]);
		}
		for (loc[1]=jMin; loc[1]<=jMax; loc[1]++)
		{
			x[1] = origin[1] + (loc[1]+this->Extent[2]) * spacing[1]; 
			bounds[2] = (x[1] < bounds[2] ? x[1] : bounds[2]);
			bounds[3] = (x[1] > bounds[3] ? x[1] : bounds[3]);
		}
		for (loc[0]=iMin; loc[0]<=iMax; loc[0]++)
		{
			x[0] = origin[0] + (loc[0]+this->Extent[0]) * spacing[0]; 
			bounds[0] = (x[0] < bounds[0] ? x[0] : bounds[0]);
			bounds[1] = (x[0] > bounds[1] ? x[0] : bounds[1]);
		}
	}
	else
	{
		vtkMath::UninitializeBounds(bounds);
	}
}

// Get type of cell with cellId such that: 0 <= cellId < NumberOfCells.
// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
// THE DATASET IS NOT MODIFIED
int vtkMAFLargeImageData::GetCellType(vtkIdType64 vtkNotUsed(cellId))
{
	switch (this->DataDescription)
	{
	case VTK_EMPTY: 
		return VTK_EMPTY_CELL;

	case VTK_SINGLE_POINT: 
		return VTK_VERTEX;

	case VTK_X_LINE: case VTK_Y_LINE: case VTK_Z_LINE:
		return VTK_LINE;

	case VTK_XY_PLANE: case VTK_YZ_PLANE: case VTK_XZ_PLANE:
		return VTK_PIXEL;

	case VTK_XYZ_GRID:
		return VTK_VOXEL;

	default:
		vtkErrorMacro(<<"Bad data description!");
		return VTK_EMPTY_CELL;
	}
}

// Locate the closest point to the global coordinate x. Return the
// point id. If point id < 0; then no point found. (This may arise
// when point is outside of dataset.)
// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
// THE DATASET IS NOT MODIFIED
vtkIdType64 vtkMAFLargeImageData::FindPoint(double x[3])
{
	int i, loc[3];
	double d;
	double *origin = this->GetOrigin();
	double *spacing = this->GetSpacing();
	int *dims = this->GetDimensions();

	//
	//  Compute the ijk location
	//
	for (i=0; i<3; i++) 
	{
		d = x[i] - origin[i];
		loc[i] = (int) ((d / spacing[i]) + 0.5);
		if ( loc[i] < this->Extent[i*2] || loc[i] > this->Extent[i*2+1] )
		{
			return -1;
		} 
		// since point id is relative to the first point actually stored
		loc[i] -= this->Extent[i*2];
	}
	//
	//  From this location get the point id
	//
	return loc[2]*dims[0]*dims[1] + loc[1]*dims[0] + loc[0];
}

// Description:
// Locate cell based on global coordinate x. Parameters cell, cellId
// tol2, subId are ignored. Returns value >= 0
// if inside, < 0 otherwise.  The parametric coordinates are
// provided in pcoords[3]. The interpolation weights are returned in
// weights[]. (The number of weights is equal to the number of
// points in the found cell). 
// THIS METHOD IS NOT THREAD SAFE.
vtkIdType64 vtkMAFLargeImageData::FindCell(double x[3], vtkCell *vtkNotUsed(cell), 
									  vtkIdType64 vtkNotUsed(cellId),
									  double vtkNotUsed(tol2), 
									  int& subId, double pcoords[3], double *weights)
{
	int loc[3];
	int *dims = this->GetDimensions();

	if ( this->ComputeStructuredCoordinates(x, loc, pcoords) == 0 )
	{
		return -1;
	}

	vtkVoxel::InterpolationFunctions(pcoords,weights);

	//
	//  From this location get the cell id
	//
	subId = 0;
	return loc[2] * (dims[0]-1)*(dims[1]-1) +
		loc[1] * (dims[0]-1) + loc[0];
}

// Compute the data bounding box from data points.
// THIS METHOD IS NOT THREAD SAFE.
void vtkMAFLargeImageData::ComputeBounds()
{
	double *origin = this->GetOrigin();
	double *spacing = this->GetSpacing();

	if ( this->Extent[0] > this->Extent[1] || 
		this->Extent[2] > this->Extent[3] ||
		this->Extent[4] > this->Extent[5] )
	{
		vtkMath::UninitializeBounds(this->Bounds);
		return;
	}
	this->Bounds[0] = origin[0] + (this->Extent[0] * spacing[0]);
	this->Bounds[2] = origin[1] + (this->Extent[2] * spacing[1]);
	this->Bounds[4] = origin[2] + (this->Extent[4] * spacing[2]);

	this->Bounds[1] = origin[0] + (this->Extent[1] * spacing[0]);
	this->Bounds[3] = origin[1] + (this->Extent[3] * spacing[1]);
	this->Bounds[5] = origin[2] + (this->Extent[5] * spacing[2]);
}

#pragma endregion //vtkLargeDataAPI

//----------------------------------------------------------------------------
int* vtkMAFLargeImageData::GetDimensions()
{
	this->Dimensions[0] = this->Extent[1] - this->Extent[0] + 1;
	this->Dimensions[1] = this->Extent[3] - this->Extent[2] + 1;
	this->Dimensions[2] = this->Extent[5] - this->Extent[4] + 1;

	return this->Dimensions;
}


//----------------------------------------------------------------------------
// Convenience function computes the structured coordinates for a point x[3].
// The voxel is specified by the array ijk[3], and the parametric coordinates
// in the cell are specified with pcoords[3]. The function returns a 0 if the
// point x is outside of the volume, and a 1 if inside the volume.
int vtkMAFLargeImageData::ComputeStructuredCoordinates(double x[3], int ijk[3], 
													double pcoords[3])
{
	int i;
	double d, doubleLoc;
	double *origin = this->GetOrigin();
	double *spacing = this->GetSpacing();
	int *dims = this->GetDimensions();

	//
	//  Compute the ijk location
	//
	for (i=0; i<3; i++) 
	{
		d = x[i] - origin[i];
		doubleLoc = d / spacing[i];
		// Floor for negtive indexes.
		ijk[i] = (int) (floor(doubleLoc));
		if ( ijk[i] >= this->Extent[i*2] && ijk[i] < this->Extent[i*2 + 1] )
		{
			pcoords[i] = doubleLoc - (double)ijk[i];
		}

		else if ( ijk[i] < this->Extent[i*2] || ijk[i] > this->Extent[i*2+1] ) 
		{
			return 0;
		} 

		else //if ( ijk[i] == this->Extent[i*2+1] )
		{
			if (dims[i] == 1)
			{
				pcoords[i] = 0.0;
			}
			else
			{
				ijk[i] -= 1;
				pcoords[i] = 1.0;
			}
		}

	}
	return 1;
}

// Different ways to set the extent of the data array.  The extent
// should be set in the corelation with PointData and CellData dimensions
// The Extent is stored  in the order (X, Y, Z).
void vtkMAFLargeImageData::SetExtent(int x1, int x2, int y1, int y2, int z1, int z2)
{
	int ext[6];
	ext[0] = x1;
	ext[1] = x2;
	ext[2] = y1;
	ext[3] = y2;
	ext[4] = z1;
	ext[5] = z2;
	this->SetExtent(ext);
}

// Different ways to set the extent of the data array.  The extent
// should be set in the corelation with PointData and CellData dimensions
// The Extent is stored  in the order (X, Y, Z).
void vtkMAFLargeImageData::SetExtent(int *extent)
{
	int description;

	description = vtkStructuredData::SetExtent(extent, this->Extent);
	if ( description < 0 ) //improperly specified
	{
		vtkErrorMacro (<< "Bad Extent, retaining previous values");
	}

	if (description == VTK_UNCHANGED)
	{
		return;
	}

	this->DataDescription = description;

	this->Modified();
	this->ComputeIncrements();
}

//Computes the increments in line, plane and space
//The increments are computed from the NumberOfScalarComponents and the extent.
//NB: NumberOfScalarComponents should correlate with 
//information set in PointDataProvider->GetScalarDescriptor()
void vtkMAFLargeImageData::ComputeIncrements()
{
	int idx;
	int inc = this->GetNumberOfScalarComponents();

	for (idx = 0; idx < 3; ++idx)
	{
		this->Increments[idx] = inc;
		inc *= (this->Extent[idx*2+1] - this->Extent[idx*2] + 1);
	}
}

// Different ways to get the increments for moving around the data.
// incX is always returned with 0.  incY is returned with the
// increment needed to move from the end of one X scanline of data
// to the start of the next line.  incZ is filled in with the
// increment needed to move from the end of one image to the start
// of the next.  The proper way to use these values is to for a loop
// over Z, Y, X, C, incrementing the pointer by 1 after each
// component.  When the end of the component is reached, the pointer
// is set to the beginning of the next pixel, thus incX is properly set to 0.
void vtkMAFLargeImageData::GetContinuousIncrements(int extent[6], vtkIdType64 &incX,
												vtkIdType64 &incY, vtkIdType64 &incZ)
{
	int e0, e1, e2, e3;

	incX = 0;

	e0 = extent[0];
	if (e0 < this->Extent[0])
	{
		e0 = this->Extent[0];
	}
	e1 = extent[1];
	if (e1 > this->Extent[1])
	{
		e1 = this->Extent[1];
	}
	e2 = extent[2];
	if (e2 < this->Extent[2])
	{
		e2 = this->Extent[2];
	}
	e3 = extent[3];
	if (e3 > this->Extent[3])
	{
		e3 = this->Extent[3];
	}

	// Make sure the increments are up to date
	this->ComputeIncrements();

	incY = this->Increments[1] - (e1 - e0 + 1)*this->Increments[0];
	incZ = this->Increments[2] - (e3 - e2 + 1)*this->Increments[1];
}

//----------------------------------------------------------------------------
double vtkMAFLargeImageData::GetScalarTypeMin()
{
	switch (this->ScalarType)
	{
	case VTK_DOUBLE:
		return (double)(VTK_DOUBLE_MIN);
	case VTK_FLOAT:
		return (double)(VTK_FLOAT_MIN);
	case VTK_LONG:
		return (double)(VTK_LONG_MIN);
	case VTK_UNSIGNED_LONG:
		return (double)(VTK_UNSIGNED_LONG_MIN);
	case VTK_INT:
		return (double)(VTK_INT_MIN);
	case VTK_UNSIGNED_INT:
		return (double)(VTK_UNSIGNED_INT_MIN);
	case VTK_SHORT:
		return (double)(VTK_SHORT_MIN);
	case VTK_UNSIGNED_SHORT:
		return (double)(VTK_UNSIGNED_SHORT_MIN);
	case VTK_CHAR:
		return (double)(VTK_CHAR_MIN);
	case VTK_UNSIGNED_CHAR:
		return (double)(VTK_UNSIGNED_CHAR_MIN);
	default:
		vtkErrorMacro("Cannot handle scalar type " << this->ScalarType);
		return 0.0;
	}
}


//----------------------------------------------------------------------------
double vtkMAFLargeImageData::GetScalarTypeMax()
{
	switch (this->ScalarType)
	{
	case VTK_DOUBLE:
		return (double)(VTK_DOUBLE_MAX);
	case VTK_FLOAT:
		return (double)(VTK_FLOAT_MAX);
	case VTK_LONG:
		return (double)(VTK_LONG_MAX);
	case VTK_UNSIGNED_LONG:
		return (double)(VTK_UNSIGNED_LONG_MAX);
	case VTK_INT:
		return (double)(VTK_INT_MAX);
	case VTK_UNSIGNED_INT:
		return (double)(VTK_UNSIGNED_INT_MAX);
	case VTK_SHORT:
		return (double)(VTK_SHORT_MAX);
	case VTK_UNSIGNED_SHORT:
		return (double)(VTK_UNSIGNED_SHORT_MAX);
	case VTK_CHAR:
		return (double)(VTK_CHAR_MAX);
	case VTK_UNSIGNED_CHAR:
		return (double)(VTK_UNSIGNED_CHAR_MAX);
	default:
		vtkErrorMacro("Cannot handle scalar type " << this->ScalarType);
		return 0.0;
	}
}

//----------------------------------------------------------------------------
//Prepares scalars associated with points of this object.
//It modifies (or creates a new, if it does not exist) the current
//scalar descriptor in PointData
void vtkMAFLargeImageData::AllocateScalars()
{
	// if the scalar type has not been set then we have a problem
	if (this->ScalarType == VTK_VOID)
	{
		vtkErrorMacro("Attempt to allocate scdesc before scalar type was set!.");
		return;
	}

	// if we currently have scalar descriptor then just adjust the size
	vtkMAFDataArrayDescriptor* scdesc = this->PointDataProvider->GetScalarsDescriptor();
	if (scdesc == NULL)
	{
		scdesc = vtkMAFDataArrayDescriptor::New();
		scdesc->SetName("Scalars");

		this->PointDataProvider->SetScalarsDescriptor(scdesc);
		scdesc->Delete();	//SetScalarsDescriptor increased the reference count
	}
		
	scdesc->SetDataType(this->ScalarType);
	scdesc->SetNumberOfComponents(this->GetNumberOfScalarComponents());
	scdesc->SetNumberOfTuples(((vtkIdType64)(this->Extent[1] - this->Extent[0] + 1))*
		(this->Extent[3] - this->Extent[2] + 1)*
		(this->Extent[5] - this->Extent[4] + 1));
	
	// Since the execute method will be modifying the scdesc
	// directly.
	scdesc->Modified();
	PointDataProvider->Modified();
}

//----------------------------------------------------------------------------
void vtkMAFLargeImageData::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	int idx;
	int *dims = this->GetDimensions();

	os << indent << "ScalarType: " << this->ScalarType << endl;
	os << indent << "NumberOfScalarComponents: " << 
		this->NumberOfScalarComponents << endl;
	os << indent << "Spacing: (" << this->Spacing[0] << ", "
		<< this->Spacing[1] << ", "
		<< this->Spacing[2] << ")\n";
	os << indent << "Origin: (" << this->Origin[0] << ", "
		<< this->Origin[1] << ", "
		<< this->Origin[2] << ")\n";
	os << indent << "Dimensions: (" << dims[0] << ", "
		<< dims[1] << ", "
		<< dims[2] << ")\n";
	os << indent << "Increments: (" << this->Increments[0] << ", "
		<< this->Increments[1] << ", "
		<< this->Increments[2] << ")\n";
	os << indent << "Extent: (" << this->Extent[0];
	for (idx = 1; idx < 6; ++idx)
	{
		os << ", " << this->Extent[idx];
	}
	os << ")\n";
	os << indent << "WholeExtent: (" << this->WholeExtent[0];
	for (idx = 1; idx < 6; ++idx)
	{
		os << ", " << this->WholeExtent[idx];
	}
	os << ")\n";
}

//----------------------------------------------------------------------------
void vtkMAFLargeImageData::UpdateData()
{
	//this should update the providers information
	this->vtkDataObject::UpdateData();
	
	//check if the snapshot requires to be updated
	unsigned long mtime = this->GetMTime();
	if (mtime > m_SnapshotTime)
	{
		this->InvokeEvent(vtkCommand::StartEvent);

		vtkImageData* output = vtkImageData::SafeDownCast(this->Snapshot);
		if (output != NULL && this->VOI[0] <= this->VOI[1] && 
      this->VOI[2] <= this->VOI[3] && this->VOI[4] <= this->VOI[5]    //BES: 25.6.2008 - add check for invalid VOI
      )
		{
			this->ExecuteSnapshotInformation();
			output->AllocateScalars();
			this->ExecuteSnapshotData();
			
			this->Snapshot->Modified();
			m_SnapshotTime.Modified();
		}

		this->InvokeEvent(vtkCommand::EndEvent);
	}
}

//Copies the information to snapshot
/*virtual*/ void vtkMAFLargeImageData::ExecuteSnapshotInformation()
{
	//copy the information	
	vtkImageData* output = vtkImageData::SafeDownCast(this->Snapshot);

	int inExtent[6];
	this->GetVOI(inExtent);	//get the VOI

	//now, we have to modify it according to sampling scheme
	if (this->GetAutoSampleRate())
	{
		//compute sample rate from the size of volume and the memory limit		
		vtkIdType64 fsize = (inExtent[1] - inExtent[0] + 1);
		fsize *= (inExtent[3] - inExtent[2] + 1);
		fsize *= (inExtent[5] - inExtent[4] + 1);
		fsize *= GetIncrements()[0];
		fsize /= 1024;	//in KB

		unsigned long mlimit = this->GetMemoryLimit();
		if (fsize <= mlimit)
			this->SetSampleRate(1, 1, 1);		//we have enough memory to load everything
		else
		{
			//we will have to subsample it, because we do not have enough memory to load everything			
			
			int samp_rate = 1;
			vtkIdType64 fsize_new;

			do
			{
				fsize_new = fsize;
				samp_rate++;

				for (int i = 0; i < GetDataDimension(); i++) {
					fsize_new /= samp_rate;
				}
			}
			while (fsize_new > mlimit);

			this->SetSampleRate(samp_rate, samp_rate, samp_rate);
		}
	}


	//we need to set extent for our output according to sample rate
	//and we also needs to keep spacing (i.e., units) OK
	int samp[3];
	int outExtent[6];
	this->GetSampleRate(samp);	

	for (int i = 0; i < 3; i++) {
		outExtent[2*i] = 0;
		outExtent[2*i + 1] = ((inExtent[2*i + 1] - inExtent[2*i] + 1) / samp[i]) - 1;
	}

	output->SetExtent(outExtent);
	output->SetWholeExtent(outExtent);
	output->SetUpdateExtentToWholeExtent();
	//we have set a new Extent for the output snapshot

	//now we must update origin and spacing
	double sp[3], origin[3];
	this->GetSpacing(sp);		
	this->GetOrigin(origin);
	for (int i = 0; i < 3; i++) {
		origin[i] += this->VOI[2*i]*sp[i];
	}	

	for (int i = 0; i < 3; i++) {
		sp[i] = sp[i]*samp[i];
	}

	output->SetOrigin(origin);
	output->SetSpacing(sp);		

	//set scalar type, number of components and we are ready
	output->SetScalarType(this->ScalarType);
	output->SetNumberOfScalarComponents(this->NumberOfScalarComponents);
}

#pragma region READING_TEMPLATES
//----------------------------------------------------------------------------
// This function reads in one data of data.
// template to handle different data types.
template <class IT, class OT>
void vtkMAFLargeImageDataUpdate2(vtkMAFLargeImageData *self, vtkImageData *data,
								IT *inPtr, OT *outPtr)
{
	//Get the data extent (full not sampled) and its increments
	int dataExtent[6];
	vtkIdType64 dataIncr[3];
	self->GetVOI(dataExtent);
	self->GetIncrements(dataIncr);

	//Get the output extent (the sampled) and increments in the output data
	int outExtent[6], outIncr[3];	
	data->GetWholeExtent(outExtent);	
	data->GetIncrements(outIncr);

	//adjust the output pointer outPtr2 
	OT* outPtr2 = outPtr;
	if (outIncr[0] < 0) 	
		outPtr2 = outPtr2 - outIncr[0]*(dataExtent[1] - dataExtent[0]);

	if (outIncr[1] < 0) 	
		outPtr2 = outPtr2 - outIncr[1]*(dataExtent[3] - dataExtent[2]);

	if (outIncr[2] < 0) 	
		outPtr2 = outPtr2 - outIncr[2]*(dataExtent[5] - dataExtent[4]);	


	//now let us compute also information about the reading of source
	//gets the sample rate
	int samp_rate[3];
	self->GetSampleRate(samp_rate);

  //the number of elements needed for the representation of one pixel in the output buffer	
  int pixelSkip = dataIncr[0];

	//calculate number of pixels to be read in one block (at most one line is read)
	int pixelRead = dataExtent[1] - dataExtent[0] + 1;

	//number of elements to be loaded in one block (if whole data is loaded, this is dataIncr[1])
	vtkIdType64 streamRead = pixelRead * pixelSkip;  

	//number of elements to be skipped after one block is loaded calculated from the number 
	//of pixels remaining to the end of the line + number of lines to be skipped due to sampling
	vtkIdType64 streamSkip0 = (samp_rate[1] * dataIncr[1] - streamRead);	

	//if we need to read from the bottom up, recalculate something
	if (!self->GetDataLowerLeft()) 
	{
		//We are going to read file from the end => we need to recalculate our skip elements
		streamSkip0 = (-static_cast<long>(streamRead) 
			- samp_rate[1]*dataIncr[1]);
	}

	// create a buffer to hold a row of the data
	vtkSmartPointer<vtkDataArray> buffer = vtkDataArray::CreateDataArray(data->GetScalarType());
	buffer->Delete();	//NB: vtkSmartPointer increases ReferenceCount

	vtkMAFLargeDataProvider* pp = self->GetPointDataProvider();	

	////target and count is here for progress update
	unsigned long count = 0;
	unsigned long target = (outExtent[5] - outExtent[4] + 1);
	
	//z-plane reading
	unsigned short DataMask = self->GetDataMask();
	for (int zz = outExtent[4]; zz <= outExtent[5] && !self->AbortSnapshotExecute; zz++)
	{
		vtkIdType64 idxSeek	= self->GetDataIndex(dataExtent[0],
			dataExtent[2], dataExtent[4] + zz * samp_rate[2]);					

		//y-coordinates, read line
		OT* outPtr1 = outPtr2;
		for (int yy = outExtent[2]; yy <= outExtent[3]; yy++)
		{
			OT* outPtr0 = outPtr1;

			// read the row, as GetScalars work with tuples rather than with elements
      //we need to adjust the seek index
			pp->GetScalars(NULL, buffer.GetPointer(), idxSeek / pixelSkip, pixelRead);
			if (pixelRead != buffer->GetNumberOfTuples())
			{
				vtkGenericWarningMacro("GetScalars operation failed. row = " << yy
					<< ", Tried to Read = " << pixelRead
					<< ", Read = " << (int)buffer->GetNumberOfTuples()
					<< ", Skip0 = " << (int)streamSkip0
					<< ", FilePosHi = " << (long)(idxSeek >> 32)
					<< ", FilePosLo = " << (long)(idxSeek)
					);				
				return;
			}
			
			// copy the bytes into the typed data, i.e., process x-coordinate
			inPtr = (IT *)buffer->GetVoidPointer(0);
			for (int xx = outExtent[0]; xx <= outExtent[1]; xx++)
			{
				// Copy pixel into the output.
				if (DataMask == 0xffff)
				{
					for (int comp = 0; comp < pixelSkip; comp++)
					{
						outPtr0[comp] = (OT)(inPtr[comp]);
					}
				}
				else
				{
					// left over from short reader (what about other types.
					for (int comp = 0; comp < pixelSkip; comp++)
					{
						outPtr0[comp] = (OT)((short)(inPtr[comp]) & DataMask);
					}
				}

				// move to next pixel
				inPtr += (pixelSkip * samp_rate[0]);
				outPtr0 += outIncr[0];
			}

			
			// move to the next row in the file and data
			idxSeek += streamRead + streamSkip0;
			outPtr1 += outIncr[1];
		}

		// move to the next image in the file and data
		outPtr2 += outIncr[2];

//		if (!(count%target)) {
//			//progress update
			self->UpdateSnapshotProgress(count / (double)target);
//		}

		count++;
	}
}

//----------------------------------------------------------------------------
// This function reads in one data of one slice.
// template to handle different data types.
template <class T>
void vtkMAFLargeImageDataUpdate1(vtkMAFLargeImageData *self, vtkImageData *data, T *inPtr)
{
	void *outPtr;

	// Call the correct template function for the input
	outPtr = data->GetScalarPointer();
	switch (data->GetScalarType())
	{
		vtkTemplateMacro4(vtkMAFLargeImageDataUpdate2, self, data, inPtr, 
			(VTK_TT *)(outPtr));
	default:
		vtkGenericWarningMacro("Update1: Unknown data type\n");
	}  
}

#pragma endregion //READING_TEMPLATES


//Fills the snapshot with the data (size of snapshot is set in prior to this
//call in ExecuteSnapshotInformation
/*virtual*/ void vtkMAFLargeImageData::ExecuteSnapshotData()
{
	this->ComputeIncrements();

	// Call the correct template function for the output
	void* ptr = NULL;
	switch (this->GetScalarType())
	{
		vtkTemplateMacro3(vtkMAFLargeImageDataUpdate1, this, 
			(vtkImageData*)this->Snapshot, (VTK_TT *)(ptr));

	default:
		vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
	}
}

//computes the index base for the specified slice
//NB: public to be accessed from reading templates
vtkIdType64 vtkMAFLargeImageData::GetDataIndex(int i, int j, int k)
{
	int dataExtent[6];
	vtkIdType64 dataIncr[3];
	this->GetExtent(dataExtent);
	this->GetIncrements(dataIncr);

	// convert data extent into constants that can be used to seek.
	vtkIdType64 streamStart =
		(i - dataExtent[0]) * dataIncr[0];

	if (this->DataLowerLeft)
	{
		streamStart = streamStart + 
			(j - dataExtent[2]) * dataIncr[1];
	}
	else
	{
		streamStart = streamStart + 
			(dataExtent[3] - dataExtent[2] - j) * 
			dataIncr[1];
	}

	// handle three and four dimensional files
	if (this->GetDataDimension() >= 3)
	{
		streamStart = streamStart + 
			(k - dataExtent[4]) * dataIncr[2];
	}

	return streamStart;	
}