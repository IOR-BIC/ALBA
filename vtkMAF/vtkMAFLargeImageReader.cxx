/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageReader.cxx,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#pragma warning(disable: 4996)	//depricated

#include "vtkMAFLargeImageReader.h"
#include "vtkMAFLargeImageData.h"
#include "vtkMAFFileDataProvider.h"
#include "vtkMAFMultiFileDataProvider.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFLargeImageReader, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFLargeImageReader);

#include "mafMemDbg.h"

//ctor
vtkMAFLargeImageReader::vtkMAFLargeImageReader()
{
	this->SampleRate[0] = this->SampleRate[1] = this->SampleRate[2] = 1;
	this->AutoSampleRate = true;
	this->MemoryLimit = 1700;		//some magic constant given by Anupam

	this->DataScalarType = VTK_SHORT;
	this->NumberOfScalarComponents = 1;

	this->DataOrigin[0] = this->DataOrigin[1] = this->DataOrigin[2] = 0.0;

	this->DataSpacing[0] = this->DataSpacing[1] = this->DataSpacing[2] = 1.0;

	this->DataExtent[0] = this->DataExtent[2] = this->DataExtent[4] = 0;
	this->DataExtent[1] = this->DataExtent[3] = this->DataExtent[5] = 0;

	this->DataIncrements[0] = this->DataIncrements[1] = 
		this->DataIncrements[2] = this->DataIncrements[3] = 1;

	this->FileName = NULL;
  this->InternalFileName = NULL;
  this->InternalFilePattern = NULL;
  this->FilePrefix = NULL;
  this->FilePattern = new char[strlen("%s.%d") + 1];
  strcpy (this->FilePattern, "%s.%d");  

  this->FileNameSliceOffset = 0;
  this->FileNameSliceSpacing = 1;

	this->HeaderSize = 0;
	this->ManualHeaderSize = 0;

	// Left over from short reader
	this->SwapBytes = 0;
	this->FileLowerLeft = 0;
	this->FileDimensionality = 2;

	for (int idx = 0; idx < 3; ++idx)
	{
		this->DataVOI[idx*2] = this->DataVOI[idx*2 + 1] = 0;
	}

	this->DataMask = 0xffff;
}

vtkMAFLargeImageReader::~vtkMAFLargeImageReader()
{
	if (this->FileName)
	{
		delete [] this->FileName;
		this->FileName = NULL;
	}

  if (this->FilePrefix)
  {
    delete [] this->FilePrefix;
    this->FilePrefix = NULL;
  }

  if (this->FilePattern)
  {
    delete [] this->FilePattern;
    this->FilePattern = NULL;
  }

  if (this->InternalFileName)
  {
    delete [] this->InternalFileName;
    this->InternalFileName = NULL;
  }

  if (this->InternalFilePattern)
  {
    delete [] this->InternalFilePattern;
    this->InternalFilePattern = NULL;
  }
}

//Initializes data providers for the given image data set
/*virtual*/ void vtkMAFLargeImageReader::InitializeDataProviders(vtkMAFLargeImageData* ds)
{
  vtkMAFLargeDataProvider* pp = ds->GetPointDataProvider();
  if (this->FileName != NULL)
  {
    //simple file => data provider must be of vtkMAFFileDataProvider type
    vtkMAFFileDataProvider* fp = vtkMAFFileDataProvider::SafeDownCast(pp);
    if (fp == NULL)
    {
      //no provider at all, create the default one
      fp = vtkMAFFileDataProvider::New();
      ds->SetPointDataProvider(pp = fp);
      pp->Delete();	//we no longer need it
    }

    if (fp->GetFileName() == NULL || 
      strcmp(fp->GetFileName(), this->GetFileName()) != 0)
    {
      //open a new file
      if (!fp->OpenFile(this->GetFileName()))
      {
        vtkErrorMacro(<< "Cannot open file: " << this->GetFileName());
      }
    }
  }
  else
  {
    //multiple files => data provider must be of vtkMAFMultiFileDataProvider type
    vtkMAFMultiFileDataProvider* fp = vtkMAFMultiFileDataProvider::SafeDownCast(pp);
    if (fp == NULL)
    {
      //no provider at all, create the default one
      fp = vtkMAFMultiFileDataProvider::New();
      ds->SetPointDataProvider(pp = fp);
      pp->Delete();	//we no longer need it
    }

    fp->SetHeaderSize2(this->GetHeaderSize());
    if (!fp->OpenMultiFile(this->GetInternalFilePattern(), 
      this->DataExtent[5] - this->DataExtent[4] + 1,
      this->GetFileNameSliceOffset(), this->GetFileNameSliceSpacing()))
    {    
      vtkErrorMacro(<< "Cannot open multi-file: " << this->GetInternalFilePattern());
    }    
  }

	pp->SetHeaderSize(this->GetHeaderSize());
	pp->SetSwapBytes(this->GetSwapBytes() != 0);
}

