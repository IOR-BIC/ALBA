/*=========================================================================

 Program: MAF2
 Module: vtkDicomUnPacker
 Authors: Marco Petrone m.petrone@cineca.it, Paolo Quadrani p.quadrani@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkDicomUnPacker - This class allows to unpack a JPEG image
// .SECTION Description
// vtkDicomUnPacker is a wrapper of the LibJPEG Library. At present it takes its  
// input from a file and produces as output vtkImageData. Unpacking from a memory (from
// a vtkPackedImage is not yet supported
// .SECTION See Also
// vtkLibJPEGPacker vtkImagePacker vtkImageUnPacker vtkPackedImage vtkPackedImageWriter 
// vtkPackedImageReader vtkDataStreamer vtkDataUnStreamer
#ifndef __vtkDicomUnPacker_h
#define __vtkDicomUnPacker_h

#include "vtkImageUnPacker.h"
#include "vtkPackedImage.h"

#include "mafConfigure.h"

#include <string>

#ifndef VTK_IMG_PACK_DICOM
#define VTK_IMG_PACK_DICOM 2
#endif

/*************************************************
            DEFINE
 ************************************************/
#define multeplicity 10
#define SIZE_TAG    600

/************************************************
            TYPEDEF
*************************************************/
typedef unsigned long     uint32;
typedef unsigned short    uint16;
typedef unsigned char     uint8;

typedef long int          int32;

class MAF_EXPORT vtkDicomUnPacker : public vtkImageUnPacker
{
public:
  /************************************************
            TYPEDEF
  *************************************************/
  
  typedef enum { yes, no } present;
  typedef enum { string, num }tipo;

  typedef struct {
	  tipo  type;
	  char   mult;           
	  double num[multeplicity];
	  char stringa[multeplicity][300];
  } VALUE;

  typedef struct  {
	present  intoDictionary; 
	int  Group;
	int  Element;
	char Version[10];
	char Val_Repr[20];
	char Val_Mult[10];
	char Keyword[100];
	char Name[110];                              

  } DICOM;

  typedef struct {
    int  Group;
    int  Element;
    std::string Version;
    std::string Val_Repr;
    std::string Val_Mult;
    std::string Keyword;
    std::string Name;
  } DICOM_DEFAULT;

public:
	vtkTypeRevisionMacro(vtkDicomUnPacker,vtkImageUnPacker);
	void PrintSelf(ostream& os, vtkIndent indent);

	/**	Constructor.*/
  static vtkDicomUnPacker *New();

	/**	Set the dictionary file name*/
	void SetDictionaryFileName(const char *filename);
	const char * GetDictionaryFileName() {return DictionaryFileName;};

  /**	use default dictionary flag*/
  vtkBooleanMacro(UseDefaultDictionary,int);
  vtkSetMacro(UseDefaultDictionary,int);
  vtkGetMacro(UseDefaultDictionary,int);

  /**	Get the patient information*/
	vtkGetStringMacro(PatientName);
	vtkGetStringMacro(PatientBirthDate);
	vtkGetStringMacro(PatientSex);
	char *GetCTMode();
	vtkGetStringMacro(PatientID);
	vtkGetStringMacro(Modality);
	vtkGetStringMacro(StudyUID);
	vtkGetStringMacro(StudyDate);
	vtkGetStringMacro(Study);

	/**	Get the image information*/
//	vtkGetMacro(SliceLocation,double);
	void GetSliceLocation(double pos[3]);
  char *GetImageType(int id_caracrteristic = 0);
  
	/**	flip the image readed*/
	vtkBooleanMacro(FlipImage,int);
	vtkSetMacro(FlipImage,int);
	vtkGetMacro(FlipImage,int);

	//modified by Matteo 30-6-2006 (begin)
  /** Get the image number*/
  vtkGetMacro(InstanceNumber, int);

  /** Get the cardiac number of images ie the number of time stamps for a time varying dicom*/
  vtkGetMacro(CardiacNumberOfImages, int);
  
  /** Get the time stamp of the dicom slice*/
  vtkGetMacro(TriggerTime, double);
  //modified by Stefano 30-6-2006 (end)

	/**	return the status of the reader: 0 if ok, -1 if there is an error*/
	int GetStatus() {return Status;};
	
