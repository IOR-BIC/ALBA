/**=======================================================================
  
  File:    	 mafVolumeLargeWriter.h
  Language:  C++
  Date:      8:2:2008   11:26
  Version:   $Revision: 1.1.2.3 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================

=========================================================================*/
#ifndef __mafVolumeLargeWriter__
#define __mafVolumeLargeWriter__

#include "medVMEDefines.h"
#include "mafObject.h"
#include "mafString.h"
#include "mafObserver.h"
#include "../vtkMAF/vtkMAFLargeImageData.h"
#include "vtkDoubleArray.h"

class MED_VME_EXPORT mafVolumeLargeWriter : public mafObject
{
public:
	mafTypeMacro(mafVolumeLargeWriter, mafObject);

protected:
	//file name of the brick file
	mafString m_BrickFileName;

	//input data set that should be bricked
	vtkMAFLargeImageData* m_InputDataSet;
  vtkDoubleArray* m_PInputXYZCoords[3];   //<X,Y,Z-coordinates for rectilinear grids

	//listener that should receive events
	mafObserver* m_Listener;

	//the maximal allowed ratio of the output and input data
	double m_DblLimitCoef;

public:
	mafVolumeLargeWriter();
	virtual ~mafVolumeLargeWriter();

public:
	//sets a listener that should receive callbacks
	inline void SetListener(mafObserver* listener) {		
		m_Listener = listener;
	}

	//Gets the associated brick file
	inline mafString GetOutputFileName() {
		return m_BrickFileName;
	}

	//Sets a new  associated brick file
	inline void SetOutputFileName(const char* filename) {		
		m_BrickFileName = filename;		
	}

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
	
	//Gets the current size limit coefficient
	inline double GetLimitConst() {
		return m_DblLimitCoef;
	}

	//Sets the maximal allowed ratio of the output and input data,
	//i.e., it limits the number of constructed levels
	//For an instance, having a 4GB input file and this constant set to 1.2, 
	//no higher resolution level will be constructed after the total size of
	//levels constructed previously exceeds 4.8 GB
	inline void SetLimitConst(double dblLimitCoef) {
		m_DblLimitCoef = dblLimitCoef;
	}

	//returns estimated total size for the current VOI and number of levels
	//or all levels, if nLevels == 0
	vtkIdType64 GetEstimatedTotalSize(int nLevels = 0);

public:
	//performs the process, returns false if an error occurs
	virtual bool Update();

protected:
	//computes the volume size in bytes
	vtkIdType64 GetVOISizeInBytes();

	//computes the sample rate needed for the volume of nSize so it
	//can be kept in memory of nMemLim
	int ComputeSampleRate(vtkIdType64 nSize, vtkIdType64 nMemLim);

	//computes the best brick size for the given sample rate
	int ComputeBrickSize(int nSampleRate, int nMaxSampleRate);

	//creates BBF files with LOD with sample rate ranges from 1 to nMaxSampleRate
	//skipping less important levels in order to fit into nTotalMaxSize Bytes
  //returns number of constructed levels and in nTotalMaxSize their size in bytes
	int CreateLODs(int nMaxSampleRate, vtkIdType64& nTotalMaxSize) throw(...);
};

#endif //__mafBrickingProcessObject__