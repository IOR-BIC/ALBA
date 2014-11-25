/**=======================================================================
  
  File:    	 mafVolumeLargeWriter.cpp
  Language:  C++
  Date:      8:2:2008   11:27
  Version:   $Revision: 1.1.2.3 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVolumeLargeWriter.h"
#include "mafBrickedFileWriter.h"
#include "vtkMAFFileDataProvider.h"
#include <wx/busyinfo.h>

mafCxxTypeMacro(mafVolumeLargeWriter);
#include "mafMemDbg.h"

//#define _PROFILE_LARGEDATA_

mafVolumeLargeWriter::mafVolumeLargeWriter()
{
  m_InputDataSet = NULL;	
  m_PInputXYZCoords[0] = NULL;
  m_PInputXYZCoords[1] = NULL;
  m_PInputXYZCoords[2] = NULL;
	m_DblLimitCoef = 1.35;	//NB. upper bound is lest than 2, 
							            //even 30 levels has 1.2+
	m_Listener = NULL;
}

mafVolumeLargeWriter::~mafVolumeLargeWriter()
{	
	//all should be removed in Update
	vtkDEL(m_InputDataSet);
  vtkDEL(m_PInputXYZCoords[0]);
  vtkDEL(m_PInputXYZCoords[1]);
  vtkDEL(m_PInputXYZCoords[2]);
}

//Sets a new associated input data set
//NB: the reference count of the specified input data set is increased
void mafVolumeLargeWriter::SetInputDataSet(vtkMAFLargeImageData* ds)
{
	if (ds != m_InputDataSet)
	{
		vtkDEL(m_InputDataSet);

		if ((m_InputDataSet = ds) != NULL)
			m_InputDataSet->Register(NULL);	
	}
}

//------------------------------------------------------------------------
//Specifies the grid coordinates in x-direction
void mafVolumeLargeWriter::SetInputXCoordinates(vtkDoubleArray* pCoords)
//------------------------------------------------------------------------
{
  if (pCoords != m_PInputXYZCoords[0])
  {
    vtkDEL(m_PInputXYZCoords[0]);  
    if (NULL != (m_PInputXYZCoords[0] = pCoords))    
      m_PInputXYZCoords[0]->Register(NULL);    
  }
}

//------------------------------------------------------------------------
//Specifies the grid coordinates in y-direction
void mafVolumeLargeWriter::SetInputYCoordinates(vtkDoubleArray* pCoords)
//------------------------------------------------------------------------
{
  if (pCoords != m_PInputXYZCoords[1])
  {
    vtkDEL(m_PInputXYZCoords[1]);  
    if (NULL != (m_PInputXYZCoords[1] = pCoords))    
      m_PInputXYZCoords[1]->Register(NULL);
  }
}

//------------------------------------------------------------------------
//Specifies the grid coordinates in z-direction
void mafVolumeLargeWriter::SetInputZCoordinates(vtkDoubleArray* pCoords)
//------------------------------------------------------------------------
{
  if (pCoords != m_PInputXYZCoords[2])
  {
    vtkDEL(m_PInputXYZCoords[2]);  
    if (NULL != (m_PInputXYZCoords[2] = pCoords))
      m_PInputXYZCoords[2]->Register(NULL);    
  }
}


//computes the volume size in bytes
vtkIdType64 mafVolumeLargeWriter::GetVOISizeInBytes()
{
	int VOI[6];
	m_InputDataSet->GetVOI(VOI);

	vtkIdType64 nTotalSize = ((vtkIdType64)(VOI[5] - VOI[4] + 1))*
		(VOI[3] - VOI[2] + 1)*(VOI[1] - VOI[0] + 1);	//volume size in voxels

	nTotalSize *= m_InputDataSet->GetNumberOfScalarComponents()*
		m_InputDataSet->GetScalarSize();	//volume size in bytes

	return nTotalSize;
}

//computes the sample rate needed for the volume of nSize so it
//can be kept in memory of nMemLim
int mafVolumeLargeWriter::ComputeSampleRate(vtkIdType64 nSize, vtkIdType64 nMemLim)
{		
	int samp_rate = 1;	
	if (nSize > nMemLim)
	{
		vtkIdType64 fsize_new;

		do
		{
			fsize_new = nSize;
			samp_rate++;

			for (int i = 0; i < 3; i++) {
				fsize_new /= samp_rate;
			}
		}
		while (fsize_new > nMemLim);
	}

	//round the sample rate to the nearest higher 2^k value
	int max_sample = 1;
	while (max_sample < samp_rate) {
		max_sample *= 2;
	}

	return max_sample;
}

//This method updates the output (i.e., it performs the bricking)
/*virtual*/ bool mafVolumeLargeWriter::Update()
{	
	wxBusyCursor cursor;
	wxBusyInfo wait(_("Constructing LOD for the volume ..."));

	try
	{
		if (m_InputDataSet == NULL) {
			throw std::invalid_argument(_("Invalid argument. Input Data Set cannot be NULL"));
		}

#ifdef _PROFILE_LARGEDATA_
    LARGE_INTEGER liBegin;
    ::QueryPerformanceCounter(&liBegin);
#endif //_PROFILE_LARGEDATA_

		//compute the volume size
		vtkIdType64 nTotalSize = GetVOISizeInBytes();
		int max_sample = ComputeSampleRate(nTotalSize / 1024, 1024
			/*m_InputDataSet->GetMemoryLimit()*/);		
		
		//compute the limit for our LODs
		nTotalSize = (vtkIdType64)(nTotalSize*m_DblLimitCoef);
		vtkIdType64 nNewTotalSize = nTotalSize;
    int nLevels = CreateLODs(max_sample, nNewTotalSize);

#ifdef _PROFILE_LARGEDATA_
    LARGE_INTEGER liEnd, liFreq;
    ::QueryPerformanceCounter(&liEnd);
    ::QueryPerformanceFrequency(&liFreq);
    
    int VOI[6];
    m_InputDataSet->GetVOI(VOI);

    FILE* fLog = fopen("mafVolumeLargeWriter.log", "at");
    fprintf(fLog, "%dx%dx%d (%.2f MB -> %.2f MB in %d levels) in %.2f s.\n", 
      VOI[1] + 1, VOI[3] + 1, VOI[5] + 1, (double)(GetVOISizeInBytes() / (1024*1024.0)),
      (double)(nNewTotalSize / (1024*1024.0)), nLevels,
      ((double)(liEnd.QuadPart - liBegin.QuadPart)) / liFreq.QuadPart);
    fclose(fLog);          
#endif //_PROFILE_LARGEDATA_
	}
	catch (std::exception& e)
	{
		//time to display the message
		wxMessageBox(e.what(), _("Error: Bricking process failed"), wxOK | wxICON_EXCLAMATION);
		return false;
	}

	return true;
}

