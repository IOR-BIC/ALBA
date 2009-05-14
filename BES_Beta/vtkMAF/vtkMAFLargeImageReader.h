/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageReader.h,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkMAFLargeImageReader_h
#define __vtkMAFLargeImageReader_h

#pragma warning(disable: 4068)	//VS2003 does not support regions

#include "vtkMAFLargeImageSource.h"

class VTK_IO_EXPORT vtkMAFLargeImageReader : public vtkMAFLargeImageSource
{
protected:
	//Sample rate (in x,y and z) denotes the quality of the snapshot on output
	//1:1:1 is the highest quality
	int SampleRate[3];

	//if it is true (default), sample rate is automatically calculated from
	//the memory limit (see MemoryLimit)
	bool AutoSampleRate;

	//the memory limit in kilobytes for the snapshot of data
	unsigned long MemoryLimit;

	//Volume Of Interest
	int DataVOI[6];

#pragma region vtkImageReader2 stuff
  char* InternalFileName;           //<this is the name of the file from which we will read
  char* InternalFilePattern;        //<this is combined FilePattern and FilePrefix
  char *FileName;
  char *FilePrefix;
  char *FilePattern;  
	int NumberOfScalarComponents;
	int FileLowerLeft;

	vtkIdType64 DataIncrements[4];
	int DataExtent[6];
	int SwapBytes;

	int FileDimensionality;
	unsigned long HeaderSize;
	int DataScalarType;
	unsigned long ManualHeaderSize;   //<zero, if the header size should be determined automatically

	double DataSpacing[3];
	double DataOrigin[3];	

  int FileNameSliceOffset;
  int FileNameSliceSpacing;
#pragma endregion //vtkImageReader2 stuff
	
	// Mask each pixel with this DataMask
	unsigned short DataMask;
public:
	//////////////////////////////////////////////////////////////////////////
	//SETTERS / GETTERS

	// Description:
	// Set the sampling rate in the i, j, and k directions. If the rate is >
	// 1, then the resulting VOI will be subsampled representation of the
	// input.  For example, if the SampleRate=(2,2,2), every other point will
	// be selected, resulting in a volume 1/8th the original size.
	vtkSetVector3Macro(SampleRate, int);
	vtkGetVectorMacro(SampleRate, int, 3);

	//Get/Set automatic calculation of sample rate from the memory limit
	vtkSetMacro(AutoSampleRate, bool);
	vtkGetMacro(AutoSampleRate, bool);
	vtkBooleanMacro(AutoSampleRate, bool);

	//Get/Set the memory limit in kilobytes for the snapshot of data
	vtkSetMacro(MemoryLimit, unsigned long);
	vtkGetMacro(MemoryLimit, unsigned long);

	// Description:
	// Set/get the data VOI. You can limit the reader to only
	// read a subset of the data. 
	vtkSetVector6Macro(DataVOI,int);
	vtkGetVector6Macro(DataVOI,int);

#pragma region vtkImageReader2 stuff
	// Description:
	// Specify file name for the image file. You should specify either
	// a FileName or a FilePrefix. Use FilePrefix if the data is stored 
	// in multiple files.
	virtual void SetFileName(const char *);
	vtkGetStringMacro(FileName);

  // Description:
  // Specify file prefix for the image file(s).You should specify either
  // a FileName or FilePrefix. Use FilePrefix if the data is stored
  // in multiple files.
  virtual void SetFilePrefix(const char *);
  vtkGetStringMacro(FilePrefix);

  // Description:
  // The sprintf format used to build filename from FilePrefix and number.
  virtual void SetFilePattern(const char *);
  vtkGetStringMacro(FilePattern);

	// Description:
	// Set the data type of pixels in the file.  
	// If you want the output scalar type to have a different value, set it
	// after this method is called.
	virtual void SetDataScalarType(int type);
	virtual void SetDataScalarTypeToFloat(){this->SetDataScalarType(VTK_FLOAT);}
	virtual void SetDataScalarTypeToDouble(){this->SetDataScalarType(VTK_DOUBLE);}
	virtual void SetDataScalarTypeToInt(){this->SetDataScalarType(VTK_INT);}
	virtual void SetDataScalarTypeToShort(){this->SetDataScalarType(VTK_SHORT);}
	virtual void SetDataScalarTypeToUnsignedShort()
	{this->SetDataScalarType(VTK_UNSIGNED_SHORT);}
	virtual void SetDataScalarTypeToUnsignedChar()
	{this->SetDataScalarType(VTK_UNSIGNED_CHAR);}

	// Description:
	// Get the file format.  Pixels are this type in the file.
	vtkGetMacro(DataScalarType, int);

	// Description:
	// Set/Get the number of scalar components
	vtkSetMacro(NumberOfScalarComponents,int);
	vtkGetMacro(NumberOfScalarComponents,int);

