/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSet.cxx,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkDataSet.h"
#include "vtkMath.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkGenericCell.h"
#include "vtkCellTypes.h"

#include "vtkMAFLargeDataSet.h"
#include "vtkMAFLargeDataProvider.h"

vtkCxxRevisionMacro(vtkMAFLargeDataSet, "$Revision: 1.1.2.1 $");

#include "mafMemDbg.h"

//----------------------------------------------------------------------------
// Constructor with default bounds (0,1, 0,1, 0,1).
vtkMAFLargeDataSet::vtkMAFLargeDataSet ()
{
	vtkMath::UninitializeBounds(this->Bounds);

	//there is no snapshot at this moment
	Snapshot = NULL;
	PointDataProvider = CellDataProvider = NULL;

	this->SampleRate[0] = this->SampleRate[1] = this->SampleRate[2] = 1;
	this->AutoSampleRate = true;
	this->MemoryLimit = 16384;		//some magic constant, 16MB

	this->AbortSnapshotExecute = 0;
}

//----------------------------------------------------------------------------
vtkMAFLargeDataSet::~vtkMAFLargeDataSet ()
{
	if (PointDataProvider != NULL)
  {
		PointDataProvider->Delete();
    PointDataProvider = NULL;
  }

	if (CellDataProvider != NULL)
  {
		CellDataProvider->Delete();
    CellDataProvider = NULL;
  }
}

//----------------------------------------------------------------------------
// Compute the data bounding box from data points.
void vtkMAFLargeDataSet::ComputeBounds()
{
	int j;
	vtkIdType64 i;
	double x[3];

	if ( this->GetMTime() > this->ComputeTime )
	{
		if (this->GetNumberOfPoints())
		{
			this->GetPoint(0, x);
			this->Bounds[0] = x[0];
			this->Bounds[2] = x[1];
			this->Bounds[4] = x[2];
			this->Bounds[1] = x[0];
			this->Bounds[3] = x[1];
			this->Bounds[5] = x[2];
			for (i=1; i<this->GetNumberOfPoints(); i++)
			{
				this->GetPoint(i, x);
				for (j=0; j<3; j++)
				{
					if ( x[j] < this->Bounds[2*j] )
					{
						this->Bounds[2*j] = x[j];
					}
					if ( x[j] > this->Bounds[2*j+1] )
					{
						this->Bounds[2*j+1] = x[j];
					}
				}
			}
		}
		else
		{
			vtkMath::UninitializeBounds(this->Bounds);
		}
		this->ComputeTime.Modified();
	}
}

// Description:
// Convenience method to get the range of the DATA TYPE of scalar data (if there is any 
// scalar data). Returns the (min/max) range of combined point and cell data.
// If there are no point or cell scalars the method will return (0,1).
// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
// THE DATASET IS NOT MODIFIED
/*virtual*/ void vtkMAFLargeDataSet::GetScalarRange(double range[2])
{
	vtkMAFDataArrayDescriptor *ptScalars, *cellScalars;
	ptScalars = PointDataProvider == NULL ? NULL : PointDataProvider->GetScalarsDescriptor();
	cellScalars = CellDataProvider == NULL ? NULL : CellDataProvider->GetScalarsDescriptor();

	if ( ptScalars && cellScalars)
	{
		double r1[2], r2[2];
		ptScalars->GetDataTypeRange(r1);
		cellScalars->GetDataTypeRange(r2);
		range[0] = (r1[0] < r2[0] ? r1[0] : r2[0]);
		range[1] = (r1[1] > r2[1] ? r1[1] : r2[1]);
	}
	else if ( ptScalars )
	{
		ptScalars->GetDataTypeRange(range);
	}
	else if ( cellScalars )
	{
		cellScalars->GetDataTypeRange(range);
	}
	else
	{
		range[0] = 0.0;
		range[1] = 1.0;
	}
}

//----------------------------------------------------------------------------
// Return a pointer to the geometry bounding box in the form
// (xmin,xmax, ymin,ymax, zmin,zmax).
double *vtkMAFLargeDataSet::GetBounds()
{
	this->ComputeBounds();
	return this->Bounds;
}

