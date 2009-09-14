/**=======================================================================
  
  File:    	 mafBrickedFileWriter.cpp
  Language:  C++
  Date:      11:2:2008   12:42
  Version:   $Revision: 1.1.2.2 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
Writes the BBF file, using data from vtkMAFLargeImageDataSet
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafBrickedFileWriter.h"
#include "../vtkMAF/vtkMAFFileDataProvider.h"

mafCxxTypeMacro(mafBrickedFileWriter);
#include "mafMemDbg.h"


mafBrickedFileWriter::mafBrickedFileWriter()
{
	memset(&m_FileHeader, 0, sizeof(m_FileHeader));
	m_FileHeader.signature = Signature;
	m_FileHeader.version = CurrentVersion;		//version 1
	m_FileHeader.bricksize = 16;	//16x16x16 by the default
	m_FileHeader.sample_rate = 1;	//no subsampling by the default
	
	m_InputDataSet = NULL;
  m_PInputXYZCoords[0] = NULL;
  m_PInputXYZCoords[1] = NULL;
  m_PInputXYZCoords[2] = NULL;

	m_PBricksBuffer = NULL;
	m_PBricksValidity = NULL;	

	m_PTuplesBuffer = NULL;
	m_PSumTuplesBuffer = NULL;		
}

mafBrickedFileWriter::~mafBrickedFileWriter()
{	
	//all should be removed in Update
	vtkDEL(m_InputDataSet);
  vtkDEL(m_PInputXYZCoords[0]);
  vtkDEL(m_PInputXYZCoords[1]);
  vtkDEL(m_PInputXYZCoords[2]);
}


//Sets a new associated input data set
//NB: the reference count of the specified input data set is increased
void mafBrickedFileWriter::SetInputDataSet(vtkMAFLargeImageData* ds)
{
	if (ds != m_InputDataSet)
	{
		vtkDEL(m_InputDataSet);

		if ((m_InputDataSet = ds) != NULL)
			m_InputDataSet->Register(NULL);

		this->Modified();
	}
}

//------------------------------------------------------------------------
//Specifies the grid coordinates in x-direction
void mafBrickedFileWriter::SetInputXCoordinates(vtkDoubleArray* pCoords)
//------------------------------------------------------------------------
{
  if (pCoords != m_PInputXYZCoords[0])
  {
    vtkDEL(m_PInputXYZCoords[0]);  
    if (NULL != (m_PInputXYZCoords[0] = pCoords))
    {
      m_PInputXYZCoords[0]->Register(NULL);
      m_FileHeader.rlgrid = 1;
    }
    else
    {
      if (m_PInputXYZCoords[1] == NULL && m_PInputXYZCoords[2] == NULL)
        m_FileHeader.rlgrid = 0;
    }

    this->Modified();
  }
}

//------------------------------------------------------------------------
//Specifies the grid coordinates in y-direction
void mafBrickedFileWriter::SetInputYCoordinates(vtkDoubleArray* pCoords)
//------------------------------------------------------------------------
{
  if (pCoords != m_PInputXYZCoords[1])
  {
    vtkDEL(m_PInputXYZCoords[1]);  
    if (NULL != (m_PInputXYZCoords[1] = pCoords))
    {
      m_PInputXYZCoords[1]->Register(NULL);
      m_FileHeader.rlgrid = 1;
    }
    else
    {
      if (m_PInputXYZCoords[0] == NULL && m_PInputXYZCoords[2] == NULL)
        m_FileHeader.rlgrid = 0;
    }

    this->Modified();
  }
}

//------------------------------------------------------------------------
//Specifies the grid coordinates in z-direction
void mafBrickedFileWriter::SetInputZCoordinates(vtkDoubleArray* pCoords)
//------------------------------------------------------------------------
{
  if (pCoords != m_PInputXYZCoords[2])
  {
    vtkDEL(m_PInputXYZCoords[2]);  
    if (NULL != (m_PInputXYZCoords[2] = pCoords))
    {
      m_PInputXYZCoords[2]->Register(NULL);
      m_FileHeader.rlgrid = 1;
    }
    else
    {
      if (m_PInputXYZCoords[0] == NULL && m_PInputXYZCoords[1] == NULL)
        m_FileHeader.rlgrid = 0;
    }

    this->Modified();
  }
}

//Called by Update to fill some internal structures
/*virtual*/ void mafBrickedFileWriter::ExecuteInformation() throw(...)
{
	if (m_InputDataSet == NULL)	//error
		throw std::invalid_argument(_("Invalid argument. Input Data Set cannot be NULL"));

	int VOI[6],wext[6];	
	m_InputDataSet->GetVOI(VOI);		//find out the volume to be processed	
	m_InputDataSet->GetExtent(wext);
	m_InputDataSet->GetOrigin(m_FileHeader.origin);
	m_InputDataSet->GetSpacing(m_FileHeader.spacing);

	for (int i = 0; i < 3; i++) 
	{
		m_FileHeader.origin[i] += (VOI[2*i] - wext[2*i])*m_FileHeader.spacing[i];
		m_FileHeader.dims[i] = (VOI[2*i + 1] - VOI[2*i] + 1) / m_FileHeader.sample_rate +
      ((VOI[2*i + 1] - VOI[2*i] + 1) % m_FileHeader.sample_rate != 0);
		m_FileHeader.spacing[i] *= m_FileHeader.sample_rate;

		m_NBricksDim[i] = (m_FileHeader.dims[i] + m_FileHeader.bricksize - 1) / 
			m_FileHeader.bricksize;
	}
	
	vtkMAFLargeDataProvider* dp = m_InputDataSet->GetPointDataProvider();
	vtkMAFDataArrayDescriptor* dsc = dp->GetScalarsDescriptor();

	m_FileHeader.numcomps = dsc->GetNumberOfComponents();
	m_FileHeader.datatype = dsc->GetDataType();

	m_NVoxelSizeInB = m_FileHeader.numcomps*dsc->GetDataTypeSize();

	unsigned short end_test = 0xFFFE;
	m_FileHeader.endian = (*((unsigned char*)&end_test) == 0xFF);

	m_NBrickSize[0] = m_FileHeader.bricksize;		 
	m_NBricksDimSize[0] = m_NBricksDim[0]; 
	for (int i = 1; i < 3; i++)
	{	
		m_NBrickSize[i] = m_NBrickSize[i - 1]*m_NBrickSize[0];		
		m_NBricksDimSize[i] = m_NBricksDim[i]*m_NBricksDimSize[i - 1];
	}
	
	for (int i = 2; i >= 0; i--)
	{
		m_NBrickSizeInB[i] = m_NBrickSize[i]*m_NVoxelSizeInB;	
		m_NBricksDimSizeInB[i] = m_NBricksDimSize[i]*m_NBrickSizeInB[2];
	}
}

