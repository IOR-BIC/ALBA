/**=======================================================================
  
  File:    	 mafVolumeLargeReader.cpp
  Language:  C++
  Date:      20:2:2008   14:36
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

#include "mafVolumeLargeReader.h"
#include "vtkMAFIdType64.h"
#include "vtkPointData.h"

mafCxxTypeMacro(mafVolumeLargeReader);

//#define _PROFILE_LARGEDATA_

mafVolumeLargeReader::mafVolumeLargeReader(void)
{
	m_DataSet = NULL;
  m_DataSetRLG = NULL;
	m_PLevels = NULL;
	m_NLevels = 0;

	m_VOI[0] = m_VOI[2] = m_VOI[4] = 0;
	m_VOI[1] = m_VOI[3] = m_VOI[5] = 0xFFFF;	//short max
	m_VOI_Initialized = false;

	m_MemoryLimit = 16384;		//16M - experimentally found to be a good constant
	m_NCurrentLevel = 0;
	m_Listener = NULL;
}

mafVolumeLargeReader::~mafVolumeLargeReader(void)
{
	DestroyLODs();

	vtkDEL(m_DataSet);
  vtkDEL(m_DataSetRLG);
}

//Sets a new associated output data set
//NB: the reference count of the specified output data set is increased
void mafVolumeLargeReader::SetOutputDataSet(vtkImageData* ds)
{
	if (ds != m_DataSet)
	{
		vtkDEL(m_DataSet);

		if ((m_DataSet = ds) != NULL)
			m_DataSet->Register(NULL);
		
		this->Modified();
	}
}

//Sets a new associated output data set
//NB: the reference count of the specified output data set is increased
//This forces the Execute to produce vtkRectilinearGrid object even, if
//the underlaying grid is regular one (use IsRectilinearGrid to check it)
void mafVolumeLargeReader::SetOutputRLGDataSet(vtkRectilinearGrid* ds)
{
  if (ds != m_DataSetRLG)
  {
    vtkDEL(m_DataSetRLG);

    if ((m_DataSetRLG = ds) != NULL)
      m_DataSetRLG->Register(NULL);
    
    this->Modified();
  }
}


//Creates all levels
/*virtual*/ void mafVolumeLargeReader::CreateLODs() throw(...)
{
	wxString szPath, szFile, szExt;
	wxSplitPath(m_BrickFileName, &szPath, &szFile, &szExt);

	wxString szFNamePrefix = szPath;
	if (!wxEndsWithPathSeparator(szPath))
		szFNamePrefix += wxFILE_SEP_PATH;
	szFNamePrefix += szFile;	
	
	wxString szFName = szFNamePrefix + "_01.bbf";
	if (!::wxFileExists(szFName)) 
	{
		//error, there should be at least high resolution file
		throw std::ios::failure(wxString::Format(
			_("Highest resolution bricked file not found.\nFileName='%s'"), szFName).c_str());
	}
			
	//detect the coarsest level we have
	m_NLevels = 99;
	while (!::wxFileExists(wxString::Format("%s_%02d.bbf",
		szFNamePrefix, m_NLevels))) {
			m_NLevels--;
	}
	
	//and populate it
	m_PLevels = new mafBrickedFileReader*[++m_NLevels];
	memset(m_PLevels, 0, m_NLevels*sizeof(m_PLevels[0]));
	for (int i = 1; i < m_NLevels; i++)
	{
		szFName = wxString::Format("%s_%02d.bbf", szFNamePrefix, i);
		if (::wxFileExists(szFName))
		{
			m_PLevels[i] = new mafBrickedFileReader();
			m_PLevels[i]->SetFileName(szFName);
			m_PLevels[i]->SetEmptyVOI();
			m_PLevels[i]->Update();				//force the validation
		}
	}
}

//Releases every level
/*virtual*/ void mafVolumeLargeReader::DestroyLODs()
{
	while (m_NLevels > 0) 
	{		
		m_NLevels--;
		cppDEL(m_PLevels[m_NLevels]);		
	}

	cppDEL(m_PLevels);
	m_NLevels = 0;
}

//------------------------------------------------------------------------
//returns the best sample rate required for the given VOI and memory limit
/*virtual*/ int mafVolumeLargeReader::ComputeSampleRate(int VOI[6], unsigned int nMemoryLimit)
//------------------------------------------------------------------------
{
  vtkIdType64 nTotalSize = ((vtkIdType64)(VOI[1] - VOI[0] + 1))*
    (VOI[3] - VOI[2] + 1)*(VOI[5] - VOI[4] + 1)*
    m_PLevels[1]->GetNumberOfComponents()*
    m_PLevels[1]->GetDataTypeSize() / 1024;

  int sr = 1;
  vtkIdType64 nCurSize = nTotalSize;
  while (nCurSize > nMemoryLimit)
  {
    sr++;	//increment sample rate
    nCurSize = nTotalSize / (sr*sr*sr);
  }

  if (sr >= m_NLevels)	//oops, we are out
    sr = m_NLevels - 1;
  else
  {
    while (m_PLevels[sr] == NULL) {
      sr++;	//find the first coarser
    }
  }

  return sr;
}

