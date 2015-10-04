/**=======================================================================

File:    	 mafBrickedFileReader.cpp
Language:  C++
Date:      13:2:2008   14:25
Version:   $Revision: 1.1.2.5 $
Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)

Copyright (c) 2008
University of Bedfordshire
=========================================================================
Reads the BBF file, writing data into vtkImageDataSet
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafBrickedFileReader.h"

mafCxxTypeMacro(mafBrickedFileReader);

#include "vtkDataArray.h"
#include "vtkPointData.h"

#include "mafMemDbg.h"
#include "vtkMAFIdType64.h"
#include "mafProgressBarHelper.h"

#define BBF_DATACACHE_MAXCOUNT	7	//central brick + bricks around (L1 resample only)

mafBrickedFileReader::mafBrickedFileReader()
{				 
	m_DataSet = NULL;
  m_DataSetRLG = NULL;
	m_BROIValid = false;
	
	m_PBrickDataCache = NULL;
	
	m_VOI[0] = m_VOI[2] = m_VOI[4] = 0;
	m_VOI[1] = m_VOI[3] = m_VOI[5] = 0xFFFF;	//short max
}

mafBrickedFileReader::~mafBrickedFileReader()
{	
	CloseBrickFile();
	vtkDEL(m_DataSet);
  vtkDEL(m_DataSetRLG);
}


//Sets a new associated output data set
//NB: the reference count of the specified output data set is increased
void mafBrickedFileReader::SetOutputDataSet(vtkImageData* ds)
{
	if (ds != m_DataSet)
	{
		vtkDEL(m_DataSet);

		if ((m_DataSet = ds) != NULL)
			m_DataSet->Register(NULL);

		m_BROIValid = false;
		this->Modified();
	}
}

//Sets a new associated output data set
//NB: the reference count of the specified output data set is increased
//This forces the Execute to produce vtkRectilinearGrid object even, if
//the underlaying grid is regular one (use IsRectilinearGrid to check it)
void mafBrickedFileReader::SetOutputRLGDataSet(vtkRectilinearGrid* ds)
{
  if (ds != m_DataSetRLG)
  {
    vtkDEL(m_DataSetRLG);

    if ((m_DataSetRLG = ds) != NULL)
      m_DataSetRLG->Register(NULL);

    m_BROIValid = false;
    this->Modified();
  }
}

//opens the brick file, loading index table, etc. 
/*virtual*/ void mafBrickedFileReader::OpenBrickFile() throw(...)
{
	//close previously opened file
	CloseBrickFile();

  m_BrickFile = vtkMAFFile2::New();
	m_BrickFile->Open(m_BrickFileName);
	m_BrickFile->Read(&m_FileHeader, sizeof(m_FileHeader));
	if (m_FileHeader.signature != mafBrickedFile::m_Signature) {
		m_BrickFile->Close();
		throw std::ios::failure(_("Not BBF file or corrupted one."));
	}

	//recompute global information
	for (int i = 0; i < 3; i++) 
	{
		m_NBricksDim[i] = (m_FileHeader.dims[i] + m_FileHeader.bricksize - 1) / 
			m_FileHeader.bricksize;
	}

	//NB: GetOutputDataSet to force the construction of m_DataSet, if it does not exist
	GetOutputDataSet()->SetScalarType(m_FileHeader.datatype);	
	m_NVoxelSizeInB = m_FileHeader.numcomps*m_DataSet->GetScalarSize();

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

	AllocateBuffers();
}

//closes the currently opened brick file (if there is any)
//releasing memory allocated for index table, etc.
/*virtual*/ void mafBrickedFileReader::CloseBrickFile()
{	
	DeallocateBuffers();
  if (m_BrickFile != NULL)
  {
    m_BrickFile->Close();
    m_BrickFile->Delete();
    m_BrickFile = NULL;
  }
}

