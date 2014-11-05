/**=======================================================================
  
  File:    	 BrickedFile.h
  Language:  C++
  Date:      11:2:2008   11:49
  Version:   $Revision: 1.1.2.3 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
This abstract class contains the stuff common to both, the reader and writer
=========================================================================*/
#ifndef __mafBrickedFile__
#define __mafBrickedFile__

#pragma warning (disable: 4068) //unknown pragma

#include "mafObject.h"
#include "mafString.h"
#include "mafObserver.h"
#include "mafTimeStamped.h"

#include "vtkDoubleArray.h"
#include "../vtkMAF/vtkMAFFile.h"
#include "../vtkMAF/vtkMAFDataArrayDescriptor.h"

class mafBrickedFile : public mafObject, public mafTimeStamped
{
public:
	mafAbstractTypeMacro(mafBrickedFile, mafObject);

	const static unsigned long m_Signature = 0xCA464242;	//'BBF' + CRC of 'BBF'		
  const static unsigned short m_CurrentVersion = 2;

public:

	//header structure
	typedef struct BBF_HEADER 
	{
		unsigned long signature;	//signature
		unsigned short version;		//version of this file				
		unsigned short dims[3];		//sample data dimension (in voxels)
		unsigned short bricksize;	//size of one brick => number of brick can
									            //be calculated from aligned(dim[i] / bricksize)		
		unsigned short sample_rate;	//sample rate that was used			
		double origin[3];			  //origin
		double spacing[3];			//spacing (sampled one)

		unsigned char datatype;		//VTK data type
		unsigned char numcomps;		//number of components
		unsigned char endian;		  //0 - little-endian (Intel), 1 - big-endian (Motorola)
		unsigned char rlgrid;     //0 - regular grid, 1 - rectilinear grid => coordinates stored in the file

		unsigned long long idxtblofs;	//offset to index table (in file), map is always before
		unsigned long extra_idx_items;	//number of extra index items
	} BBF_HEADER;

	typedef struct BBF_IDX_MAINITEM
	{
		unsigned long nPrevSkipped;			//total number of bricks skipped in previous lines
		unsigned short nFromBrIndex;		//first non uniform brick index (relative to the current line)
		unsigned short nToBrIndex;			//last non uniform brick index (relative to the current line)
		struct 
		{
			unsigned long nListLength	: 8;	//total length of list
			unsigned long nNextItemIndex : 24;	//index to continuous area
		};
	} BBF_IDX_MAINITEM;

	typedef struct BBF_IDX_EXITEM 
	{
		unsigned short nFromBrIndex;		//first non uniform brick index (relative to the current line)
		unsigned short nToBrIndex;			//last non uniform brick index (relative to the current line)
	} BBF_IDX_EXITEM;

protected:
	//file name of the brick file
	mafString m_BrickFileName;
	
	//File handle	
	vtkMAFFile2* m_BrickFile;

	//File header
	BBF_HEADER m_FileHeader;

	//brick maps
	BBF_IDX_MAINITEM* m_PMainIdxTable;
	BBF_IDX_EXITEM* m_PExIdxTable;

	//low resolution level (brick map)
	char* m_PLowResLevel;

  vtkDoubleArray* m_PXYZCoords[3];   //<X,Y,Z-coordinates for rectilinear grids  

#pragma region Values Precomputed to Speed Up Operations
	//number of bytes per one voxel = element size*number of components
	int m_NVoxelSizeInB;

	//number of voxels in one line, plane, and in the entire brick
	//and the appropriate number of bytes it occupies
	int m_NBrickSize[3];
	int m_NBrickSizeInB[3];

	//number of bricks in every dimension
	int m_NBricksDim[3];

	//number of bricks in one line, plane, and number of all bricks
	//and the appropriate number of bytes it occupies
	//NB: number of bytes for all bricks is not valid
	//for volumes >= 2GB (or a bit smaller)
	int m_NBricksDimSize[3];	
	int m_NBricksDimSizeInB[3];

	//time, when it was update last
	mafMTime m_LastUpdateTime;
#pragma endregion 

	//listener that should receive events
	mafObserver* m_Listener;

protected:
	mafBrickedFile();
	virtual ~mafBrickedFile();

public:	
	//gets the current listener that receives callbacks
	inline mafObserver* GetListener() {
		return m_Listener;
	}

	//sets a listener that should receive callbacks
	inline void SetListener(mafObserver* listener) {		
		m_Listener = listener;
	}

	//Gets the associated brick file
	inline mafString GetFileName() {
		return m_BrickFileName;
	}