//----------------------------------------------------------------------------
void vtkMAFLargeDataSet::GetBounds(double bounds[6])
{
	this->ComputeBounds();
	for (int i=0; i<6; i++)
	{
		bounds[i] = this->Bounds[i];
	}
}

//----------------------------------------------------------------------------
// Get the center of the bounding box.
double *vtkMAFLargeDataSet::GetCenter()
{	
	this->ComputeBounds();
	for (int i=0; i<3; i++)
	{
		this->Center[i] = (this->Bounds[2*i+1] + this->Bounds[2*i]) / 2.0;
	}
	return this->Center;
}

//----------------------------------------------------------------------------
void vtkMAFLargeDataSet::GetCenter(double center[3])
{	
	this->ComputeBounds();
	for (int i=0; i<3; i++)
	{
		center[i] = (this->Bounds[2*i+1] + this->Bounds[2*i]) / 2.0;
	}
}

//----------------------------------------------------------------------------
// Return the length of the diagonal of the bounding box.
double vtkMAFLargeDataSet::GetLength()
{
	double diff, l=0.0;
	int i;

	this->ComputeBounds();
	for (i=0; i<3; i++)
	{
		diff = static_cast<double>(this->Bounds[2*i+1]) - 
			static_cast<double>(this->Bounds[2*i]);
		l += diff * diff;
	}
	diff = sqrt(l);
	return diff;
}

//----------------------------------------------------------------------------
unsigned long int vtkMAFLargeDataSet::GetMTime()
{
	unsigned long mtime, result;

	result = vtkDataObject::GetMTime();
	if (this->PointDataProvider != NULL)
	{
		mtime = this->PointDataProvider->GetMTime();
		if (mtime > result)
			result = mtime;
	}

	if (this->CellDataProvider != NULL)
	{
		mtime = this->CellDataProvider->GetMTime();
		if (mtime > result)
			result = mtime;
	}

	return result;
}


//----------------------------------------------------------------------------
void vtkMAFLargeDataSet::GetCellNeighbors(vtkIdType64 cellId, vtkIdList *ptIds,
									   vtkIdList *cellIds)
{
	vtkIdType64 i, numPts;
	vtkIdList *otherCells = vtkIdList::New();
	otherCells->Allocate(VTK_CELL_SIZE);

	// load list with candidate cells, remove current cell
	this->GetPointCells(ptIds->GetId(0), cellIds);
	cellIds->DeleteId(cellId);

	// now perform multiple intersections on list
	if ( cellIds->GetNumberOfIds() > 0 )
	{
		for ( numPts=ptIds->GetNumberOfIds(), i=1; i < numPts; i++)
		{
			this->GetPointCells(ptIds->GetId(i), otherCells);
			cellIds->IntersectWith(*otherCells);
		}
	}

	otherCells->Delete();
}

//----------------------------------------------------------------------------
void vtkMAFLargeDataSet::GetCellTypes(vtkCellTypes *types)
{
	vtkIdType64 cellId, numCells = this->GetNumberOfCells();
	unsigned char type;

	types->Reset();
	for (cellId=0; cellId < numCells; cellId++)
	{
		type = this->GetCellType(cellId);
		if ( ! types->IsType(type) )
		{
			types->InsertNextType(type);
		}
	}
}


//----------------------------------------------------------------------------
// Default implementation. This is very slow way to compute this information.
// Subclasses should override this method for efficiency.
void vtkMAFLargeDataSet::GetCellBounds(vtkIdType64 cellId, double bounds[6])
{
	vtkGenericCell *cell = vtkGenericCell::New();

	this->GetCell(cellId, cell);
	cell->GetBounds(bounds);
	cell->Delete();
}

//----------------------------------------------------------------------------
unsigned long vtkMAFLargeDataSet::GetActualMemorySize()
{
	unsigned long size=this->vtkDataObject::GetActualMemorySize();
	size += this->Snapshot->GetActualMemorySize();
	return size;
}

//----------------------------------------------------------------------------
void vtkMAFLargeDataSet::ShallowCopy(vtkDataObject *dataObject)
{
	vtkMAFLargeDataSet *dataSet = vtkMAFLargeDataSet::SafeDownCast(dataObject);

	if ( dataSet != NULL )
	{
		this->InternalDataSetCopy(dataSet);
		this->Snapshot->ShallowCopy(dataSet->GetSnapshot());	
		this->PointDataProvider->ShallowCopy(dataSet->GetPointDataProvider());
		this->CellDataProvider->ShallowCopy(dataSet->GetCellDataProvider());
	}
	// Do superclass
	this->vtkDataObject::ShallowCopy(dataObject);
}

