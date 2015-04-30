/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageData.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:01 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkMAFLargeImageData_h
#define __vtkMAFLargeImageData_h

#include "vtkStructuredData.h" // Needed for inline methods
#include "vtkMAFLargeDataSet.h"
#include "vtkMAFDataArrayDescriptor.h"

class vtkDataArray;
class vtkLine;
class vtkPixel;
class vtkVertex;
class vtkVoxel;

#pragma warning(disable: 4068)	//pragma region is unsupported in VS2003

class MAF_EXPORT vtkMAFLargeImageData : public vtkMAFLargeDataSet
{
public:
	static vtkMAFLargeImageData *New();

	vtkTypeRevisionMacro(vtkMAFLargeImageData,vtkMAFLargeDataSet);
	virtual void PrintSelf(ostream& os, vtkIndent indent);

#pragma region vtkLargeDataAPI
	// Description:
	// Determine the number of points composing the dataset.
	// THIS METHOD IS THREAD SAFE
	inline virtual vtkIdType64 GetNumberOfPoints() {
		int* dims = this->GetDimensions();
		return ((vtkIdType64)dims[0])*dims[1]*dims[2];
	}

	// Description:
	// Determine the number of cells composing the dataset.
	// THIS METHOD IS THREAD SAFE
	virtual vtkIdType64 GetNumberOfCells();

	// Description:
	// Get point coordinates with ptId such that: 0 <= ptId < NumberOfPoints.
	// THIS METHOD IS NOT THREAD SAFE.
	virtual void GetPoint(vtkIdType64 ptId, double x[3]) ;			

	// Description:
	// Get cell with cellId such that: 0 <= cellId < NumberOfCells. 	
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual void GetCell(vtkIdType64 cellId, vtkGenericCell *cell) ;

	// Description:
	// Get the bounds of the cell with cellId such that:
	//		 0 <= cellId < NumberOfCells.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual void GetCellBounds(vtkIdType64 cellId, double bounds[6]);

	// Description:
	// Get type of cell with cellId such that: 0 <= cellId < NumberOfCells.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	virtual int GetCellType(vtkIdType64 cellId) ;

	// Description:
	// Topological inquiry to get points defining cell.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual void GetCellPoints(vtkIdType64 cellId, vtkIdList *ptIds) {
		vtkStructuredData::GetCellPoints(cellId, ptIds,
			this->DataDescription, this->GetDimensions());
	}

	// Description:
	// Topological inquiry to get cells using point.
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual void GetPointCells(vtkIdType64 ptId, vtkIdList *cellIds) {
		vtkStructuredData::GetPointCells(ptId,cellIds,this->GetDimensions());
	}

	// Description:
	// Locate the closest point to the global coordinate x. Return the
	// point id. If point id < 0; then no point found. (This may arise
	// when point is outside of dataset.)
	// THIS METHOD IS THREAD SAFE IF FIRST CALLED FROM A SINGLE THREAD AND
	// THE DATASET IS NOT MODIFIED
	// NB: THIS OPERATION MAY BE VERY SLOW
	virtual vtkIdType64 FindPoint(double x[3]);

	// Description:
	// Locate cell based on global coordinate x. Parameters cell, cellId
	// tol2, subId are ignored. Returns value >= 0
	// if inside, < 0 otherwise.  The parametric coordinates are
	// provided in pcoords[3]. The interpolation weights are returned in
	// weights[]. (The number of weights is equal to the number of
	// points in the found cell). 
	// THIS METHOD IS NOT THREAD SAFE.
	virtual vtkIdType64 FindCell(double x[3], vtkCell *cell, vtkIdType64 cellId,
		double tol2, int& subId, double pcoords[3], double *weights);

	// Description:
	// This is a version of the above method that can be used with 
	// multithreaded applications. Parameters cell, gencell, cellId, tol2,
	// and subId are ignored in the default implementation.
	virtual vtkIdType64 FindCell(double x[3], vtkCell *cell,
		vtkGenericCell *gencell, vtkIdType64 cellId,
		double tol2, int& subId, double pcoords[3], double *weights) {
		return FindCell( x, (vtkCell *)NULL, 0, 0.0, subId, pcoords, weights );
	}

	// Description:
	// Compute the data bounding box from data points.
	// THIS METHOD IS NOT THREAD SAFE.
	virtual void ComputeBounds();

	// Convenience method returns largest cell size in dataset. This is generally
	// used to allocate memory for supporting data structures.
	virtual int GetMaxCellSize() {
		return 8; //voxel is the largest
	}; 

	// Copy the geometric and topological structure of an object. Note that
	// the invoking object and the object pointed to by the parameter ds must
	// be of the same type.
	// THIS METHOD IS NOT THREAD SAFE.
	virtual void CopyStructure(vtkMAFLargeDataSet *ds);

		// Description:
	// Return what type of dataset this is.
	virtual int GetDataObjectType() {
		return VTK_IMAGE_DATA;
	};