//allocates the required buffers
/*virtual*/ void mafBrickedFileReader::AllocateBuffers()  throw(...)
{
	int nLRSize = m_NBricksDimSize[2]*m_NVoxelSizeInB;
	m_BrickFile->Seek( m_FileHeader.idxtblofs - nLRSize);

	m_PLowResLevel = new char[nLRSize];
	m_BrickFile->Read( m_PLowResLevel, nLRSize);

	int nBrickLines = m_NBricksDim[1]*m_NBricksDim[2];
	m_PMainIdxTable = new BBF_IDX_MAINITEM[nBrickLines];
	m_BrickFile->Read( m_PMainIdxTable, nBrickLines*sizeof(BBF_IDX_MAINITEM));

	m_PExIdxTable = new BBF_IDX_EXITEM[m_FileHeader.extra_idx_items];
	m_BrickFile->Read( m_PExIdxTable, m_FileHeader.extra_idx_items*sizeof(BBF_IDX_EXITEM));

  if (this->IsRectilinearGrid())
  {
    for (int i = 0; i < 3; i++)
    {
      m_PXYZCoords[i] = vtkDoubleArray::New();
      m_PXYZCoords[i]->Allocate(m_FileHeader.dims[i]);   

      //now, we need to store them
      m_BrickFile->Read(m_PXYZCoords[i]->WritePointer(0, m_FileHeader.dims[i]), 
        m_FileHeader.dims[i]*sizeof(double));
    }
  }

	m_PBrickDataCache = new char[m_NBrickSizeInB[2]];	



	//wxString szMsg = wxString::Format("SR:%d, BS:%d, BDims:%dx%dx%d, IdxLen:%.2f",
	//	m_FileHeader.sample_rate, m_FileHeader.bricksize,
	//	m_nBricksDim[0],m_nBricksDim[1],m_nBricksDim[2],
	//	(m_FileHeader.extra_idx_items + nBrickLines) / (double)nBrickLines);

	//wxMessageBox(szMsg);

}

//deallocates the buffers created in AllocateBuffers
/*virtual*/ void mafBrickedFileReader::DeallocateBuffers()  throw(...)
{
	cppDEL(m_PBrickDataCache);
	cppDEL(m_PLowResLevel);
	cppDEL(m_PMainIdxTable);
  cppDEL(m_PExIdxTable);

  for (int i = 0; i < 3; i++) {    
    vtkDEL(m_PXYZCoords[i]);
  }
}