//This method should fill the data providers in ds by the
//currently selected extent
/*virtual*/ void vtkMAFLargeImageReader::PopulateDataProviders(vtkMAFLargeImageData* ds)
{
	//nothing to do for vtkMAFFileDataProvider, etc.
}

// By default, UpdateInformation calls this method to copy information
// unmodified from the input to the output.
/*virtual*/ void vtkMAFLargeImageReader::ExecuteInformation()
{	
	//default computation of output extent
	vtkMAFLargeImageData *output = this->GetOutput();	

	//first, let us initialize data providers
	this->InitializeDataProviders(output);
	
	//set the dimensions of underlaying file
	//NB: Extent will be set to UpdateExtent by the caller
	output->SetWholeExtent(this->DataExtent);
	output->SetUpdateExtentToWholeExtent();

	// set the whole extent (area of interest in the image data) to our VOI
	if ((this->DataVOI[0] | this->DataVOI[1] | this->DataVOI[2] |
		this->DataVOI[3] | this->DataVOI[4] | this->DataVOI[5]) != 0)	
		output->SetVOI(this->DataVOI);	//if it was set
	
	// set the spacing
	output->SetSpacing(this->DataSpacing);

	// set the origin.
	output->SetOrigin(this->DataOrigin);

	output->SetScalarType(this->DataScalarType);
	output->SetNumberOfScalarComponents(this->NumberOfScalarComponents);
	output->SetDataLowerLeft(this->FileLowerLeft != 0);
	output->SetDataMask(this->DataMask);	
	output->SetMemoryLimit(this->MemoryLimit);
}

//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkMAFLargeImageReader::ExecuteData(vtkDataObject *output)
{	
	if (this->FileName == NULL && this->InternalFilePattern == NULL)
	{
		vtkErrorMacro("A valid FileName or FilePattern must be specified.");
		return;
	}

	//This call initializes the output: it calls ExecuteInformation()
	//to copy Extents, Spacing, etc. and calls vtkMAFLargeImageData::AllocateScalars   
	//that updates the descriptor of point scalar data with the provided 
	//information (if the descriptor does not exist, it is created)
	vtkMAFLargeImageData *data = this->AllocateOutputData(output);

	int* ext = data->GetExtent();
	vtkDebugMacro("Reading extent: " << ext[0] << ", " << ext[1] << ", " 
		<< ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]);
	
	PopulateDataProviders(data);
}


