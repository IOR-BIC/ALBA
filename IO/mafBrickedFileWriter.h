/**=======================================================================
  
  File:    	 mafBrickedFileWriter.h
  Language:  C++
  Date:      11:2:2008   12:40
  Version:   $Revision: 1.1.2.2 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
Writes the BBF file, using data from vtkMAFLargeImageDataSet
=========================================================================*/

#ifndef __mafBrickedFileWriter__
#define __mafBrickedFileWriter__

#include "mafBrickedFile.h"
#include "vtkMAFLargeImageData.h"

class mafProgressBarHelper;

class mafBrickedFileWriter : public mafBrickedFile
{
public:
	mafTypeMacro(mafBrickedFileWriter, mafBrickedFile);	

protected:
	//input data set that should be bricked
	vtkMAFLargeImageData* m_InputDataSet;
  vtkDoubleArray* m_PInputXYZCoords[3];   //<X,Y,Z-coordinates for rectilinear grids
  
	//buffer for bricks data
	char* m_PBricksBuffer;
	bool* m_PBricksValidity;	//false mean that the brick is uniform

	//buffer for input data set	
	char* m_PDataBuffer;	//sampled data

	//a small buffers capable to hold 16 tuples (or their sums)
	char* m_PTuplesBuffer;
	double* m_PSumTuplesBuffer;

	std::vector< BBF_IDX_EXITEM > m_ExtraBrckMAP;

	mafProgressBarHelper *m_ProgressHelper;
	
public:
	mafBrickedFileWriter();
	virtual ~mafBrickedFileWriter();

public:
	//Gets the associated input data set
	inline vtkMAFLargeImageData* GetInputDataSet() {
		return m_InputDataSet;
	}

	//Sets a new associated input data set
	//NB: the reference count of the specified input data set is increased
	void SetInputDataSet(vtkMAFLargeImageData* ds);
		
  /** Specifies the grid coordinates in x-direction */
  void SetInputXCoordinates(vtkDoubleArray* pCoords);

  /** Specifies the grid coordinates in y-direction */
  void SetInputYCoordinates(vtkDoubleArray* pCoords);

  /** Specifies the grid coordinates in z-direction */
  void SetInputZCoordinates(vtkDoubleArray* pCoords);

	//Sets the size of brick
	//NB: must be equal to 2^k, where k is an integer > 0
	inline void SetBrickSize(int nSize) 
	{
		if (m_FileHeader.bricksize != nSize) {
			m_FileHeader.bricksize = (unsigned short)nSize;
			this->Modified();
		}
	}

	//Sets the sample rate to be used to produce the data from the original volume
	inline void SetSampleRate(int nSampleRate) 
	{
		if (m_FileHeader.sample_rate != nSampleRate) {
			m_FileHeader.sample_rate = (unsigned short)nSampleRate;
			this->Modified();
		}		
	}  

public:
	//This method updates the output (i.e., it performs the bricking)	
	//returns false if an error occurs
	/*virtual*/ bool Update();

protected:
	//Called by Update to fill some internal structures
	/*virtual*/ void ExecuteInformation() throw(...);

	//allocates the required buffers
	/*virtual*/ void AllocateBuffers() throw(...);

	//deallocates the buffers created in AllocateBuffers
	/*virtual*/ void DeallocateBuffers() throw(...);

	//processes data
	virtual void ExecuteData() throw(...);

	//processes the currently loaded block of data
	//creating its bricked version
	virtual void ConstructBricks() throw(...);

	//Process the currently loaded bricks, computing average values
	//for every brick and determining which bricks are uniform
	virtual void ProcessBricks(int nCurBrickPlane) throw(...);

  /** Process the grid coordinates for rectilinear grid
  If the input data is regular, this method does nothing */
  virtual void ProcessCoordinates() throw(...);

protected:
	//computes an average value for every brick in the current
	//brick buffer, as it computes avg and min, native type must be specified
	//T_VAL is the data type of values stored in bricks, 
	//T_SUM is the data type to be used for summing
	//the computed values are stored as the lowest resolution snapshot
	//and uniform bricks are also denoted during the process
	template< typename T_VAL, typename T_SUM >
		void CreateBricksLowResolution(int nCurBrickPlane);

	//updates brick map, i.e., data stucture storing the information which bricks are uniform
	//NB: nPrevSkipped member has a temporary value 
	void CreateBricksIndexTable(int nCurBrickPlane);
};

#endif //__mafBrickedFileWriter__