//allocates the required buffers
/*virtual*/ void mafBrickedFileWriter::AllocateBuffers() throw(...)
{	
	m_PDataBuffer = new char[m_NBricksDimSizeInB[1]];
	m_PBricksBuffer = new char[m_NBricksDimSizeInB[1]];	
	m_PBricksValidity = new bool[m_NBricksDimSize[1]];
	m_PLowResLevel = new char[m_NBricksDimSize[2]*m_NVoxelSizeInB];

	memset(m_PDataBuffer, 0, m_NBricksDimSizeInB[1]);	//to ensure we have zeros

	//buffers for tuples
	m_PTuplesBuffer = new char[16*m_NVoxelSizeInB];
	m_PSumTuplesBuffer = new double[16*this->GetNumberOfComponents()];

	m_PMainIdxTable = new BBF_IDX_MAINITEM[m_NBricksDim[1]*m_NBricksDim[2]];
	memset(m_PMainIdxTable, 0, m_NBricksDim[1]*m_NBricksDim[2]*sizeof(BBF_IDX_MAINITEM));
	
	m_ExtraBrckMAP.clear();	

  if (this->IsRectilinearGrid())
  {
    for (int i = 0; i < 3; i++)
    {
      m_PXYZCoords[i] = vtkDoubleArray::New();
      m_PXYZCoords[i]->Allocate(m_FileHeader.dims[i]);  
      m_PXYZCoords[i]->SetNumberOfTuples(m_FileHeader.dims[i]);
    }
  }
}