#pragma region vtkImageReader stuff
//----------------------------------------------------------------------------
// This function sets the name of the file. 
void vtkMAFLargeImageReader::ComputeInternalFileName(int slice)
{
  // delete any old filename
  if (this->InternalFileName)
  {
    delete [] this->InternalFileName;
    this->InternalFileName = NULL;
  }

  if (!this->FileName && !this->InternalFilePattern)
  {
    vtkErrorMacro(<<"Either a FileName or FilePattern must be specified.");
    return;
  }

  // make sure we figure out a filename to open
  if (this->FileName != NULL)
  {
    this->InternalFileName = new char [strlen(this->FileName) + 1];    
    strcpy(this->InternalFileName, this->FileName);
  }
  else if (this->InternalFilePattern != NULL)
  {
    int slicenum = slice * this->FileNameSliceSpacing + this->FileNameSliceOffset;       
    int len = _scprintf(this->InternalFilePattern, slicenum) + 1;        
    this->InternalFileName = new char [len];  

#if defined(_MSC_VER) && _MSC_VER >= 1400
    sprintf_s(this->InternalFileName, len, 
#else
    sprintf(this->InternalFileName, 
#endif            
      this->InternalFilePattern, slicenum);    
  }
  else
  {
    //FilePattern is NULL
    delete [] this->InternalFileName;
    this->InternalFileName = NULL;
  }
}

//----------------------------------------------------------------------------
// This function sets the name of the file. 
void vtkMAFLargeImageReader::SetFileName(const char *name)
{
	if ( this->FileName && name && (!strcmp(this->FileName,name)))
	{
		return;
	}
	if (!name && !this->FileName)
	{
		return;
	}
	if (this->FileName)
	{
		delete [] this->FileName;
	}

	if (name)
	{
		this->FileName = new char[strlen(name) + 1];
		strcpy(this->FileName, name);
	}
	else
	{
		this->FileName = NULL;
	}

	this->Modified();
}

//----------------------------------------------------------------------------
// This function sets the prefix of the file name. "image" would be the
// name of a series: image.1, image.2 ...
void vtkMAFLargeImageReader::SetFilePrefix(const char *prefix)
{
  if ( this->FilePrefix && prefix && (!strcmp(this->FilePrefix,prefix)))
  {
    return;
  }
  if (!prefix && !this->FilePrefix)
  {
    return;
  }
  if (this->FilePrefix)
  {
    delete [] this->FilePrefix;
  }
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }  
  if (prefix == NULL)
    this->FilePrefix = NULL;
  else
  {
    this->FilePrefix = new char[strlen(prefix) + 1];
    strcpy(this->FilePrefix, prefix);
  }

  ComputeInternalFilePattern();  
  this->Modified();
}

//----------------------------------------------------------------------------
// This function sets the pattern of the file name which turn a prefix
// into a file name. "%s.%3d" would be the
// pattern of a series: image.001, image.002 ...
void vtkMAFLargeImageReader::SetFilePattern(const char *pattern)
{
  if ( this->FilePattern && pattern && 
    (!strcmp(this->FilePattern,pattern)))
  { //neither is NULL but they are the same
    return;
  }
  if (!pattern && !this->FilePattern)
  { //both are NULLs
    return;
  }
  if (this->FilePattern)
  {
    delete [] this->FilePattern;
  }
  if (this->FileName)
  {
    delete [] this->FileName;
    this->FileName = NULL;
  }

  if (pattern == NULL)
    this->FilePattern = NULL;
  else
  {
    this->FilePattern = new char[strlen(pattern) + 1];
    strcpy(this->FilePattern, pattern);
  }
    
  ComputeInternalFilePattern();
  this->Modified();
}