//Called by Update to fill some internal structures
/*virtual*/ void mafVolumeLargeReader::ExecuteInformation() throw(...)
{
	if (m_NLevels == 0)
		CreateLODs();	//creates level of details

	//if VOI is not initialized, set it to the whole extent
	if (!m_VOI_Initialized) 
	{
		int nDims[3];	//m_pLevels[1] keeps the highest resolution data
		m_PLevels[1]->GetDataDimensions(nDims);
		for (int i = 0; i < 3; i++)
		{
			m_VOI[2*i] = 0;
			m_VOI[2*i + 1] = nDims[i] - 1;
		}
	}

	//compute the version, we need to take
  int sr = ComputeSampleRate(m_VOI, m_MemoryLimit);

	//sr denotes now the best level  
  if (m_NCurrentLevel != sr && m_NCurrentLevel != 0)
  {
    m_PLevels[m_NCurrentLevel]->SetEmptyVOI();
    m_PLevels[m_NCurrentLevel]->Update();
  }

	m_PLevels[sr]->SetListener(this->GetListener());
	m_PLevels[sr]->SetVOI(m_VOI);  
	m_NCurrentLevel = sr;
}

//processes data
/*virtual*/ void mafVolumeLargeReader::ExecuteData() throw(...)
{	
	m_PLevels[m_NCurrentLevel]->Update();

	//copy data to output port
	vtkImageData* output = GetOutputDataSet();
	vtkImageData* input = m_PLevels[m_NCurrentLevel]->GetOutputDataSet();

	output->ShallowCopy(input);
	output->SetUpdateExtentToWholeExtent();

  if (this->IsRectilinearGrid())
  {
    vtkRectilinearGrid* outputRLG = GetOutputRLGDataSet();
    vtkRectilinearGrid* inputRLG = m_PLevels[m_NCurrentLevel]->GetOutputRLGDataSet();

    outputRLG->ShallowCopy(inputRLG);
    outputRLG->SetUpdateExtentToWholeExtent();
  }

	//get modified VOI for the data (it might change)
	m_PLevels[m_NCurrentLevel]->GetVOI(m_VOI);
}

//This method updates the output (i.e., it performs the retrieval)	
//returns false if an error occurs
/*virtual*/ bool mafVolumeLargeReader::Update()
{
	if (this->GetMTime() <= m_LastUpdateTime) {
		return true; //no change
	}

#ifdef _PROFILE_LARGEDATA_
  LARGE_INTEGER liBegin;
  ::QueryPerformanceCounter(&liBegin);
#endif //_PROFILE_LARGEDATA_

	try
	{
		ExecuteInformation();
		ExecuteData();

		if (m_DataSet->GetScalarPointer() == NULL ||
      (this->IsRectilinearGrid() && m_DataSetRLG->GetPointData()
        ->GetScalars()->GetVoidPointer(0) == NULL))
			throw std::bad_alloc(_("Out of memory."));
	}
	catch (std::exception& e)
	{
		//time to display the message
		wxMessageBox(e.what(), _("Error: Loading failed"), 
			wxOK | wxICON_EXCLAMATION);

		return false;
	}

#ifdef _PROFILE_LARGEDATA_
  LARGE_INTEGER liEnd;
  ::QueryPerformanceCounter(&liEnd);   

  LARGE_INTEGER liFreq;
  ::QueryPerformanceFrequency(&liFreq);

  int VOI[6];
  this->GetVOI(VOI);     

  FILE* fLog = fopen("mafVolumeLargeReader.log", "at");
  fprintf(fLog, "%dx%dx%d (%d-%dx%d-%dx%d-%d = %.2f MB) retrieved in %.2f s - SR = %d, ML = %d MB.\n",
    VOI[1] - VOI[0] + 1, VOI[3] - VOI[2] + 1, VOI[5] - VOI[4] + 1, 
    VOI[0], VOI[1], VOI[2], VOI[3], VOI[4], VOI[5],
    this->GetOutputDataSet()->GetScalarSize()*
    (((VOI[1] - VOI[0] + 1)*(VOI[3] - VOI[2] + 1)/1024.0)*(VOI[5] - VOI[4] + 1)) / 1024.0,    
    ((double)(liEnd.QuadPart - liBegin.QuadPart)) / liFreq.QuadPart,
    this->GetSampleRate(), this->GetMemoryLimit() / 1024
    );
  fclose(fLog);          
#endif //_PROFILE_LARGEDATA_


	m_LastUpdateTime.Modified();
	return true;
}

//gets the total number of bytes in level files
//NB: this routine must be called after Update
vtkIdType64 mafVolumeLargeReader::GetLevelFilesSize()
{
	vtkIdType64 nRet = 0;
	for (int i = 1; i < m_NLevels; i++)
	{
		if (m_PLevels[i] != NULL)
		{
			nRet += vtkMAFFile::GetFileSize(m_PLevels[i]->GetFileName());
		}
	}

	return nRet;
}