//Called by Update to fill some internal structures
/*virtual*/ void mafBrickedFileReader::ExecuteInformation() throw(...)
{
	vtkImageData* output = GetOutputDataSet();

	//compute extents in samples that covers the specified VOI
	int sampExtents[6];
	int RSR = m_FileHeader.sample_rate;
	for (int i = 0; i < 3; i++) 
	{
		sampExtents[2*i] = m_VOI[2*i] / RSR;		
		sampExtents[2*i + 1] = m_VOI[2*i + 1] / RSR;
	}
	
	//verify the user specified VOI, if it is within dimensions
	for (int i = 0; i < 3; i++)
	{
		if (sampExtents[2*i] < 0)
			sampExtents[2*i] = 0;

		if (sampExtents[2*i] >= m_FileHeader.dims[i])
			sampExtents[2*i] = m_FileHeader.dims[i] - 1;

		if (sampExtents[2*i + 1] >= m_FileHeader.dims[i])
			sampExtents[2*i + 1] = m_FileHeader.dims[i] - 1;
	}
		
	////align extents to bricksize (large always)
	////this is to speed up the further processing
	//for (int i = 0; i < 3; i++) 
	//{
	//	sampExtents[2*i] = (sampExtents[2*i] / m_nBrickSize[0]) * m_nBrickSize[0];

	//	int res = (sampExtents[2*i + 1] + 1) % m_nBrickSize[0];
	//	sampExtents[2*i + 1] = (((sampExtents[2*i + 1] + 1) / 
	//		m_nBrickSize[0] + (res != 0)) * m_nBrickSize[0]) - 1;

	//	assert(sampExtents[2*i + 1] / m_nBrickSize[0] < m_nBricksDim[i]);

	//	//check if new requested dim is not larger that supported one
	//	if (sampExtents[2*i + 1] >= m_FileHeader.dims[i])
	//		sampExtents[2*i + 1] = m_FileHeader.dims[i] - 1;
	//}

	//realign m_VOI to sampExtents
	for (int i = 0; i < 6; i++) {
		m_VOI[i] = sampExtents[i]*m_FileHeader.sample_rate;
	}

	int outLDims[3], outLStart[3];
	vtkDataArray* outLScalars = NULL;		

	//if the resample mode and resample rate is the same as the one 
	//used during the last update, we might have already some data
	//loaded in the output data set
	if (m_BROIValid)
	{
		//data in the intersection of the current VOI and ValidROI
		//can be reused (no need to reload it)				
		for (int i = 0; i < 3; i++)
		{
			outLStart[i] = m_ValidROI[2*i];	//store origin

			if (m_VOI[2*i] > m_ValidROI[2*i])
				m_ValidROI[2*i] = m_VOI[2*i];

			if (m_VOI[2*i + 1] < m_ValidROI[2*i + 1])
				m_ValidROI[2*i + 1] = m_VOI[2*i + 1];

			if (m_ValidROI[2*i + 1] < m_ValidROI[2*i]) {
				m_BROIValid = false;	//we are out
				break;
			}
		}

		if (m_BROIValid)
		{
			//store scalars, to be used later
			vtkPointData* pp = output->GetPointData();
			outLScalars = pp->GetScalars();
			if (outLScalars == NULL)
				m_BROIValid = false;
			else
			{
				outLScalars->Register(NULL);
				pp->SetScalars(NULL);

				//and we will also need extents
				output->GetDimensions(outLDims);
			}
		}
	}	
	
	//Now setup the new output
	int outExtent[6];
	for (int i = 0; i < 3; i++)
	{
		outExtent[2*i] = 0;
		outExtent[2*i + 1] = sampExtents[2*i + 1] - sampExtents[2*i];
	}

	output->SetExtent(outExtent);
	output->SetWholeExtent(outExtent);
	output->SetUpdateExtentToWholeExtent();

	//now we must update also origin
	double sp[3], origin[3];
	this->GetDataSpacing(sp);
	this->GetDataOrigin(origin);
	for (int i = 0; i < 3; i++) {		
		origin[i] += m_VOI[2*i]*sp[i] / m_FileHeader.sample_rate;		
	}	

	output->SetOrigin(origin);
	output->SetSpacing(sp);		

	//set scalar type, number of components and we are ready
	output->SetScalarType(m_FileHeader.datatype);
	output->SetNumberOfScalarComponents(m_FileHeader.numcomps);		
	output->AllocateScalars();

	if (m_BROIValid)
	{
		//copy the appropriate part from previous scalars
		int srcPos[3], dstPos[3], cpySize[3];	//extent to be copied
		for (int i = 0; i < 3; i++)
		{
			srcPos[i] = (m_ValidROI[2*i] - outLStart[i]) / m_FileHeader.sample_rate;
			dstPos[i] = (m_ValidROI[2*i] - m_VOI[2*i]) / m_FileHeader.sample_rate;
			cpySize[i] = (m_ValidROI[2*i + 1] - m_ValidROI[2*i]) / m_FileHeader.sample_rate + 1;
		}

		int srcIncr[2], dstIncr[2];
		srcIncr[0] = outLDims[0]*m_NVoxelSizeInB;
		srcIncr[1] = outLDims[1]*srcIncr[0];

		dstIncr[0] = (outExtent[1] + 1)*m_NVoxelSizeInB;
		dstIncr[1] = (outExtent[3] + 1)*dstIncr[0];
		
		char* pSrcData = (char*)outLScalars->GetVoidPointer(0);
		char* pDstData = (char*)m_DataSet->GetScalarPointer();				
		pSrcData += srcPos[2]*srcIncr[1] + srcPos[1]*srcIncr[0] + srcPos[0]*m_NVoxelSizeInB;
		pDstData += dstPos[2]*dstIncr[1] + dstPos[1]*dstIncr[0] + dstPos[0]*m_NVoxelSizeInB;

		//copying itself
		int nLineSizeInB = cpySize[0]*m_NVoxelSizeInB;
		for (int zz = 0; zz < cpySize[2]; zz++)
		{
			char* pDstDataY = pDstData;
			char* pSrcDataY = pSrcData;

			for (int yy = 0; yy < cpySize[1]; yy++)
			{
				memcpy(pDstDataY, pSrcDataY, nLineSizeInB);
				pSrcDataY += srcIncr[0];
				pDstDataY += dstIncr[0];
			}

			pSrcData += srcIncr[1];
			pDstData += dstIncr[1];			
		}

		outLScalars->UnRegister(NULL);
	}	
}