//------------------------------------------------------------------------
// Set/Get the internal file pattern
/*virtual*/ void vtkMAFLargeImageReader::ComputeInternalFilePattern()
//------------------------------------------------------------------------
{
  //For security reasons, analyze the given FilePattern;  
  //invalid pattern specified by the user may have dire effects
  bool bFilePatternHasString = false;
  int nPercents = 0;

  char *pSChrStart, *pSChrEnd;  
  char* pChr = this->FilePattern;
  while (*pChr != '\0')
  {         
    if (*pChr != '%') 
    {
      //ordinary character
      pChr++;
      continue;
    }    

    pChr++;  //advance to next
    if (*pChr == '%') 
    {
      //'%' character
      pChr++;
      continue;
    }

    if (nPercents == 2)
    {
      //we already have two %, the rest must be invalidate
      *pChr = '%'; pChr++;
      continue;
    }

    //search for strings (those are critical); the format is:
    //%[flags][width][.precision][{h,l,ll,I,I32,I64}]type
    //strings have type 's' or 'S' (MS specified)
    char* pChrPStart = pChr;      
    if (*pChr == '-' || *pChr == '+' || *pChr == '0' || 
      *pChr == ' ' || *pChr == '#')
      pChr++;  //skip flags

    while (*pChr >= '0' && *pChr <= '9') {
      pChr++;  //skip width digit
    }

    if (*pChr == '.')
    {
      pChr++;    //skip precision mark
      while (*pChr >= '0' && *pChr <= '9') {
        pChr++;  //skip precision digit
      }     
    }

    if (*pChr == 'h')
      pChr++;
    else if (*pChr == 'l')
    {
      pChr++;
      if (*pChr == 'l')
        pChr++;  //'ll'        
    }
    else if (*pChr == 'I')
    {
      pChr++;

      if ((*pChr == '3' && pChr[1] == '2') ||
        (*pChr == '6' && pChr[1] == '4'))
        pChr += 2;
    }

    if (*pChr == 's' || *pChr == 'S')
    {
      //string, we found it
      if (bFilePatternHasString)
        *pChrPStart = '%';    //we have already one %s there
      else
      {
        pSChrStart = pChrPStart - 1;    //start with %
        pSChrEnd = pChr + 1;            //one more (exclusive end)
        bFilePatternHasString = true;
      }        
    }

    pChr++;                   //advance type
    nPercents++;
  }

  if (this->InternalFilePattern != NULL)
  {
    delete[] this->InternalFilePattern;
  }

  if (!bFilePatternHasString)
  {
    //there is no %s in the pattern => prefix will be ignored
    //N.B. we might have detected more than one %, sprintf will generate
    //invalid string, however, no matter what will be in the stack, there will 
    //be no crash because parameters in the stack will be considered as value types 
    //(unlike %s parameters, which are reference types). => we are ready

#if defined(_MSC_VER) && _MSC_VER >= 1400
    this->InternalFilePattern = _strdup(this->FilePattern);
#else
    int nLen = (int)strlen(this->FilePattern);
    this->InternalFilePattern = new char[nLen + 1];
    strcpy(this->InternalFilePattern, this->FilePattern);
#endif
  }
  else
  {
    //we have there some %s, so, we will need to merge it with FilePrefix
    char chOld2 = *pSChrEnd;
    *pSChrEnd = '\0';

    int nLen2 = _scprintf(pSChrStart, 
      (this->FilePrefix != NULL ? this->FilePrefix : "")) + 1;

    char* tmp = new char[nLen2];
#if defined(_MSC_VER) && _MSC_VER >= 1400
    sprintf_s(tmp, nLen2, 
#else
    sprintf(tmp, 
#endif
      pSChrStart, (this->FilePrefix != NULL ? this->FilePrefix : ""));

    //tmp now contains formated FilePrefix, merge it
    int nLen1 = pSChrStart - this->FilePattern;
    int nLen3 = pChr - pSChrEnd;

    char chOld1 = *pSChrStart;
    *pSChrStart = '\0';

    this->InternalFilePattern = new char[nLen1 + nLen2 + nLen3];
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(this->InternalFilePattern, nLen1 + nLen2 + nLen3, this->FilePattern);
    strcpy_s(&this->InternalFilePattern[nLen1], nLen2 + nLen3, tmp);    
#else
    strcpy(this->InternalFilePattern, this->FilePattern);
    strcpy(&this->InternalFilePattern[nLen1], tmp);    
#endif

    //release memory
    delete[] tmp;

    //restore original characters
    *pSChrEnd = chOld2;
    *pSChrStart = chOld1;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(&this->InternalFilePattern[nLen1 + nLen2 - 1], nLen3 + 1, pSChrEnd);
#else
    strcpy(&this->InternalFilePattern[nLen1 + nLen2 - 1], pSChrEnd);
#endif
  }
}



void vtkMAFLargeImageReader::SetDataByteOrderToBigEndian()
{
#ifndef VTK_WORDS_BIGENDIAN
	this->SwapBytesOn();
#else
	this->SwapBytesOff();
#endif
}

void vtkMAFLargeImageReader::SetDataByteOrderToLittleEndian()
{
#ifdef VTK_WORDS_BIGENDIAN
	this->SwapBytesOn();
#else
	this->SwapBytesOff();
#endif
}

#include "vtkImageReader2.h"
void vtkMAFLargeImageReader::SetDataByteOrder(int byteOrder)
{
	if ( byteOrder == VTK_FILE_BYTE_ORDER_BIG_ENDIAN )
	{
		this->SetDataByteOrderToBigEndian();
	}
	else
	{
		this->SetDataByteOrderToLittleEndian();
	}
}

int vtkMAFLargeImageReader::GetDataByteOrder()
{
#ifdef VTK_WORDS_BIGENDIAN
	if ( this->SwapBytes )
	{
		return VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN;
	}
	else
	{
		return VTK_FILE_BYTE_ORDER_BIG_ENDIAN;
	}
#else
	if ( this->SwapBytes )
	{
		return VTK_FILE_BYTE_ORDER_BIG_ENDIAN;
	}
	else
	{
		return VTK_FILE_BYTE_ORDER_LITTLE_ENDIAN;
	}
#endif
}

const char *vtkMAFLargeImageReader::GetDataByteOrderAsString()
{
#ifdef VTK_WORDS_BIGENDIAN
	if ( this->SwapBytes )
	{
		return "LittleEndian";
	}
	else
	{
		return "BigEndian";
	}
#else
	if ( this->SwapBytes )
	{
		return "BigEndian";
	}
	else
	{
		return "LittleEndian";
	}
#endif
}


//----------------------------------------------------------------------------
void vtkMAFLargeImageReader::PrintSelf(ostream& os, vtkIndent indent)
{
	int idx;

	this->Superclass::PrintSelf(os,indent);

	// this->File, this->Colors need not be printed  
	os << indent << "FileName: " <<
		(this->FileName ? this->FileName : "(none)") << "\n";
  os << indent << "FilePrefix: " << 
    (this->FilePrefix ? this->FilePrefix : "(none)") << "\n";
  os << indent << "FilePattern: " << 
    (this->FilePattern ? this->FilePattern : "(none)") << "\n";

  os << indent << "FileNameSliceOffset: " 
    << this->FileNameSliceOffset << "\n";
  os << indent << "FileNameSliceSpacing: " 
    << this->FileNameSliceSpacing << "\n";

	os << indent << "DataScalarType: " 
		<< vtkImageScalarTypeNameMacro(this->DataScalarType) << "\n";
	os << indent << "NumberOfScalarComponents: " 
		<< this->NumberOfScalarComponents << "\n";

	os << indent << "File Dimensionality: " << this->FileDimensionality << "\n";

	os << indent << "File Lower Left: " << 
		(this->FileLowerLeft ? "On\n" : "Off\n");

	os << indent << "Swap Bytes: " << (this->SwapBytes ? "On\n" : "Off\n");

	os << indent << "DataIncrements: (" << this->DataIncrements[0];
	for (idx = 1; idx < 2; ++idx)
	{
		os << ", " << this->DataIncrements[idx];
	}
	os << ")\n";

	os << indent << "DataExtent: (" << this->DataExtent[0];
	for (idx = 1; idx < 6; ++idx)
	{
		os << ", " << this->DataExtent[idx];
	}
	os << ")\n";

	os << indent << "DataSpacing: (" << this->DataSpacing[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataSpacing[idx];
	}
	os << ")\n";

	os << indent << "DataOrigin: (" << this->DataOrigin[0];
	for (idx = 1; idx < 3; ++idx)
	{
		os << ", " << this->DataOrigin[idx];
	}
	os << ")\n";

	os << indent << "HeaderSize: " << this->HeaderSize << "\n";

  if ( this->InternalFileName )
  {
    os << indent << "Internal File Name: " << this->InternalFileName << "\n";
  }
  else
  {
    os << indent << "Internal File Name: (none)\n";
  }
}


//----------------------------------------------------------------------------
// Manual initialization.
void vtkMAFLargeImageReader::SetHeaderSize(unsigned long size)
{
	if (size != this->HeaderSize)
	{
		this->HeaderSize = size;
		this->Modified();
	}
	this->ManualHeaderSize = 1;
}


//----------------------------------------------------------------------------
// This function opens a file to determine the file size, and to
// automatically determine the header size.
void vtkMAFLargeImageReader::ComputeDataIncrements()
{
	int idx;
	vtkIdType64 fileDataLength;

	// Determine the expected length of the data ...
	switch (this->DataScalarType)
	{
	case VTK_FLOAT:
		fileDataLength = sizeof(float);
		break;
	case VTK_DOUBLE:
		fileDataLength = sizeof(double);
		break;
	case VTK_INT:
		fileDataLength = sizeof(int);
		break;
	case VTK_UNSIGNED_INT:
		fileDataLength = sizeof(unsigned int);
		break;
	case VTK_LONG:
		fileDataLength = sizeof(long);
		break;
	case VTK_UNSIGNED_LONG:
		fileDataLength = sizeof(unsigned long);
		break;
	case VTK_SHORT:
		fileDataLength = sizeof(short);
		break;
	case VTK_UNSIGNED_SHORT:
		fileDataLength = sizeof(unsigned short);
		break;
	case VTK_CHAR:
		fileDataLength = sizeof(char);
		break;
	case VTK_UNSIGNED_CHAR:
		fileDataLength = sizeof(unsigned char);
		break;
	default:
		vtkErrorMacro(<< "Unknown DataScalarType");
		return;
	}

	fileDataLength *= this->NumberOfScalarComponents;

	// compute the fileDataLength (in units of bytes)
	for (idx = 0; idx < 3; ++idx)
	{
		this->DataIncrements[idx] = fileDataLength;
		fileDataLength = fileDataLength *
			(this->DataExtent[idx*2+1] - this->DataExtent[idx*2] + 1);
	}

	this->DataIncrements[3] = fileDataLength;
}


#include <sys/stat.h>
unsigned long vtkMAFLargeImageReader::GetHeaderSize()
{
	if (!this->FileName && !this->FilePattern)
	{
		vtkErrorMacro(<<"Either a FileName or FilePattern must be specified.");
		return 0;
	}

	if (!this->ManualHeaderSize)
	{
		this->ComputeDataIncrements();

		// make sure we figure out a filename to open
		struct stat statbuf;
		stat(this->FileName, &statbuf);

		return (int)(statbuf.st_size -
			this->DataIncrements[this->GetFileDimensionality()]);
	}

	return this->HeaderSize;
}


//----------------------------------------------------------------------------
// Set the data type of pixels in the file.  
// If you want the output scalar type to have a different value, set it
// after this method is called.
void vtkMAFLargeImageReader::SetDataScalarType(int type)
{
	if (type == this->DataScalarType)
	{
		return;
	}

	this->Modified();
	this->DataScalarType = type;
	// Set the default output scalar type
	this->GetOutput()->SetScalarType(this->DataScalarType);
}
#pragma endregion

#pragma region vtkImageReader stuff
void vtkMAFLargeImageReader::ComputeTransformedExtent(int inExtent[6],
											  int outExtent[6])
{
	int idx;
	int dataExtent[6];

	memcpy (outExtent, inExtent, 6 * sizeof (int));
	memcpy (dataExtent, this->DataExtent, 6 * sizeof(int));

	for (idx = 0; idx < 6; idx += 2)
	{
		if (outExtent[idx] > outExtent[idx+1]) 
		{
			int temp = outExtent[idx];
			outExtent[idx] = outExtent[idx+1];
			outExtent[idx+1] = temp;
		}
		// do the slide to 000 origin by subtracting the minimum extent
		outExtent[idx] -= dataExtent[idx];
		outExtent[idx+1] -= dataExtent[idx];
	}

	vtkDebugMacro(<< "Transformed extent are:" 
		<< outExtent[0] << ", " << outExtent[1] << ", "
		<< outExtent[2] << ", " << outExtent[3] << ", "
		<< outExtent[4] << ", " << outExtent[5]);
}

void vtkMAFLargeImageReader::ComputeInverseTransformedExtent(int inExtent[6],
													 int outExtent[6])
{
	int idx;

	memcpy (outExtent, inExtent, 6 * sizeof (int));
	for (idx = 0; idx < 6; idx += 2)
	{
		// do the slide to 000 origin by subtracting the minimum extent
		outExtent[idx] += this->DataExtent[idx];
		outExtent[idx+1] += this->DataExtent[idx];
	}

	vtkDebugMacro(<< "Inverse Transformed extent are:" 
		<< outExtent[0] << ", " << outExtent[1] << ", "
		<< outExtent[2] << ", " << outExtent[3] << ", "
		<< outExtent[4] << ", " << outExtent[5]);
}

#pragma endregion