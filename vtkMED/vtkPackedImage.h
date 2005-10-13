/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPackedImage.h
  Language:  C++
  Date:      12/1999
  Version:   .1
  Thanks:    This class has been developed by Marco Petrone (m.petrone@cineca.it,petrus@ieee.org)


=========================================================================*/
// .NAME vtkPackedImage - A class to contain data of packed images
// .SECTION Description
// vtkPackedImage is a class derived from vtkUnsignedCharArray, thought to contain data of 
// packed images. The only extra field in this class refers to the type of packaging.
// .SECTION See Also
// vtkImagePacker vtkImageUnPacker

#ifndef __vtkPackedImage_h
#define __vtkPackedImage_h

#include "vtkUnsignedCharArray.h"
#include "vtkMAFConfigure.h"


#define VTK_IMG_PACK_NONE 0
  
class VTK_vtkMAF_EXPORT vtkPackedImage : public vtkUnsignedCharArray
{
public:
  static vtkPackedImage *New();
  vtkTypeRevisionMacro(vtkPackedImage, vtkUnsignedCharArray);

  void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Set/Get the type of packaging used for this image.*/
  vtkSetMacro(PackType,int);
  vtkGetMacro(PackType,int);

  /**
  Set/Get the real size of the packed image.*/
  void SetImageSize(unsigned long imgsize);
  int GetImageSize() {return this->GetSize();};

  /**
  Import an image: the specified memory is copied or referenced depending 
  on the copy parameter (0 or 1)*/
  void ImportImage(unsigned char *ptr,unsigned long size,int copy);

  /**
  Return the pointer to the memory allocated for the image*/
  unsigned char *GetImagePointer() {return (unsigned char *)GetPointer(0);};
 
protected:
  vtkPackedImage();
  ~vtkPackedImage();

  int PackType;
};

#endif