//computes extents in inner bricks and boundary bricks	
//for the given extent VOI that is specified in voxels (of highest resolution level)
void mafBrickedFileReader::GetBricksExtent(int VOI[6], int inBExt[6], int bndBExt[6])
{	
	for (int i = 0; i < 3; i++)
	{	
		//VOI is always aligned to sample rate
		int Wmin = VOI[2*i] / m_FileHeader.sample_rate;
		int Wmax = VOI[2*i + 1] / m_FileHeader.sample_rate;

		bndBExt[2*i] = inBExt[2*i] = Wmin / m_NBrickSize[0];
		if ((Wmin % m_NBrickSize[0]) != 0)
			inBExt[2*i]++;		//boundary detected
		
		bndBExt[2*i + 1] = inBExt[2*i + 1] = (Wmax + 1) / m_NBrickSize[0] - 1;
		if ((Wmax + 1) % m_NBrickSize[0] != 0)
			bndBExt[2*i + 1]++;	//boundary detected
	}
}

//processes data
/*virtual*/ void mafBrickedFileReader::ExecuteData() throw(...)
{	
	//fill the output data set (except for already filled valid region)
	//first, we need to detect the area to be processed	

	int inBExt[6], inVBExt[6];
	int bndBExt[6], bndVBExt[6];
	GetBricksExtent(m_VOI, inBExt, bndBExt);

	if (m_BROIValid)	//if ROI is valid, get extents for it as well
		GetBricksExtent(m_ValidROI, inVBExt, bndVBExt);
	else
	{
		//ROI is not valid, invalidate also these cells
		for (int i = 0; i < 6; i++) {
			inVBExt[i] = bndVBExt[i] = INT_MAX;
		}
	}

	//number of voxels (in original highest resolution) per one brick size
	int nOrigVxlsPerBS = m_NBrickSize[0]*m_FileHeader.sample_rate;

	//extents for one brick
	int brckExt[6], brckMinExt[6], brckMaxExt[6];	
	for (int i = 0; i < 6; i += 2)
	{
		brckMinExt[i] = 0;
		brckMinExt[i + 1] = m_VOI[i] / m_FileHeader.sample_rate - bndBExt[i]*m_NBrickSize[0];

		brckMaxExt[i] = m_NBrickSize[0] - 1;
		brckMaxExt[i + 1] = m_VOI[i + 1] / m_FileHeader.sample_rate - bndBExt[i + 1]*m_NBrickSize[0];
	}	

	//compute variables to help us compute the output address	
	int outIncr[3], outIncrInB[3];
	vtkImageData* output = GetOutputDataSet();	
	output->GetIncrements(outIncr);
	for (int i = 0; i < 3; i++) 
  {
    //BES: 11.7.2008 - GetIncrements takes number of components into account =>
    //we need to make some corrections
		outIncrInB[i] = outIncr[i]*(m_NVoxelSizeInB / m_FileHeader.numcomps);
	}

	int RSR = m_FileHeader.sample_rate;
	char* pOutPtrZ = (char*)output->GetScalarPointer();	

	//compute the initial BrickLine (i.e., index into the index table)
	//and compute also initial index to low resolution map
	int nBrickLineZ = (bndBExt[4]*m_NBricksDim[1] + bndBExt[2]);
	int nLRIdxZ = (nBrickLineZ*m_NBricksDim[0] + bndBExt[0])*m_NVoxelSizeInB;
		
	int xyzb[3];
	for (xyzb[2] = bndBExt[4]; xyzb[2] <= bndBExt[5]; xyzb[2]++, nBrickLineZ += m_NBricksDim[1], 
		nLRIdxZ += m_NBricksDimSize[1]*m_NVoxelSizeInB)
	{
		m_ProgressHelper->UpdateProgressBar(100*(xyzb[2] - bndBExt[4]) / (bndBExt[5] - bndBExt[4] + 1));

		//planes from bndBExt[4] to inBExt[4] (exclusively)
		//and planes from inBExt[5] (exclusively) to bndExt[5]
		//contains boundary bricks and must be processed differently
		int nBrickLineY = nBrickLineZ;
		int nBrickIndexY = nBrickLineY*m_NBricksDimSize[0];
		int nLRIdxY = nLRIdxZ;

		brckExt[4] = brckMinExt[4 + (xyzb[2] == bndBExt[4])];
		brckExt[5] = brckMaxExt[4 + (xyzb[2] == bndBExt[5])];

		bool zbValid = xyzb[2] >= inVBExt[4] && xyzb[2] <= inVBExt[5];		
		char* pOutPtrY = pOutPtrZ;

		for (xyzb[1] = bndBExt[2]; xyzb[1] <= bndBExt[3]; xyzb[1]++, nBrickLineY++,
			nBrickIndexY += m_NBricksDimSize[0], nLRIdxY += m_NBricksDimSize[0]*m_NVoxelSizeInB)
		{	
			//get the index table item
			BBF_IDX_MAINITEM* pIdxMain = &m_PMainIdxTable[nBrickLineY];
			BBF_IDX_EXITEM* pIdxEx = &m_PExIdxTable[pIdxMain->nNextItemIndex];
			int nRemPos = pIdxMain->nListLength - 1;

			//compute number of bricks skipped before the first item
			int nBrickIndex = nBrickIndexY;
			int nSkippedBricks = pIdxMain->nPrevSkipped;
			for (xyzb[0] = 0; xyzb[0] < bndBExt[0]; xyzb[0]++, nBrickIndex++)
			{
				if (IsBrickUniform(xyzb[0], pIdxMain, pIdxEx, nRemPos))
					nSkippedBricks++;
			}
			
			brckExt[2] = brckMinExt[2 + (xyzb[1] == bndBExt[2])];
			brckExt[3] = brckMaxExt[2 + (xyzb[1] == bndBExt[3])];

			int nLRIdx = nLRIdxY;	
			bool ybValid = zbValid && xyzb[1] >= inVBExt[2] && xyzb[1] <= inVBExt[3];
			char* pOutPtrX = pOutPtrY;

			for (xyzb[0] = bndBExt[0]; xyzb[0] <= bndBExt[1]; xyzb[0]++, 
				nLRIdx += m_NVoxelSizeInB, nBrickIndex++
				)
			{
				brckExt[0] = brckMinExt[xyzb[0] == bndBExt[0]];
				brckExt[1] = brckMaxExt[xyzb[0] == bndBExt[1]];

				//check first, if it is not uniform
				bool bUniform = nRemPos < 0 || IsBrickUniform(xyzb[0], pIdxMain, pIdxEx, nRemPos);
				if (bUniform)	//increase number of skipped bricks, if it is uniform
					nSkippedBricks++;

				//check if this brick should be processed
				if (!ybValid || xyzb[0] < inVBExt[0] || xyzb[0] > inVBExt[1]) 
				{
					//unfortunately, this brick has to be processed
					//compute index of this current cell					 
					if (bUniform)
						FillBrick(m_PBrickDataCache, &m_PLowResLevel[nLRIdx]);						
					else					
						LoadBrick(nBrickIndex - nSkippedBricks, m_PBrickDataCache);					

					//having the data in pBrickData, copy its bytes													
					CopyBrickData(m_PBrickDataCache, brckExt, pOutPtrX, outIncrInB);
				} //end if - check of validity

				//advance addresses
				pOutPtrX += (brckExt[1] - brckExt[0] + 1)*outIncrInB[0];
			} //end for x

			pOutPtrY += (brckExt[3] - brckExt[2] + 1)*outIncrInB[1];
		} //end for xyzb[1]

		//advance to next brick plane
		pOutPtrZ += (brckExt[5] - brckExt[4] + 1)*outIncrInB[2];
	} //end for xyzb[2]

	memcpy(&m_ValidROI[0], &m_VOI[0], sizeof(int)*6);
	m_BROIValid = true;  
}