//computes the best brick size for the given sample rate
#define MEM_PLANE_LIMIT	64*1024*1024	//64MB limit
int mafVolumeLargeWriter::ComputeBrickSize(int nSampleRate, int nMaxSampleRate)
{
	//we prefer brick size of 16, which is good constant for any file 
	//of sizes ranging from 1GB to 64GB that are sampled (nMaxSampleRate)
	//using 16MB memory limit
	int nBrickSize = (nMaxSampleRate - (nSampleRate - 1)) & ~3;	//rounded four
	if (nBrickSize == 0)
		nBrickSize = 4;
	else if (nBrickSize > 16)
		nBrickSize = 16;	
	
	int VOI[6];
	m_InputDataSet->GetVOI(VOI);

	int nPlSize = ((VOI[3] - VOI[2] + 1)*(VOI[1] - VOI[0] + 1) / 
		(nSampleRate*nSampleRate));		//plane size in voxels
	nPlSize *= m_InputDataSet->GetNumberOfScalarComponents()*
		m_InputDataSet->GetScalarSize();		//plane size in bytes

	//64MB memory limit for one plane	
	while (nPlSize*nBrickSize > MEM_PLANE_LIMIT && nBrickSize > 4) {
		nBrickSize -= 4;
	}

	return nBrickSize;
}