//allocates the required buffers
/*virtual*/ void mafBrickedFileWriter::DeallocateBuffers() throw(...)
{
	cppDEL(m_PLowResLevel);
	cppDEL(m_PMainIdxTable);	
	cppDEL(m_PDataBuffer);
	cppDEL(m_PBricksBuffer);
	cppDEL(m_PBricksValidity);
	cppDEL(m_PTuplesBuffer);
	cppDEL(m_PSumTuplesBuffer);

  for (int i = 0; i < 3; i++) {    
    vtkDEL(m_PXYZCoords[i]);
  }
}

//processes data
/*virtual*/ void mafBrickedFileWriter::ExecuteData()  throw(...)
{
	int nSampleRate = this->GetSampleRate();

	mafString szMsg = wxString::Format(_("Sampling and bricking data (sr: %d, bs: %d) ..."),
		nSampleRate, m_NBrickSize[0]);

	mafEventMacro(mafEvent(this, PROGRESSBAR_SET_TEXT, &szMsg));
	mafEventMacro(mafEvent(this, PROGRESSBAR_SET_VALUE, (long)0));

	vtkMAFLargeDataProvider* dp = m_InputDataSet->GetPointDataProvider();	
	int nScalarsDscIndex = dp->GetIndexOfScalarsDescriptor();
	vtkMAFDataArrayDescriptor* dsc = dp->GetDescriptor(nScalarsDscIndex);

	//get VOI
	int VOI[6];
	m_InputDataSet->GetVOI(VOI);

	//compute output dimensions
	int nDims[3];
	this->GetDataDimensions(nDims);

	//compute how many elements will be read in one step
	int nElemsPerLine = (VOI[1] - VOI[0] + 1) * dsc->GetNumberOfComponents();
	int nBytesPerLine = nElemsPerLine * dsc->GetDataTypeSize();
  char* pLineBuffer = new char[nBytesPerLine];
	
	//number of sampled bytes
	int nSkipBytesX = (m_NBricksDim[0]*m_NBrickSize[0] - nDims[0])*m_NVoxelSizeInB;
	int nSkipBytesY = m_NBricksDim[0]*m_NBrickSize[0]*
		(m_NBricksDim[1]*m_NBrickSize[0] - nDims[1])*m_NVoxelSizeInB;

	//get increments
	vtkIdType64 dataIncr[3];
	m_InputDataSet->GetIncrements(dataIncr);	//get increments (in elements)	

	vtkIdType64 dataIncrSkip[3];
	dataIncrSkip[0] = dataIncr[0]*nSampleRate;
	dataIncrSkip[1] = dataIncr[1]*nSampleRate;
	dataIncrSkip[2] = dataIncr[2]*nSampleRate;
  int nLineBufSkip = 
  //BES: 11.7.2008 - GetIncrements takes number of components into account
    ((int)dataIncrSkip[0] - dsc->GetNumberOfComponents())*
      (m_NVoxelSizeInB / dsc->GetNumberOfComponents());

	char* pDstBuf = m_PDataBuffer;

	//index to the first element	
	vtkIdType64 nStartIndex = VOI[0]*dataIncr[0] + VOI[2]*dataIncr[1] + VOI[4]*dataIncr[2];		
	for (int zb = 0; zb < nDims[2]; zb++, nStartIndex += dataIncrSkip[2])
	{
		mafEventMacro(mafEvent(this, PROGRESSBAR_SET_VALUE, (long)(100*zb / nDims[2])));

		//processing one plane
		//process sampled lines at this plane
		vtkIdType64 nLineIndex = nStartIndex;				
		for (int yb = 0; yb < nDims[1]; yb++, nLineIndex += dataIncrSkip[1])
		{
			//process every pixel the current line 
			dp->GetDataArray(nScalarsDscIndex, (void*)pLineBuffer, 
				(vtkIdType64)nElemsPerLine, nLineIndex);

			//now sample the read pixel line and write it
			//first voxel is always present
			char* pSrcLineBuf = pLineBuffer;
			for (int xb = 0; xb < nDims[0]; xb++)
			{
				for (int i = 0; i < m_NVoxelSizeInB; i++) {
					*pDstBuf = *pSrcLineBuf;
					pDstBuf++; pSrcLineBuf++;
				}

				pSrcLineBuf += nLineBufSkip;	//we increased it in loop
			}

			pDstBuf += nSkipBytesX;
		}

		pDstBuf += nSkipBytesY;

		bool bBrickFinished = ((zb + 1) % m_NBrickSize[0]) == 0;
		if (!bBrickFinished && (zb + 1) == nDims[2])
		{
			//reset the rest planes
			memset(pDstBuf, 0, m_NBricksDimSizeInB[1] - (pDstBuf - m_PDataBuffer));
			bBrickFinished = true;
		}

		if (bBrickFinished)
		{
			//perform bricking
			ConstructBricks();

			//process the loaded bricks
			ProcessBricks(zb / m_NBrickSize[0]);

			//data written already in ProcessBricks
			//m_BrickFile->WriteFile( m_pDataBuffer, m_nBricksDimSizeInB[1]);

			pDstBuf = m_PDataBuffer;
		}
	}	

  delete[] pLineBuffer;
}