	/**	generate txt file with the dump of the header*/
	vtkBooleanMacro(DebugFlag,int);
	vtkSetMacro(DebugFlag,int);
	vtkGetMacro(DebugFlag,int);
  const DICOM *GetTag(unsigned int n) {if (n<TAGNumbers) return &RESULT[n]; else return NULL;};
  const VALUE *GetTagElement(unsigned int n) {if (n<TAGNumbers) return &VALUES[n]; else return NULL;};
  const DICOM *GetFromDictionary(int pos) {return (pos<DICT_line?&this->DICT[pos]:NULL);};
  int FindInDictionary(int group, int element) {return this->Find(group,element,DICT,DICT_line);};

  int GetNumberOfTags() {return this->TAGNumbers;};

  DICOM *GetDictionary() {return this->DICT;}
  int GetDictionarySize() {return this->DICT_line;}

  /**	Modality single file*/
  vtkBooleanMacro(ModeSingleFile,bool);
  vtkSetMacro(ModeSingleFile,bool);
  vtkGetMacro(ModeSingleFile,bool);

  /** Return the number of frame */
  vtkGetMacro(NumberOfFrames,int);

  vtkSetMacro(NumOfFrameToGet,int);

protected:
	vtkDicomUnPacker();
	~vtkDicomUnPacker();

  vtkDicomUnPacker(const vtkDicomUnPacker&);
  void operator=(const vtkDicomUnPacker&);

  bool ModeSingleFile;//<<Set modality if in the file is a single DICOM time variant
  int NumberOfFrames;
  int NumOfFrameToGet;

	uint32 TAGNumbers;
	DICOM  RESULT[SIZE_TAG];
	VALUE  VALUES[SIZE_TAG];
	uint8  *IMAGE;
	uint32  ImageSize;  
	FILE *Text;

  DICOM  DICT[1500]; /* dictionary */ 
  long   DICT_line; // number of lines in the dictionary

  bool DictionaryRead;
  int UseDefaultDictionary;
  int DebugFlag;
	int	FlipImage;
	int Flag;
	int Status;
	const char * DictionaryFileName;
	char PatientName[256];
	char PatientBirthDate[256];
	char CTMode[2][15];
	char PatientSex[3];
	char Modality[3];
	char PatientID[11];
	char StudyUID[256];
	char Study[80];
	char StudyDate[15];
  char ImageType[3][256];

	//modified by Matteo 30-6-2006
  // Image number
  int InstanceNumber;

  // Number of time stamp for the image
  int CardiacNumberOfImages;

  // Image time stamp
  double TriggerTime;

//	double SliceLocation;
	
	/**
	Read image information contained in the header and neccessary for 
	allocating the output image cache.*/
	int ReadImageInformation(vtkPackedImage *packed);

	/**
	UnPacks the image into the output buffer. */
	int VtkImageUnPackerUpdate(vtkPackedImage *packed, vtkImageData *data);
//	int read_dicom_string_image(uint16 *IMAGE, double slope_value, double intercept_value);
	int ReadDicomHeader(DICOM RESULT[], VALUE VALUES[], uint32 *size_image, uint32 *result_line);

protected:
/****************************************************************************
                            F U N C T I O N S
****************************************************************************/
int LoadDictionary(DICOM DICT[]);
int LoadDictionaryFromFile(DICOM DICT[]);

/* FROM (Group, Element)  returns the m_Position of rispective pattern into Dictionary */ 
long Find (long Group, long Element , DICOM DICT[], long n_line);

//uint16 read16 (FILE* fp, char little_endian) ;
//uint32 read32 (FILE* fp, char little_endian);
int BitsAllocated;
int BitsStored;
int HighBit;
int PixelRepresentation;
int SmallestImagePixelValue;
int LargestImagePixelValue;
int DimX;
int DimY;
char PhotometricInterpretation[11];
double Intercept;
double Slope;
double Spacing[2];
double Position[3];
double Orientation[3];

void  ReadDicomHeader(char * input, DICOM RESULT[], VALUE VALUES[], uint32 *size_image, uint32 *result_line);
void  ParserMultepl(tipo str , FILE * fp, long Length, VALUE * VALUES);
};
#endif