/** processes data by converting m_DataSet into m_DataSetRLG */
/*virtual*/ void mafBrickedFileReader::ExecuteRLGData()
{
  int* dims = m_DataSet->GetDimensions();
  double* origin = m_DataSet->GetOrigin();
  double* spacing = m_DataSet->GetSpacing();    

  vtkDoubleArray* XYZCoords[3];
  for (int i = 0; i < 3; i++)
  {
    XYZCoords[i] = vtkDoubleArray::New();
    XYZCoords[i]->Allocate(dims[i]);
    double* pDstPtr = XYZCoords[i]->WritePointer(0, dims[i]);

    if (m_PXYZCoords[i] != NULL)
    {
      //copy part of it
      memcpy(pDstPtr, m_PXYZCoords[i]->GetPointer(
        m_VOI[2*i] / m_FileHeader.sample_rate), //VOI is given in highest resolution
        dims[i]*sizeof(double));
    }
    else
    {
      //the coordinates not specified        
      if (spacing[i] == 0.0) //if there is no spacing, there is 0.0
        spacing[i] = 1.0;

      for (int j = 0; j < dims[i]; j++) {
        pDstPtr[j] = origin[i] + j*spacing[i];
      }
    }            
  }

  vtkDataArray *scalars = m_DataSet->GetPointData()->GetScalars();
  m_DataSetRLG->SetXCoordinates(XYZCoords[0]);
  m_DataSetRLG->SetYCoordinates(XYZCoords[1]);
  m_DataSetRLG->SetZCoordinates(XYZCoords[2]);
  m_DataSetRLG->SetDimensions(dims);
  m_DataSetRLG->GetPointData()->SetScalars(scalars);

  for (int i = 0; i < 3; i++) {
    XYZCoords[i]->Delete();
  }
}