//creates BBF files with LOD with sample rate ranges from 1 to nMaxSampleRate
//skipping less important levels in order to fit into nTotalMaxSize Bytes
//returns number of constructed levels and in nTotalMaxSize their size in bytes
int mafVolumeLargeWriter::CreateLODs(int nMaxSampleRate, vtkIdType64& nTotalMaxSize) throw(...)
{
	//construct name
	wxString szPath, szFile, szExt;	
	wxSplitPath(m_BrickFileName, &szPath, &szFile, &szExt);
	wxString szFNamePref = szPath;
	if (!wxEndsWithPathSeparator(szPath))
		szFNamePref += wxFILE_SEP_PATH;	
	
	//create directory for our files
	if (!wxDirExists(szFNamePref))
		wxMkdir(szFNamePref);
	szFNamePref += wxFILE_SEP_PATH;
	szFNamePref += szFile;


	//process every LOD
	mafBrickedFileWriter bf;
	bf.SetListener(m_Listener);	
	bf.SetInputDataSet(m_InputDataSet);
  bf.SetInputXCoordinates(m_PInputXYZCoords[0]);
  bf.SetInputYCoordinates(m_PInputXYZCoords[1]);
  bf.SetInputZCoordinates(m_PInputXYZCoords[2]);

#ifdef _PROFILE_LARGEDATA_
  int VOI[6];
  m_InputDataSet->GetVOI(VOI);
  double dblVOISizeInMB = GetVOISizeInBytes() / (1024*1024.0);
  
  LARGE_INTEGER liFreq;
  ::QueryPerformanceFrequency(&liFreq);
#endif

  int nRetLevels = 0;
	vtkIdType64 nTotalSize = 0;
	for (int i = 1; i <= nMaxSampleRate/*nCount*/; i++)
	{
//		if (pOrder[i] <= nMaxSampleRate)
		{
#ifdef _PROFILE_LARGEDATA_
      LARGE_INTEGER liBegin;
      ::QueryPerformanceCounter(&liBegin);
#endif //_PROFILE_LARGEDATA_

			wxString szFName = wxString::Format("%s_%02d.bbf", szFNamePref, /*pOrder[i]*/i);

			bf.SetSampleRate(i/*pOrder[i]*/);
			bf.SetBrickSize(ComputeBrickSize(i/*pOrder[i]*/, nMaxSampleRate));
			bf.SetFileName(szFName);
			if (!bf.Update()) 
			{
				wxString szMsg = wxString::Format(_("Unable to create level %d"), i);
				throw std::ios::failure(szMsg.c_str());
			}

      vtkIdType64 nFileSize = vtkMAFFile2::GetFileSize(szFName);

#ifdef _PROFILE_LARGEDATA_
      LARGE_INTEGER liEnd;
      ::QueryPerformanceCounter(&liEnd);      

      FILE* fLog = fopen("mafVolumeLargeWriter2.log", "at");
      fprintf(fLog, "%dx%dx%d (%.2f MB -> %.2f MB; SR = %d, BS = %d) in %.2f s.\n", 
        VOI[1] + 1, VOI[3] + 1, VOI[5] + 1, dblVOISizeInMB,
        (double)(nFileSize / (1024*1024.0)), bf.GetSampleRate(), 
        bf.GetBrickSize(), ((double)(liEnd.QuadPart - liBegin.QuadPart)) / liFreq.QuadPart);
      fclose(fLog);          
#endif //_PROFILE_LARGEDATA_

      nRetLevels++;
			nTotalSize += nFileSize;
			if (nTotalSize >= nTotalMaxSize)
				break;	//out of space
		}
	}	

  nTotalMaxSize = nTotalSize;
  return nRetLevels;
}

//returns estimated total size for the given VOI and number of levels
//or all levels, if nLevels == 0
vtkIdType64 mafVolumeLargeWriter
	::GetEstimatedTotalSize(int nLevels)
{		
	if (nLevels == 0)
		nLevels = ComputeSampleRate(GetVOISizeInBytes() / 1024, 1024);


	int VOI[6], nDims[3];
	m_InputDataSet->GetVOI(VOI);
	for (int i = 0; i < 3; i++) {
		nDims[i] = VOI[2*i + 1] - VOI[2*i] + 1;
	}

	int nVoxelSize = m_InputDataSet->GetNumberOfScalarComponents()*
		m_InputDataSet->GetScalarSize();	//voxel size in bytes

	vtkIdType64 nTotalSize = 0;	
	for (int SR = 1; SR <= nLevels; SR++)
	{		
		int nCurDims[3];
		for (int i = 0; i < 3; i++) {
			nCurDims[i] = nDims[i] / SR;
		}

		//volume size in bytes
		vtkIdType64 nSize = (((vtkIdType64)nCurDims[0])*nCurDims[1]*
			nCurDims[2]) * nVoxelSize;		

		int BS = ComputeBrickSize(SR, nLevels);
		int nBrckDims[3];
		for (int i = 0; i < 3; i++) {
			nBrckDims[i] = (nDims[i] + BS - 1) / BS;
		}

		//brick map
		nSize += nBrckDims[0]*nBrckDims[1]*nBrckDims[2]*nVoxelSize;
		nSize += nBrckDims[1]*nBrckDims[2]*sizeof(mafBrickedFile::BBF_IDX_MAINITEM);
		nSize += sizeof(mafBrickedFile::BBF_HEADER);
    if (m_PInputXYZCoords[0] != NULL || m_PInputXYZCoords[1] != NULL || m_PInputXYZCoords[2] != NULL){
      nSize += sizeof(double)*(nCurDims[0] + nCurDims[1] + nCurDims[2]);
    }

		nTotalSize += nSize;
	}

	return nTotalSize;
}