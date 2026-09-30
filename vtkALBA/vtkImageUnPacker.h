/*=========================================================================

  Program:   Visualization Toolkit Extension
  Module:    vtkImageUnPacker.h
  Language:  C++
  Date:      03/1999
  Version:   
  Credits:    This class has been developed by Marco Petrone (m.petrone@cineca.it,petrus@ieee.org)


=========================================================================*/
// .NAME vtkImageUnPacker - Superclass for image unpackers.
// .SECTION Description
// vtkImageUnPacker provides methods needed to unpack images. Images can
// be read from a memory buffer or from a file
// .SECTION See Also
// 

#ifndef __vtkImageUnPacker_h
#define __vtkImageUnPacker_h

#include "vtkImageAlgorithm.h"
#include "vtkPackedImage.h"

#include "albaConfigure.h"

#ifndef VTK_IMG_PACK_NONE
  #define VTK_IMG_PACK_NONE 0
#endif

class ALBA_EXPORT vtkImageUnPacker : public vtkImageAlgorithm
{
public:
  static vtkImageUnPacker *New();
  vtkTypeMacro(vtkImageUnPacker, vtkImageAlgorithm);

  void PrintSelf(ostream& os, vtkIndent indent);   

  /**
  This method returns the largest data that can be generated.*/
  int RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
  
  /**
  Set/Get the input object containing the packed image.*/
  vtkSetObjectMacro(Input,vtkPackedImage);
  vtkGetObjectMacro(Input,vtkPackedImage);

  /**
  Set the input and force the updating of the output. Necessary if the input 
  is always the same vtkPackedImage.*/
  void ForceUnPack(vtkPackedImage *input) {SetInput(input);this->Modified();};
  void ForceUnPack() {this->Modified();};

  /**
  Enable/Disable the unpacking from a file. Use SetFileName() to
  specify the name of the file to pack into.*/
  vtkBooleanMacro(UnPackFromFile,int);
  vtkSetMacro(UnPackFromFile,int);
  vtkGetMacro(UnPackFromFile,int);

  /**
  Set/Get the name of the file to store the image.*/
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
protected:
  vtkImageUnPacker();
  ~vtkImageUnPacker();

  vtkPackedImage *Input;

  int UnPackFromFile;
  char *FileName;

  // Image Information
  int DataExtent[6];
  int DataScalarType;
  int NumberOfScalarComponents;

  /**
  Get/Set the extent of the data on disk.  */
  vtkSetVector6Macro(DataExtent,int);
  vtkGetVector6Macro(DataExtent,int);

  /**
  Get the file format.  Pixels are this type in the file.*/
  vtkGetMacro(DataScalarType, int);
  vtkSetMacro(DataScalarType, int);

  /**
  Set/Get the number of scalar components*/
  vtkSetMacro(NumberOfScalarComponents,int);
  vtkGetMacro(NumberOfScalarComponents,int);
  
  /**
  Read image information contained in the header and neccessary for 
  allocating the output image cache.*/
  virtual int ReadImageInformation(vtkPackedImage *packed) {return 0;};

  /**
  UnPacks the image into the output buffer. */
  virtual int VtkImageUnPackerUpdate(vtkPackedImage *packed, vtkImageData *data) {return 0;};
 
};

#endif