//------------------------------------------------------------------------
//Process the grid coordinates for rectilinear grid
//If the input data is regular, this method does nothing
/*virtual*/ void mafBrickedFileWriter::ProcessCoordinates() throw(...)
//------------------------------------------------------------------------
{
  if (!this->IsRectilinearGrid())
    return; //invalid call

  int nSampleRate = this->GetSampleRate();
  for (int i = 0; i < 3; i++)
  {    
    int nDstVals = m_PXYZCoords[i]->GetNumberOfTuples();      
    double* pDstPtr = m_PXYZCoords[i]->WritePointer(0, nDstVals);

    if (m_PInputXYZCoords[i] != NULL)
    {
      int nSrcVals = m_PInputXYZCoords[i]->GetNumberOfTuples();
      double* pSrcPtr = m_PInputXYZCoords[i]->GetPointer(0);            
      
      for (int j = 0; j < nSrcVals && nDstVals > 0; j += nSampleRate)
      {
        *pDstPtr = *pSrcPtr;        
        pSrcPtr += nSampleRate;
        pDstPtr++;
        nDstVals--;
      }

      if (nDstVals > 0)
      {
        //aligning, we need to set manually the missing coordinates
        double dblStep = 0.0;
        if (nSrcVals > nSampleRate) {
          dblStep = *(pDstPtr - 1) - *(pDstPtr - 2);
        }        

        while (nDstVals > 0)
        {
          *pDstPtr = *(pDstPtr - 1) + dblStep;  //the same as previous
          pDstPtr++; nDstVals--;
        }
      }
    } //endif
    else
    {
      //the coordinates not specified
      double origin = this->GetDataOrigin()[i];
      double dblStep = this->GetDataSpacing()[i];
      if (dblStep == 0.0) //if there is no spacing, there is 0.0
        dblStep = 1.0;

      for (int j = 0; j < nDstVals; j++) {
        pDstPtr[j] = origin + j*dblStep;
      }
    }

    //now, we need to store them
    m_BrickFile->Write(m_PXYZCoords[i]->GetPointer(0), 
      m_PXYZCoords[i]->GetNumberOfTuples()*sizeof(double));
  } //end for  
}