	//Sets a new  associated brick file
	inline void SetFileName(const char* filename) {
		if (!m_BrickFileName.Equals(filename)) {
			m_BrickFileName = filename;		
			this->Modified();
		}
	}

	//Gets the dimension of data (in voxels, not in bricks)
	inline unsigned short* GetDataDimensions() {
		return m_FileHeader.dims;
	}

	//Gets the dimension of data (in voxels, not in bricks)
	inline void GetDataDimensions(int dims[3]) {
		GetDataDimensions(dims[0], dims[1], dims[2]);		
	}

	//Gets the dimension of data (in voxels, not in bricks)
	inline void GetDataDimensions(int& dim_x, int& dim_y, int& dim_z) {
		dim_x = m_FileHeader.dims[0]; dim_y = m_FileHeader.dims[1]; dim_z = m_FileHeader.dims[2];
	}

	//Gets the size of brick
	inline int GetBrickSize() {
		return (int)m_FileHeader.bricksize;
	}

	//Gets the dimension of bricks
	inline int* GetBricksDimensions() {		
		return m_NBricksDim;
	}

	//Gets the dimension of bricks
	inline void GetBricksDimensions(int dims[3]) {		
		GetBricksDimensions(dims[0], dims[1], dims[2]);
	}

	//Gets the dimension of bricks
	inline void GetBricksDimensions(int& dim_x, int& dim_y, int& dim_z) {		
		dim_x = m_NBricksDim[0]; dim_y = m_NBricksDim[1]; dim_z = m_NBricksDim[2];		
	}

	//Gets the sample rate used to produce the data from the original volume
	inline int GetSampleRate() {
		return m_FileHeader.sample_rate;
	}

	//Gets the origin	
	inline double* GetDataOrigin() {
		return m_FileHeader.origin;
	}

	//Gets the origin
	inline void GetDataOrigin(double pos[3]) {		
		GetDataOrigin(pos[0], pos[1], pos[2]);
	}

	//Gets the origin
	inline void GetDataOrigin(double& pos_x, double& pos_y, double& pos_z) {		
		pos_x = m_FileHeader.origin[0]; pos_y = m_FileHeader.origin[1]; 
		pos_z = m_FileHeader.origin[2];		
	}

	//Gets the spacing (returns 0,0,0 for rectilinear grid)	
	inline double* GetDataSpacing() {
		return m_FileHeader.spacing;
	}

	//Gets the spacing
	inline void GetDataSpacing(double sp[3]) {		
		GetDataSpacing(sp[0], sp[1], sp[2]);
	}

	//Gets the spacing
	inline void GetDataSpacing(double& sp_x, double& sp_y, double& sp_z) {		
		sp_x = m_FileHeader.spacing[0]; sp_y = m_FileHeader.spacing[1]; 
		sp_z = m_FileHeader.spacing[2];		
	}

  //Returns true, if the data is rectilinear grid
  inline bool IsRectilinearGrid() {
    return m_FileHeader.rlgrid != 0;
  }

  //Returns the grid coordinates in the x-direction
  inline vtkDoubleArray* GetXCoordinates() {
    return m_PXYZCoords[0];
  }

  //Returns the grid coordinates in the y-direction
  inline vtkDoubleArray* GetYCoordinates() {
    return m_PXYZCoords[1];
  }

  //Returns the grid coordinates in the z-direction
  inline vtkDoubleArray* GetZCoordinates() {
    return m_PXYZCoords[2];
  }

	//Gets the VTK data type
	inline int GetDataType() {
		return m_FileHeader.datatype;
	}

	//Gets the size of VTK data type
	inline int GetDataTypeSize() {
		return vtkMAFDataArrayDescriptor::GetDataTypeSize(m_FileHeader.datatype);		
	}

	//Gets number of components
	inline int GetNumberOfComponents() {
		return m_FileHeader.numcomps;
	}

	//Gets the size of one voxel in bytes
	inline int GetVoxelSize() {
		return m_NVoxelSizeInB;
	}

public:
	//This method updates the output (i.e., it performs writing / loading bricking)	
	//returns false if an error occurs
	virtual bool Update() = 0;

protected:
	//Called by Update to fill some internal structures
	virtual void ExecuteInformation() throw(...) = 0;

	//allocates the required buffers
	virtual void AllocateBuffers()  throw(...) {};

	//deallocates the buffers created in AllocateBuffers
	virtual void DeallocateBuffers()  throw(...) {};
};

#endif //__mafBrickedFile__