#pragma endregion

	// Description:
	// Set dimensions of structured points dataset.
	inline void SetDimensions(int i, int j, int k) {
		SetExtent(0, i-1, 0, j-1, 0, k-1);
	}

	// Description:
	// Set dimensions of structured points dataset.
	inline void SetDimensions(int dims[3]) {
		SetExtent(0, dims[0]-1, 0, dims[1]-1, 0, dims[2]-1);
	}

	// Description:
	// Get dimensions of this structured points dataset.
	// Dimensions are computed from Extents during this call.
	int *GetDimensions();
	inline void GetDimensions(int dOut[3]) {
		int *dims = this->GetDimensions();
		dOut[0] = dims[0]; dOut[1] = dims[1]; dOut[2] = dims[2];  
	}

	// Description:
	// Convenience function computes the structured coordinates for a point x[3].
	// The voxel is specified by the array ijk[3], and the parametric coordinates
	// in the cell are specified with pcoords[3]. The function returns a 0 if the
	// point x is outside of the volume, and a 1 if inside the volume.
	int ComputeStructuredCoordinates(double x[3], int ijk[3], double pcoords[3]);


	// Description:
	// Return the dimensionality of the data.
	inline int GetDataDimension() {
		return vtkStructuredData::GetDataDimension(this->DataDescription);
	}

	// Description:
	// Given a location in structured coordinates (i-j-k), return the point id.
	vtkIdType64 ComputePointId(int ijk[3]) {
		return vtkStructuredData::ComputePointId(this->GetDimensions(),ijk);
	};

	// Description:
	// Given a location in structured coordinates (i-j-k), return the cell id.
	vtkIdType64 ComputeCellId(int ijk[3]) {
		return vtkStructuredData::ComputeCellId(this->GetDimensions(),ijk);
	};

	// Description:
	// Different ways to set the extent of the data array.  The extent
	// should be set in the correlation with PointData and CellData dimensions
	// see: PointDataProvider->GetScalarDescriptor()
	// The Extent is stored  in the order (X, Y, Z).
	void SetExtent(int extent[6]);
	void SetExtent(int x1, int x2, int y1, int y2, int z1, int z2);
	vtkGetVector6Macro(Extent,int);

protected:
	//Computes the increments in line, plane and space
	//The increments are computed from the NumberOfScalarComponents and the extent.
	//NB: NumberOfScalarComponents should correlate with 
	//information set in PointDataProvider->GetScalarDescriptor()
	void ComputeIncrements();