//processes the currently loaded block of data
//creating its bricked version, computing average values
//for every brick and determining which bricks are uniform
/*virtual*/ void mafBrickedFileWriter::ConstructBricks()  throw(...)
{
	char* pCurLineBuf = m_PDataBuffer;

	int nBricksBufOfsZ = 0;
	for (int zb = 0; zb < m_NBrickSize[0]; zb++, nBricksBufOfsZ += m_NBrickSizeInB[1])
	{				
		//process every brick line at this plane
		int nBricksBufOfsY = nBricksBufOfsZ;	
		for (int yb = 0; yb < m_NBricksDim[1]; yb++, nBricksBufOfsY += m_NBricksDimSizeInB[0])
		{				
			int nBricksBufOfs = nBricksBufOfsY;	
			for (int y = 0; y < m_NBrickSize[0]; y++, nBricksBufOfs += m_NBrickSizeInB[0])
			{					
				//now copy the read pixel line into bricks					
				char* pCurBricksBuf = m_PBricksBuffer + nBricksBufOfs;					
				for (int i = 0; i < m_NBricksDim[0]; i++)
				{
					memcpy(pCurBricksBuf, pCurLineBuf, m_NBrickSizeInB[0]);
					
					pCurBricksBuf += m_NBrickSizeInB[2];
					pCurLineBuf += m_NBrickSizeInB[0];
				}
			}
		}
	}
}

//Process the currently loaded bricks, computing average values
//for every brick and determining which bricks are uniform
/*virtual*/ void mafBrickedFileWriter::ProcessBricks(int nCurBrickPlane)  throw(...)
{
	switch (m_InputDataSet->GetScalarType())
	{
		//short data type => we can use int for sums (will be faster)
	case VTK_UNSIGNED_CHAR: CreateBricksLowResolution< unsigned char, int >(nCurBrickPlane); break;
	case VTK_CHAR: CreateBricksLowResolution< char, int >(nCurBrickPlane); break;
	case VTK_UNSIGNED_SHORT: CreateBricksLowResolution< unsigned short, int >(nCurBrickPlane); break;
	case VTK_SHORT: CreateBricksLowResolution< short, int >(nCurBrickPlane); break;

		//we will have to use double for sums
	case VTK_DOUBLE: CreateBricksLowResolution< double, double >(nCurBrickPlane); break;
	case VTK_FLOAT: CreateBricksLowResolution< float, double >(nCurBrickPlane); break;
	case VTK_LONG: CreateBricksLowResolution< long, double >(nCurBrickPlane); break;
	case VTK_UNSIGNED_LONG: CreateBricksLowResolution< unsigned long, double >(nCurBrickPlane); break;
	case VTK_INT: CreateBricksLowResolution< int, double >(nCurBrickPlane); break;
	case VTK_UNSIGNED_INT: CreateBricksLowResolution< unsigned int, double >(nCurBrickPlane); break;

	default:
		throw std::invalid_argument(_("Unknown data type.\n"));	
	}	

	//store every non uniform brick
	char* pCurBrick = m_PBricksBuffer;
	for (int i = 0; i < m_NBricksDimSize[1]; i++)
	{
		if (m_PBricksValidity[i]) {			
			m_BrickFile->Write( pCurBrick, m_NBrickSizeInB[2]);
		}

		pCurBrick += m_NBrickSizeInB[2];
	}

	//this updates brick map, i.e., data stucture storing the information which bricks are uniform
	CreateBricksIndexTable(nCurBrickPlane);
}