	// Description:
	// Get/Set the extent of the data on disk.  
	vtkSetVector6Macro(DataExtent,int);
	vtkGetVector6Macro(DataExtent,int);

	// Description:
	// The number of dimensions stored in a file. This defaults to two.
	vtkSetMacro(FileDimensionality, int);
	int GetFileDimensionality() {
		return this->FileDimensionality;
	}

	// Description:
	// Set/Get the spacing of the data in the file.
	vtkSetVector3Macro(DataSpacing,double);
	vtkGetVector3Macro(DataSpacing,double);

	// Description:
	// Set/Get the origin of the data (location of first pixel in the file).
	vtkSetVector3Macro(DataOrigin,double);
	vtkGetVector3Macro(DataOrigin,double);

	// Description:
	// Get the size of the header computed by this object.
	unsigned long GetHeaderSize();

	// Description:
	// If there is a tail on the file, you want to explicitly set the
	// header size.
	virtual void SetHeaderSize(unsigned long size);

	// Description:
	// These methods should be used instead of the SwapBytes methods.
	// They indicate the byte ordering of the file you are trying
	// to read in. These methods will then either swap or not swap
	// the bytes depending on the byte ordering of the machine it is
	// being run on. For example, reading in a BigEndian file on a
	// BigEndian machine will result in no swapping. Trying to read
	// the same file on a LittleEndian machine will result in swapping.
	// As a quick note most UNIX machines are BigEndian while PC's
	// and VAX tend to be LittleEndian. So if the file you are reading
	// in was generated on a VAX or PC, SetDataByteOrderToLittleEndian 
	// otherwise SetDataByteOrderToBigEndian. 
	virtual void SetDataByteOrderToBigEndian();
	virtual void SetDataByteOrderToLittleEndian();
	virtual int GetDataByteOrder();
	virtual void SetDataByteOrder(int);
	virtual const char *GetDataByteOrderAsString();

  // Description:
  // When reading files which start at an unusual index, this can be added
  // to the slice number when generating the file name (default = 0)
  vtkSetMacro(FileNameSliceOffset,int);
  vtkGetMacro(FileNameSliceOffset,int);

  // Description:
  // When reading files which have regular, but non contiguous slices
  // (eg filename.1,filename.3,filename.5)
  // a spacing can be specified to skip missing files (default = 1)
  vtkSetMacro(FileNameSliceSpacing,int);
  vtkGetMacro(FileNameSliceSpacing,int);

	// Description:
	// Set/Get the byte swapping to explicitly swap the bytes of a file.
	vtkSetMacro(SwapBytes,int);
	virtual int GetSwapBytes() {return this->SwapBytes;}
	vtkBooleanMacro(SwapBytes,int);

	// Description:
	// Set/Get whether the data comes from the file starting in the lower left
	// corner or upper left corner.
	vtkBooleanMacro(FileLowerLeft, int);
	vtkGetMacro(FileLowerLeft, int);
	vtkSetMacro(FileLowerLeft, int);
	
	//Get/sets the data mask used to mask every byte read
	// Set/Get the Data mask.
	vtkGetMacro(DataMask, unsigned short);
	vtkSetMacro(DataMask, unsigned short);	

  // Description:
  // Set/Get the internal file name
  virtual void ComputeInternalFileName(int slice);
  vtkGetStringMacro(InternalFileName);

  // Description:
  // Set/Get the internal file pattern
  virtual void ComputeInternalFilePattern();
  vtkGetStringMacro(InternalFilePattern);  
#pragma endregion


public:
	//construction and RTTI information
	static vtkMAFLargeImageReader *New();
	vtkTypeRevisionMacro(vtkMAFLargeImageReader,vtkMAFLargeImageSource);
	void PrintSelf(ostream& os, vtkIndent indent);

protected:
	vtkMAFLargeImageReader();
	~vtkMAFLargeImageReader();

	// By default, UpdateInformation calls this method to copy information
	// unmodified from the input to the output.
	virtual void ExecuteInformation();

	// Description:
	// This method is the one that should be used by subclasses, right now the 
	// default implementation is to call the backwards compatibility method Execute
	virtual void ExecuteData(vtkDataObject *data);

	//These methods are used to transform extent (used by ExecuteData) 
	void ComputeTransformedExtent(int inExtent[6], int outExtent[6]);
	void ComputeInverseTransformedExtent(int inExtent[6], int outExtent[6]);

	//Computes the increments in data
	virtual void ComputeDataIncrements();

	//Initializes data providers for the given image data set
	virtual void InitializeDataProviders(vtkMAFLargeImageData* ds);
	
	//This method should fill the data providers in ds by the
	//currently selected extent
	virtual void PopulateDataProviders(vtkMAFLargeImageData* ds);

private:
	vtkMAFLargeImageReader(const vtkMAFLargeImageReader&);  // Not implemented.
	void operator=(const vtkMAFLargeImageReader&);  // Not implemented.
};

#endif