public:

	// Description:
	// Different ways to get the increments for moving around the data.
	// GetIncrements() calls ComputeIncrements() to ensure the increments are
	// up to date.
	inline vtkIdType64 *GetIncrements() {
		this->ComputeIncrements();
		return this->Increments;
	}

	inline void GetIncrements(vtkIdType64 &incX, vtkIdType64 &incY, vtkIdType64 &incZ) {
		this->ComputeIncrements(); incX = this->Increments[0];
		incY = this->Increments[1];	incZ = this->Increments[2];
	}

	inline void GetIncrements(vtkIdType64 inc[3]) {
		this->ComputeIncrements(); inc[0] = this->Increments[0]; 
		inc[1] = this->Increments[1]; inc[2] = this->Increments[2];
	}

	// Description:
	// Different ways to get the increments for moving around the data.
	// incX is always returned with 0.  incY is returned with the
	// increment needed to move from the end of one X scanline of data
	// to the start of the next line.  incZ is filled in with the
	// increment needed to move from the end of one image to the start
	// of the next.  The proper way to use these values is to for a loop
	// over Z, Y, X, C, incrementing the pointer by 1 after each
	// component.  When the end of the component is reached, the pointer
	// is set to the beginning of the next pixel, thus incX is properly set to 0.
	void GetContinuousIncrements(int extent[6], vtkIdType64 &incX, vtkIdType64 &incY, vtkIdType64 &incZ);

	// Description:
	// These returns the minimum and maximum values the ScalarType can hold
	// without overflowing.
	double GetScalarTypeMin();
	double GetScalarTypeMax();

	// Description:
	// Set the size of the scalar type in bytes.
	inline int GetScalarSize() {
		return vtkMAFDataArrayDescriptor::GetDataTypeSize(this->ScalarType);
	}

	// Description:
	// Set the spacing (width,height,length) of the cubical cells that
	// compose the data set.
	vtkSetVector3Macro(Spacing,double);
	vtkGetVector3Macro(Spacing,double);

	// Description:
	// Set the origin of the data. The origin plus spacing determine the
	// position in space of the points.
	vtkSetVector3Macro(Origin,double);
	vtkGetVector3Macro(Origin,double);

	// Description:
	// Set/Get the data scalar type (i.e VTK_DOUBLE).
	void SetScalarTypeToFloat(){this->SetScalarType(VTK_FLOAT);};
	void SetScalarTypeToDouble(){this->SetScalarType(VTK_DOUBLE);};
	void SetScalarTypeToInt(){this->SetScalarType(VTK_INT);};
	void SetScalarTypeToUnsignedInt()
	{this->SetScalarType(VTK_UNSIGNED_INT);};
	void SetScalarTypeToLong(){this->SetScalarType(VTK_LONG);};
	void SetScalarTypeToUnsignedLong()
	{this->SetScalarType(VTK_UNSIGNED_LONG);};
	void SetScalarTypeToShort(){this->SetScalarType(VTK_SHORT);};
	void SetScalarTypeToUnsignedShort()   
	{this->SetScalarType(VTK_UNSIGNED_SHORT);};
	void SetScalarTypeToUnsignedChar()
	{this->SetScalarType(VTK_UNSIGNED_CHAR);};
	void SetScalarTypeToChar()
	{this->SetScalarType(VTK_CHAR);};
	vtkSetMacro(ScalarType, int);
	inline int GetScalarType() {
		return ScalarType;
	}
	const char* GetScalarTypeAsString() { return vtkImageScalarTypeNameMacro ( this->GetScalarType() ); };

	// Description:
	// Set/Get the number of scalar components for points.
	inline void SetNumberOfScalarComponents( int n ) {
		this->NumberOfScalarComponents = n;
		this->ComputeIncrements();
	}
	vtkGetMacro(NumberOfScalarComponents,int);

	// Description:
	// The extent type is a 3D extent
	inline int GetExtentType() { 
		return VTK_3D_EXTENT; 
	};

	//Get/sets the data mask used to mask every byte read
	// Set/Get the Data mask.
	vtkGetMacro(DataMask, unsigned short);
	vtkSetMacro(DataMask, unsigned short);	

	// Description:
	// Set/Get whether the data comes from the file starting in the lower left
	// corner or upper left corner.
	vtkBooleanMacro(DataLowerLeft, bool);
	vtkGetMacro(DataLowerLeft, bool);
	vtkSetMacro(DataLowerLeft, bool);


	// Description:
	// This update method will supply the ghost level arrays if they are requested.
	virtual void UpdateData();

	//Prepares scalars associated with points of this object.
	//It modifies (or creates a new, if it does not exist) the current
	//scalar descriptor in PointData
	void AllocateScalars();

	//computes the index base for the specified slice
	//NB: public to be accessed from reading templates
	vtkIdType64 GetDataIndex(int i, int j, int k);

protected:
	//Copies the information to snapshot
	virtual void ExecuteSnapshotInformation();

	//Fills the snapshot with the data (size of snapshot is set in prior to this
	//call in ExecuteSnapshotInformation
	virtual void ExecuteSnapshotData();

protected:
	vtkMAFLargeImageData();
	~vtkMAFLargeImageData();	

	// The extent of what is currently in the structured grid.
	// Dimensions is just an array to return a value.
	// Its contents are out of data until GetDimensions is called.
	int Dimensions[3];
	int DataDescription;
	vtkIdType64 Increments[3];

	double Origin[3];
	double Spacing[3];
	int ScalarType;
	int NumberOfScalarComponents;

	// Mask each pixel with this DataMask
	unsigned short DataMask;
	
	//true, if the data in the underlaying provider 
	//starts in the lower left corner
	bool DataLowerLeft;

	//time of snapshot
	vtkTimeStamp SnapshotTime;

private:
	void InternalImageDataCopy(vtkMAFLargeImageData *src);
private:
	vtkMAFLargeImageData(const vtkMAFLargeImageData&);  // Not implemented.
	void operator = (const vtkMAFLargeImageData&);  // Not implemented.	
};

#endif