//computes an average value for every brick in the current
//brick buffer, as it computes avg and min, native type must be specified
//T_VAL is the data type of values stored in bricks, 
//T_SUM is the data type to be used for summing
//the computed values are stored as the lowest resolution snapshot
//and uniform bricks are also denoted during the process
template< typename T_VAL, typename T_SUM >
void mafBrickedFileWriter::CreateBricksLowResolution(int nCurBrickPlane)
{
	vtkMAFLargeDataProvider* dp = m_InputDataSet->GetPointDataProvider();
	vtkMAFDataArrayDescriptor* dsc = dp->GetScalarsDescriptor();

	int comps = dsc->GetNumberOfComponents();

	//prepare buffers for min and max
	T_VAL* pMins = (T_VAL*)m_PTuplesBuffer;
	T_VAL* pMaxs = pMins + comps;
	T_SUM* pSums = (T_SUM*)m_PSumTuplesBuffer;

	//compute the current index
	int index = nCurBrickPlane*m_NBricksDimSize[1];
	T_VAL* pLevelBuf = (T_VAL*)m_PLowResLevel;
	pLevelBuf += index*comps;

	//process every brick
	char* pCurBrick = m_PBricksBuffer;
	for (int i = 0; i < m_NBricksDimSize[1]; i++, index++)
	{		
		T_VAL* pData = (T_VAL*)pCurBrick;
		pCurBrick += m_NBrickSizeInB[2];

		//initialize the first tuple
		for (int j = 0; j < comps; j++) 
		{
			pMins[j] = pMaxs[j] = *pData;
			pSums[j] = (T_SUM)*pData;
			pData++;
		}		

		//get min and max, compute sum
		while(((char*)pData) < pCurBrick)
		{
			for (int j = 0; j < comps; j++) 
			{
				T_VAL v = *pData;
				if (v < pMins[j])
					pMins[j] = v;
				else if (v > pMaxs[j])
					pMaxs[j] = v;

				pSums[j] += (T_SUM)v;
				pData++;
			}
		}

		//check, if the brick is uniform
		bool bUniform = true;
		for (int j = 0; j < comps; j++) {
			if (pMins[j] != pMaxs[j]) {
				bUniform = false; break;
			}
		}

		if (!(m_PBricksValidity[i] = !bUniform))
		{
			//brick is not uniform => compute average
			for (int j = 0; j < comps; j++) {
				pMins[j] = (T_VAL)(pSums[j] / m_NBrickSize[2]);				
			}			
		}

		//in pMins is either uniform tuple or average, store it
		for (int j = 0; j < comps; j++) {
			*pLevelBuf = pMins[j];	
			pLevelBuf++;
		}		
	}
}

//updates brick map, i.e., data stucture storing the information which bricks are uniform
//NB: nPrevSkipped member has a temporary value 
void mafBrickedFileWriter::CreateBricksIndexTable(int nCurBrickPlane)
{
	BBF_IDX_EXITEM ssItem;
	
	int nIndex = nCurBrickPlane*m_NBricksDim[1];	//base line index
	for (int y = 0, idx = 0; y < m_NBricksDim[1]; y++)
	{
		bool bCurMode = false;		
		for (int x = 0; x < m_NBricksDim[0]; x++, idx++)
		{
			if (!m_PBricksValidity[idx])	//increase number of skipped bricks
				m_PMainIdxTable[nIndex].nPrevSkipped++;

			if (m_PBricksValidity[idx] != bCurMode ||	//change in mode
				(bCurMode && x + 1 == m_NBricksDim[0]))	//or last item in the line
			{
				//either begin or end of uniformity
				if (!bCurMode)
					ssItem.nFromBrIndex = (unsigned short)x;
				
				if (bCurMode || (x + 1 == m_NBricksDim[0]))	//last item
				{
					//ending previous block
					ssItem.nToBrIndex = (unsigned short)(x - 1);

					if (m_PMainIdxTable[nIndex].nListLength == 0)
					{
						//first item goes to the main block
						m_PMainIdxTable[nIndex].nFromBrIndex = ssItem.nFromBrIndex;
						m_PMainIdxTable[nIndex].nToBrIndex = ssItem.nToBrIndex;
					}
					else 
					{
						//first extra item
						if (m_PMainIdxTable[nIndex].nListLength == 1)
							m_PMainIdxTable[nIndex].nNextItemIndex = (int)m_ExtraBrckMAP.size();

						m_ExtraBrckMAP.push_back(ssItem);
					}

					m_PMainIdxTable[nIndex].nListLength++;
				}

				bCurMode = !bCurMode && (x + 1 != m_NBricksDim[0]);
			}
		}

		assert(bCurMode == false);
		nIndex++;
	}
}