//----------------------------------------------------------------------------
void vtkMAFLargeDataSet::DeepCopy(vtkDataObject *dataObject)
{
	vtkMAFLargeDataSet *dataSet = vtkMAFLargeDataSet::SafeDownCast(dataObject);

	if ( dataSet != NULL )
	{
		this->InternalDataSetCopy(dataSet);
		this->Snapshot->DeepCopy(dataSet->GetSnapshot());
		this->PointDataProvider->DeepCopy(dataSet->GetPointDataProvider());
		this->CellDataProvider->DeepCopy(dataSet->GetCellDataProvider());
	}

	// Do superclass
	this->vtkDataObject::DeepCopy(dataObject);
}

//----------------------------------------------------------------------------
// This copies all the local variables (but not objects).
void vtkMAFLargeDataSet::InternalDataSetCopy(vtkMAFLargeDataSet *src)
{
	int idx;

	this->ComputeTime = src->ComputeTime;
	for (idx = 0; idx < 3; ++idx)
	{
		this->Bounds[2*idx] = src->Bounds[2*idx];
		this->Bounds[2*idx+1] = src->Bounds[2*idx+1];
		this->SampleRate[idx] = src->SampleRate[idx];
	}

	this->AutoSampleRate = src->AutoSampleRate;
	this->MemoryLimit = src->MemoryLimit;
}

// Description:
// Return the type of data object.
/*virtual*/ int vtkMAFLargeDataSet::GetDataObjectType()
{
	if (GetSnapshot() != NULL)
		return GetSnapshot()->GetDataObjectType();

	return VTK_DATA_SET;
}

//Sets the provider for point data
//NB: caller should Delete() the object when it is no longer needed
/*virtual*/ void vtkMAFLargeDataSet::SetPointDataProvider(vtkMAFLargeDataProvider* provider)
{
	if (PointDataProvider == provider)
		return;	//no change

	if (PointDataProvider != NULL)
		PointDataProvider->Delete();

	if ((PointDataProvider = provider) != NULL)
		PointDataProvider->Register(this);

	this->Modified();
}
//Sets the provider for point data
//NB: caller should Delete() the object when it is no longer needed
/*virtual*/ void vtkMAFLargeDataSet::SetCellDataProvider(vtkMAFLargeDataProvider* provider)
{
	if (CellDataProvider == provider)
		return;	//no change

	if (CellDataProvider != NULL)
		CellDataProvider->Delete();

	if ((CellDataProvider = provider) != NULL)
		CellDataProvider->Register(this);

	this->Modified();
}

//----------------------------------------------------------------------------
void vtkMAFLargeDataSet::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	double *bounds;

	os << indent << "Number Of Points: " << this->GetNumberOfPoints() << "\n";
	os << indent << "Number Of Cells: " << this->GetNumberOfCells() << "\n";
	
	bounds = this->GetBounds();	
	os << indent << "Bounds: \n";
	os << indent << "  Xmin,Xmax: (" <<bounds[0] << ", " << bounds[1] << ")\n";
	os << indent << "  Ymin,Ymax: (" <<bounds[2] << ", " << bounds[3] << ")\n";
	os << indent << "  Zmin,Zmax: (" <<bounds[4] << ", " << bounds[5] << ")\n";

	int sample[3];
	this->GetSampleRate(sample);
	os << indent << "Snapshot Sample Rate: \n";	
	os << indent << "  X: " << sample[0] << "\n";
	os << indent << "  Y: " << sample[1] << "\n";
	os << indent << "  Z: " << sample[2] << "\n";	
	os << indent << "  Autosampling: " << this->GetAutoSampleRate() << "\n";
	os << indent << "  Memory limit: " << this->GetMemoryLimit() << "\n";	

	os << indent << "Compute Time: " <<this->ComputeTime.GetMTime() << "\n";
	os << indent << "Release Data: " << (this->ReleaseDataFlag ? "On\n" : "Off\n");
}

