/*=========================================================================

 Program: MAF2Medical
 Module: vtkMAFLargeDataSet
 Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFLargeDataSet_h
#define __vtkMAFLargeDataSet_h

#include "vtkDataObject.h"
#include "vtkCommand.h"
#include "vtkDataSet.h"

#include "vtkMAFIdType64.h"

class vtkCell;
class vtkCellTypes;
class vtkGenericCell;
class vtkIdList;
class vtkMAFLargeDataProvider;

class VTK_vtkMAF_EXPORT vtkMAFLargeDataSet : public vtkDataObject
{
public:
	vtkTypeRevisionMacro(vtkMAFLargeDataSet,vtkDataObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Copy the geometric and topological structure of an object. Note that
	// the invoking object and the object pointed to by the parameter ds must
	// be of the same type.
	// THIS METHOD IS NOT THREAD SAFE.
	virtual void CopyStructure(vtkMAFLargeDataSet *ds) = 0;

	// Description:
	// Determine the number of points composing the dataset.
	// THIS METHOD IS THREAD SAFE
	virtual vtkIdType64 GetNumberOfPoints() {
		return 0; 	//no cells by default
	}

	// Description:
	// Determine the number of cells composing the dataset.
	// THIS METHOD IS THREAD SAFE
	virtual vtkIdType64 GetNumberOfCells() {
		return 0; 	//no cells by default
	}

	// Description:
	// Get point coordinates with ptId such that: 0 <= ptId < NumberOfPoints.
	// THIS METHOD IS NOT THREAD SAFE.
	virtual void GetPoint(vtkIdType64 ptId, double x[3]) = 0;			

	// Description:
	// Get cell with cellId such that: 0 <= cellId < NumberOfCells. 	
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual void GetCell(vtkIdType64 cellId, vtkGenericCell *cell) = 0;

	// Description:
	// Get the bounds of the cell with cellId such that:
	//     0 <= cellId < NumberOfCells.
	// A subclass may be able to determine the bounds of cell without using
	// an expensive GetCell() method. A default implementation is provided
	// that actually uses a GetCell() call.  This is to ensure the method
	// is available to all datasets.  Subclasses should override this method
	// to provide an efficient implementation.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual void GetCellBounds(vtkIdType64 cellId, double bounds[6]);

	// Description:
	// Get type of cell with cellId such that: 0 <= cellId < NumberOfCells.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual int GetCellType(vtkIdType64 cellId) = 0;

	// Description:
	// Get a list of types of cells in a dataset. The list consists of an array
	// of types (not necessarily in any order), with a single entry per type.
	// For example a dataset 5 triangles, 3 lines, and 100 hexahedra would
	// result a list of three entries, corresponding to the types VTK_TRIANGLE,
	// VTK_LINE, and VTK_HEXAHEDRON.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual void GetCellTypes(vtkCellTypes *types);
	
	// Description:
	// Topological inquiry to get points defining cell.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual void GetCellPoints(vtkIdType64 cellId, vtkIdList *ptIds) = 0;

	// Description:
	// Topological inquiry to get cells using point.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual void GetPointCells(vtkIdType64 ptId, vtkIdList *cellIds) = 0;

	// Description:
	// Topological inquiry to get all cells using list of points exclusive of
	// cell specified (e.g., cellId). Note that the list consists of only
	// cells that use ALL the points provided.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual void GetCellNeighbors(vtkIdType64 cellId, vtkIdList *ptIds, vtkIdList *cellIds);

	// Description:
	// Locate the closest point to the global coordinate x. Return the
	// point id. If point id < 0; then no point found. (This may arise
	// when point is outside of dataset.)
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual vtkIdType64 FindPoint(double x[3]) = 0;
	inline vtkIdType64 FindPoint(double x, double y, double z)
	{
		double xyz[3];
		xyz[0] = x; xyz[1] = y; xyz[2] = z;
		return this->FindPoint (xyz);
	}
	
	// Description:
	// Locate cell based on global coordinate x and tolerance
	// squared. If cell and cellId is non-NULL, then search starts from
	// this cell and looks at immediate neighbors.  Returns cellId >= 0
	// if inside, < 0 otherwise.  The parametric coordinates are
	// provided in pcoords[3]. The interpolation weights are returned in
	// weights[]. (The number of weights is equal to the number of
	// points in the found cell). Tolerance is used to control how close
	// the point is to be considered "in" the cell.
	// THIS METHOD IS NOT THREAD SAFE.
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual vtkIdType64 FindCell(double x[3], vtkCell *cell, vtkIdType64 cellId,
		double tol2, int& subId, double pcoords[3],
		double *weights) = 0;

	// Description:
	// This is a version of the above method that can be used with 
	// multithreaded applications. A vtkGenericCell must be passed in
	// to be used in internal calls that might be made to GetCell()
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual vtkIdType64 FindCell(double x[3], vtkCell *cell,
		vtkGenericCell *gencell, vtkIdType64 cellId,
		double tol2, int& subId, double pcoords[3],
		double *weights) = 0;
	
	// Description:
	// Datasets are composite objects and need to check each part for MTime
	// THIS METHOD IS THREAD SAFE
	unsigned long int GetMTime();

	// Description:
	// Compute the data bounding box from data points.
	// THIS METHOD IS NOT THREAD SAFE.
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual void ComputeBounds();

	// Description:
	// Return a pointer to the geometry bounding box in the form
	// (xmin,xmax, ymin,ymax, zmin,zmax).
	// THIS METHOD IS NOT THREAD SAFE.
	// NB: THIS OPERATION MAY BE VERY SLOW
	double *GetBounds();

	// Description:
	// Return a pointer to the geometry bounding box in the form
	// (xmin,xmax, ymin,ymax, zmin,zmax).
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	void GetBounds(double bounds[6]);

	// Description:
	// Get the center of the bounding box.
	// THIS METHOD IS NOT THREAD SAFE.
	// NB: THIS OPERATION MAY BE VERY SLOW
	double *GetCenter();

	// Description:
	// Get the center of the bounding box.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	void GetCenter(double center[3]);

	// Description:
	// Return the length of the diagonal of the bounding box.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	double GetLength();

	// Description:
	// Convenience method to get the range of the DATA TYPE of scalar data (if there is any 
	// scalar data). Returns the (min/max) range of combined point and cell data.
	// If there are no point or cell scalars the method will return (0,1).
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual void GetScalarRange(double range[2]);

	// Description:
	// Convenience method to get the range of the DATA TYPE of scalar data 
	//(if there is any scalar data). 
	// THIS METHOD IS NOT THREAD SAFE.
	inline double *GetScalarRange() {
		GetScalarRange(this->ScalarRange);
		return this->ScalarRange;
	}
	
	// Description:
	// Convenience method returns largest cell size in dataset. This is generally
	// used to allocate memory for supporting data structures.
	// THIS METHOD IS THREAD SAFE
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual int GetMaxCellSize() = 0;

	// Description:
	// Return the actual size of the data in kilobytes. This number
	// is valid only after the pipeline has updated. The memory size
	// returned is guaranteed to be greater than or equal to the
	// memory required to represent the data (e.g., extra space in
	// arrays, etc. are not included in the return value). THIS METHOD
	// IS THREAD SAFE.
	unsigned long GetActualMemorySize();

	// Description:
	// Return the type of data object.
	virtual int GetDataObjectType();

	// Description:
	// Shallow and Deep copy.
	void ShallowCopy(vtkDataObject *src);  
	void DeepCopy(vtkDataObject *src);	

public:
	//////////////////////////////////////////////////////////////////////////
	//SETTERS / GETTERS

	// Description:
	// Set the sampling rate in the i, j, and k directions. If the rate is >
	// 1, then the resulting VOI will be subsampled representation of the
	// input.  For example, if the SampleRate=(2,2,2), every other point will
	// be selected, resulting in a volume 1/8th the original size.
	vtkSetVector3Macro(SampleRate, int);
	vtkGetVectorMacro(SampleRate, int, 3);

	//Get/Set automatic calculation of sample rate from the memory limit
	vtkSetMacro(AutoSampleRate, bool);
	vtkGetMacro(AutoSampleRate, bool);
	vtkBooleanMacro(AutoSampleRate, bool);

	//Get/Set the memory limit in kilobytes for the snapshot of data
	vtkSetMacro(MemoryLimit, unsigned long);
	vtkGetMacro(MemoryLimit, unsigned long);

	//returns the snapshot of data
	//NB: The caller may not Delete the returned array ->
	//DO NOT USE SMART POINTERS FOR THE RETURNED REFERENCE
	virtual vtkDataSet* GetSnapshot() {
		return Snapshot;
	}

	//sets the dataset used for snapshot (may be NULL)
	virtual void SetSnapshot(vtkDataSet* snapshot)
	{
		if (this->Snapshot != NULL)
			this->Snapshot->UnRegister(this);

		this->Snapshot = snapshot;
		if (this->Snapshot != NULL)
			this->Snapshot->Register(this);

		this->Modified();
	}

	//returns the provider that can be used to access the data
	//associated with points
	//NB: The caller may not Delete the returned array ->
	//DO NOT USE SMART POINTERS FOR THE RETURNED REFERENCE
	virtual vtkMAFLargeDataProvider* GetPointDataProvider(){
		return PointDataProvider;
	}

	//Sets the provider for point data
	//NB: caller should Delete() the object when it is no longer needed
	virtual void SetPointDataProvider(vtkMAFLargeDataProvider* provider);

	//returns the provider that can be used to access the data
	//associated with cells
	//NB: The caller may not Delete the returned array ->
	//DO NOT USE SMART POINTERS FOR THE RETURNED REFERENCE
	virtual vtkMAFLargeDataProvider* GetCellDataProvider(){
		return CellDataProvider;
	}

	//Sets the provider for point data
	//NB: caller should Delete() the object when it is no longer needed
	virtual void SetCellDataProvider(vtkMAFLargeDataProvider* provider);



	vtkSetVector6Macro(VOI, int);
	vtkGetVector6Macro(VOI, int);

protected:
	// Constructor with default bounds (0,1, 0,1, 0,1).
	vtkMAFLargeDataSet();
	~vtkMAFLargeDataSet();  

protected:
	//It provides access to whole data set 
	vtkMAFLargeDataProvider* PointDataProvider;
	vtkMAFLargeDataProvider* CellDataProvider;
	vtkDataSet* Snapshot;	//A snapshot of the whole data, this is what should be visualized
	
	vtkTimeStamp ComputeTime; // Time at which bounds, center, etc. computed

	// (xmin,xmax, ymin,ymax, zmin,zmax) geometric bounds
	// stored in order to avoid ComputeBounds
	double Bounds[6];
	double Center[3];	//because of GetCenter method, not needed otherwise
	double ScalarRange[2];	//because of GetScalarRange method

	//Sample rate (in x,y and z) denotes the quality of the snapshot on output
	//1:1:1 is the highest quality
	int SampleRate[3];

	//if it is true (default), sample rate is automatically calculated from
	//the memory limit (see MemoryLimit)
	bool AutoSampleRate;

	//the memory limit in kilobytes for the snapshot of data
	unsigned long MemoryLimit;
	
	int VOI[6];

public:
	// if this variable is set to true, Snapshot construction will terminate 
	// left public for performance since it is used in inner loops
	int AbortSnapshotExecute;
	
	// Update the progress of the Snapshot construction. The parameter amount
	// should range between (0,1).
	inline void UpdateSnapshotProgress(double amount) {
		this->InvokeEvent(vtkCommand::ProgressEvent,(void *)&amount);
	}

private:
	void InternalDataSetCopy(vtkMAFLargeDataSet *src);  
private:
	vtkMAFLargeDataSet(const vtkMAFLargeDataSet&);	// Not implemented.
	void operator = (const vtkMAFLargeDataSet&);   // Not implemented.
	static vtkMAFLargeDataSet* New() { return NULL; };				// Not implemented. Abstract class.
};
#endif
