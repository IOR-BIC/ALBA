/**=======================================================================
  
  File:    	 mafBrickedFileReader.h
  Language:  C++
  Date:      13:2:2008   9:11
  Version:   $Revision: 1.1.2.3 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
Reads the BBF file into vtkImageDataSet
=========================================================================*/

#ifndef __mafBrickedFileReader__
#define __mafBrickedFileReader__

#include "mafBrickedFile.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

class mafProgressBarHelper;

class mafBrickedFileReader : public mafBrickedFile
{
public:
	mafTypeMacro(mafBrickedFileReader, mafBrickedFile);	

	enum RESAMPLE_MODES
	{
		NO_RESAMPLE,	//do not use resampling (default)
		L0_RESAMPLE,	//resample using nearest neighbor interpolation
		L1_RESAMPLE,	//resample using linear interpolation
	};

protected:
	typedef struct BBF_DATACACHE_ITEM
	{
		int nBrickIndex;		//index
		char* pBrickData;		//data	

		int nTimeStamp;
	} BBF_DATACACHE_ITEM;

protected:
	//output data set
	vtkImageData* m_DataSet;
  vtkRectilinearGrid* m_DataSetRLG;   //rectilinear data set

	mafProgressBarHelper *m_ProgressHelper;

	//requested VOI (in the highest resolution units)
	int m_VOI[6];		

	//false denotes that information in m_ValidROI is obsolete 
	bool m_BROIValid;
	
	//region that is currently in output data set and is valid,
	//i.e., the data from this ROI can be reused (no need to load them)
	int m_ValidROI[6];

	//last file used to open
	mafString m_LUBrickFileName;

	//small cache for one brick
	char* m_PBrickDataCache;

public:
	mafBrickedFileReader();
	virtual ~mafBrickedFileReader();

public:
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
			this->Modified();
		}
	}

  /** Sets VOI to be empty, i.e., this will actually release the memory */
  inline void SetEmptyVOI()
  {
    const static int VOI[6] = {0, 0, 0, 0, 0, 0};
    SetVOI(const_cast<int*>(&VOI[0]));
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

  /** processes data by converting m_DataSet into m_DataSetRLG */
  virtual void ExecuteRLGData();

	//opens the brick file, loading index table, etc. 
	virtual void OpenBrickFile() throw(...);

	//closes the currently opened brick file (if there is any)
	//releasing memory allocated for index table, etc.
	virtual void CloseBrickFile();

protected:
	//returns true, if the brick at position xb relative to the
	//beginning of the brick line described by pIdxMain and the 
	//current pIdxEx is uniform, false otherwise. nRemPos denotes
	//number of pIdxEx that have not processed yet and that may follow, 
	//therefore, after the pIdxMain. pIdxEx and nRemPos are updated
	//as the method advances
	inline bool IsBrickUniform(int xb, BBF_IDX_MAINITEM* pIdxMain, 
		BBF_IDX_EXITEM*& pIdxEx, int& nRemPos);

	//fills the entire brick specified by pOutPtr with the value pointed by pConstVal	
	inline void FillBrick(char* pOutPtr, const char* pConstVal);

	//loads the brick with the given index from file 
	//into the memory denoted by pOutPtr
	inline void LoadBrick(int nBrickIndex, char* pOutPtr) throw(...);
	
	//computes extents in inner bricks and boundary bricks	
	//for the given extent VOI that is specified in voxels (of highest resolution level)
	void GetBricksExtent(int VOI[6], int inBExt[6], int bndBExt[6]);	

	//copies the part of brick data that is within brckExt into pOutPtr 
	//in an unbricked form, the brick is denoted by pBrickData 	
	inline void CopyBrickData(const char* pBrickData, int brckExt[6],
		char* pOutPtr, int outIncrInB[3]);
};

//returns true, if the brick at position xb relative to the
//beginning of the brick line described by pIdxMain and the 
//current pIdxEx is uniform, false otherwise. nRemPos denotes
//number of pIdxEx that have not processed yet and that may follow, 
//therefore, after the pIdxMain. pIdxEx and nRemPos are updated
//as the method advances
inline bool mafBrickedFileReader::IsBrickUniform(int xb, BBF_IDX_MAINITEM* pIdxMain, 
						   BBF_IDX_EXITEM*& pIdxEx, int& nRemPos)
{
	bool bUniform = xb < pIdxMain->nFromBrIndex;
	if (!bUniform && xb > pIdxMain->nToBrIndex)
	{
		//we are out of the main nonuniform block
		if (nRemPos == 0)		//if there is no other block, we are uniform
			bUniform = true;
		else if (xb < pIdxEx->nFromBrIndex)
			bUniform = true;											
		else if (xb > pIdxEx->nToBrIndex)
		{
			//move to next extra item
			//NOTE: as there is at least one uniform block
			//between two nonuniform, xb must be uniform
			pIdxEx++;
			nRemPos--;
			bUniform = true;
		}
	} //endif

	return bUniform;
}

//fills the entire brick specified by pOutPtr with the value pointed by pConstVal	
inline void mafBrickedFileReader::FillBrick(char* pOutPtr, const char* pConstVal)
{
	for (int i = 0; i < m_NBrickSize[2]; i++)
	{
		const char* pCurConst = pConstVal;
		for (int j = 0; j < m_NVoxelSizeInB; j++) {
			*pOutPtr = *pCurConst;
			pOutPtr++; pCurConst++;
		}
	}
}

//loads the brick with the given index from file 
//into the memory denoted by pOutPtr
inline void mafBrickedFileReader::LoadBrick(int nBrickIndex, char* pOutPtr) throw(...)
{
	long long offset = ((long long)nBrickIndex)*m_NBrickSizeInB[2] + sizeof(BBF_HEADER);

  m_BrickFile->Seek(offset);
	m_BrickFile->Read(pOutPtr, m_NBrickSizeInB[2]);
}

//copies the part of brick data that is within brckExt into pOutPtr 
//in an unbricked form, the brick is denoted by pBrickData 	
inline void mafBrickedFileReader::CopyBrickData(const char* pBrickData, int brckExt[6],
												char* pOutPtr, int outIncrInB[3])
{
	pBrickData += brckExt[4]*m_NBrickSizeInB[1] + 	//move to the first item
				brckExt[2]*m_NBrickSizeInB[0] +	
				brckExt[0]*m_NVoxelSizeInB;

	int nLineSize = (brckExt[1] - brckExt[0] + 1)*m_NVoxelSizeInB;
	for (int z = brckExt[4]; z <= brckExt[5]; z++)
	{
		char* pOutPtrY = pOutPtr;
		const char* pBrickDataY = pBrickData;
		for (int y = brckExt[2]; y <= brckExt[3]; y++)
		{
			memcpy(pOutPtrY, pBrickDataY, nLineSize);
			pOutPtrY += outIncrInB[1];
			pBrickDataY += m_NBrickSizeInB[0];
		}

		pOutPtr += outIncrInB[2];
		pBrickData += m_NBrickSizeInB[1];
	}
}
#endif //__mafBrickedFileReader__