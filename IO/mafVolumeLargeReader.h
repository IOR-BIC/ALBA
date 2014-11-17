/**=======================================================================
  
  File:    	 mafVolumeLargeReader.h
  Language:  C++
  Date:      20:2:2008   14:04
  Version:   $Revision: 1.1.2.4 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================

=========================================================================*/
#ifndef __mafVolumeLargeReader__
#define __mafVolumeLargeReader__

#include "mafobject.h"
#include "medVMEDefines.h"
#include "mafString.h"
#include "mafObserver.h"
#include "mafMTime.h"
#include "vtkImageData.h"
#include "mafBrickedFileReader.h"

//this is similar to vtkImageMultipleInputOutputFilter 
class MED_VME_EXPORT mafVolumeLargeReader : public mafObject, mafTimeStamped
{
public:
	mafTypeMacro(mafVolumeLargeReader, mafObject);

protected:
	//file name of the brick file (mask)
	mafString m_BrickFileName;

	//requested VOI (in the highest resolution units)
	int m_VOI[6];
	bool m_VOI_Initialized;

	//the memory limit in kilobytes for the snapshot of data
	unsigned long m_MemoryLimit;

	//output data set
	vtkImageData* m_DataSet;
  vtkRectilinearGrid* m_DataSetRLG;   //rectilinear data set

	//listener that should receive events
	mafObserver* m_Listener;

	//list of bricked readers (levels)
	mafBrickedFileReader** m_PLevels;
	int m_NLevels;	

	//last update stamp
	mafMTime m_LastUpdateTime;	

	int m_NCurrentLevel;	//level currently used to process
public:	
	mafVolumeLargeReader();
	virtual ~mafVolumeLargeReader();

public:
	//returns number of levels
	inline int GetNumberOfLevels() {
		return m_NLevels;
	}

	//returns the level file (corresponds to sample rate)
	inline mafBrickedFileReader* GetLevelFile(int nLevel) {
		return m_PLevels != NULL ? m_PLevels[nLevel] : NULL;
	}

	//Gets the current sample rate
	inline int GetSampleRate() {
		return m_NCurrentLevel;
	}  

	//Gets the associated brick file
	inline mafString GetFileName() {
		return m_BrickFileName;
	}

	//Sets a new  associated brick file
	inline void SetFileName(const char* filename) {
		if (!m_BrickFileName.Equals(filename)) {
			m_BrickFileName = filename;
			DestroyLODs();		//we will have to recreate it
			this->Modified();
		}
	}

  //Returns true, if the underlaying volume data set is a rectilinear grid
  //The caller should use GetOutputRLGDataSet
  inline bool IsRectilinearGrid() 
  {
    mafBrickedFileReader* pFile = GetLevelFile(1);
    if (pFile == NULL)
      return false;
    else
      return pFile->IsRectilinearGrid();
  }

	//Gets the associated output data set
	//If there is no output data set, it is created
	inline vtkImageData* GetOutputDataSet() 
	{
		if (m_DataSet == NULL)
			m_DataSet = vtkImageData::New();

		return m_DataSet;
	}

  //Gets the associated rectilinear output data set
  //If there is no output data set, it is created
  inline vtkRectilinearGrid* GetOutputRLGDataSet()
  {
    if (m_DataSetRLG == NULL)
      m_DataSetRLG = vtkRectilinearGrid::New();

    return m_DataSetRLG;
  }

	//Sets a new associated output data set
	//NB: the reference count of the specified output data set is increased
	void SetOutputDataSet(vtkImageData* ds);

  //Sets a new associated output data set
  //NB: the reference count of the specified output data set is increased
  //This forces the Execute to produce vtkRectilinearGrid object even, if
  //the underlaying grid is regular one (use IsRectilinearGrid to check it)
  void SetOutputRLGDataSet(vtkRectilinearGrid* ds);

	//Gets the currently requested VOI
	inline int* GetVOI() {
		return m_VOI;
	}

	//Gets the currently requested VOI
	inline void GetVOI(int VOI[6]) {
		memcpy(&VOI[0], &m_VOI[0], sizeof(int)*6);
	}

	//Sets the currently requested VOI
	//NB: it is specified in highest resolution units
	//i.e., GetDataDimensions()*GetSampleRate()
	inline void SetVOI(int VOI[6]) 
	{
		if (memcmp(&VOI[0], &m_VOI[0], sizeof(int)*6) != 0) {
			memcpy(&m_VOI[0], &VOI[0], sizeof(int)*6);
			m_VOI_Initialized = true;
			this->Modified();
		}
	}

	//Gets the current memory limit (in KB)
	inline unsigned long GetMemoryLimit() {
		return m_MemoryLimit;
	}

	//Sets a new memory limit for output
	inline void SetMemoryLimit(unsigned long memlimit) 
	{
		if (m_MemoryLimit != memlimit) {
			m_MemoryLimit = memlimit;
			this->Modified();
		}
	}

	//gets the current listener that receives callbacks
	inline mafObserver* GetListener() {
		return m_Listener;
	}

	//sets a listener that should receive callbacks
	inline void SetListener(mafObserver* listener) {		
		m_Listener = listener;
	}

	//gets the total number of bytes in level files
	//NB: this routine must be called after Update
	vtkIdType64 GetLevelFilesSize();

public:
	//This method updates the output (i.e., it performs the retrieving of data)	
	virtual bool Update();

  /** returns the best sample rate required for the given VOI and memory limit */
  virtual int ComputeSampleRate(int VOI[6], unsigned int nMemoryLimit);

protected:
	//processes data
	virtual void ExecuteData() throw(...);

	//Called by Update to fill some internal structures
	/*virtual*/ void ExecuteInformation() throw(...);


	//Creates all levels
	virtual void CreateLODs() throw(...);

	//Releases every level
	virtual void DestroyLODs();

	
	
};

#endif	//__mafVolumeLargeReader__