//This method updates the output (i.e., it performs the loading)	
//returns false if an error occurs
/*virtual*/ bool mafBrickedFileReader::Update()
{
	if (this->GetMTime() <= m_LastUpdateTime) {
		return true; //no change
	}

	m_ProgressHelper = new mafProgressBarHelper(m_Listener);
	m_ProgressHelper->InitProgressBar("",false);
	m_ProgressHelper->SetBarText("Retrieving data ...");
	
	try
	{
		//check if we still work with the same file
		if (!m_LUBrickFileName.Equals(m_BrickFileName)) {
			OpenBrickFile();	//if not open new one

			m_LUBrickFileName = m_BrickFileName;
			m_BROIValid = false;
		}

		ExecuteInformation();	//initialize variables				
		ExecuteData();	  		//get data

    if (this->IsRectilinearGrid())
      GetOutputRLGDataSet();  //force the construction of rectilinear grid  

    //if rectilinear grid is to be available, we will need to convert it to RLG
    if (m_DataSetRLG != NULL)    
      ExecuteRLGData();     //convert vtkImageData to vtkRectilinearGrid
	}
	catch (std::exception& e)
	{
		//time to display the message
		wxMessageBox(e.what(), _("Error: Loading failed"), 
			wxOK | wxICON_EXCLAMATION);

		return false;
	}

	cppDEL(m_ProgressHelper);

	m_LastUpdateTime.Modified();
	return true;
}