//This method updates the output (i.e., it performs the bricking)	
/*virtual*/ bool mafBrickedFileWriter::Update()
{
	if (this->GetMTime() <= m_LastUpdateTime) {
		return true; //no change
	}

	mafEventMacro(mafEvent(this, PROGRESSBAR_SHOW, this));

	mafString szMsg = _("Initialization ...");
	mafEventMacro(mafEvent(this, PROGRESSBAR_SET_TEXT, &szMsg));
	mafEventMacro(mafEvent(this, PROGRESSBAR_SET_VALUE, (long)0));

	try
	{
		//create file
    m_BrickFile = vtkMAFFile2::New();
		m_BrickFile->Create(m_BrickFileName);
    m_BrickFile->Write( &m_FileHeader, sizeof(BBF_HEADER));

		ExecuteInformation();	//initialize "global" variables
		AllocateBuffers();		//allocate memory for bricks, etc.		

		//perform sampling + bricking (stores also rectilinear grid)
		ExecuteData();

		//time to store low resolution
		szMsg = _("Writing LOW Resolution map ...");
		mafEventMacro(mafEvent(this, PROGRESSBAR_SET_TEXT, &szMsg));    
		
		m_BrickFile->Write( m_PLowResLevel, m_NBricksDimSize[2]*m_NVoxelSizeInB);

//		int f = IOFileUtils::CreateFile(wxString::Format("g:\\brckmap_%d_%d_%d.raw",
//			m_nBricksDim[0], m_nBricksDim[1], m_nBricksDim[2]));
//		IOFileUtils::WriteFile(f, m_pLowResLevel, m_nBricksDimSize[2]*m_nVoxelSizeInB);
//		IOFileUtils::CloseFile(f);

		//and our index table
		szMsg = _("Writing index table ...");
		mafEventMacro(mafEvent(this, PROGRESSBAR_SET_TEXT, &szMsg));
				
		int nPrevSum = 0;		
		int nCount = m_NBricksDim[1]*m_NBricksDim[2];
		for (int i = 0; i < nCount; i++) {
			int nNextVal = m_PMainIdxTable[i].nPrevSkipped;
			m_PMainIdxTable[i].nPrevSkipped = nPrevSum;
			nPrevSum += nNextVal;			
		}
		

		m_FileHeader.idxtblofs = m_BrickFile->GetCurrentPos();
    m_BrickFile->Write(m_PMainIdxTable, nCount*sizeof(BBF_IDX_MAINITEM));

		m_FileHeader.extra_idx_items = (unsigned long)m_ExtraBrckMAP.size();
		for (int i = 0; i < (int)m_ExtraBrckMAP.size(); i++) {
			m_BrickFile->Write(&m_ExtraBrckMAP[i], sizeof(BBF_IDX_EXITEM));
		}

    //compute rectilinear coordinates and store them
    ProcessCoordinates(); 

		m_BrickFile->Seek(0, SEEK_SET);
		m_BrickFile->Write( &m_FileHeader, sizeof(BBF_HEADER));
	}
	catch (std::exception& e)
	{
		//time to display the message
		wxMessageBox(e.what(), _("Error: Bricking failed"), wxOK | wxICON_EXCLAMATION);

		//delete the file
		m_BrickFile->Close();
    m_BrickFile->Delete();
    m_BrickFile = NULL;
#pragma warning(suppress: 6031) // warning C6031: Return value ignored: '_unlink'
		_unlink(m_BrickFileName);

		return false;
	}


	szMsg = _("Finalization ...");
	mafEventMacro(mafEvent(this, PROGRESSBAR_SET_TEXT, &szMsg));

	DeallocateBuffers();	
	m_BrickFile->Close();	
  m_BrickFile->Delete();
  m_BrickFile = NULL;

	mafEventMacro(mafEvent(this, PROGRESSBAR_HIDE, this));

	m_LastUpdateTime.Modified();
	return true;
}
