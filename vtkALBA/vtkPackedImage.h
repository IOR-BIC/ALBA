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
#include "albaConfigure.h"


#define VTK_IMG_PACK_NONE 0
/**
    class name: vtkPackedImage
    vtkPackedImage is a class derived from vtkUnsignedCharArray, thought to contain data of
    packed images. The only extra field in this class refers to the type of packaging.
*/
class ALBA_EXPORT vtkPackedImage : public vtkUnsignedCharArray
{
public:
  /** construct the object*/
  static vtkPackedImage *New();
  /** RTTI macro */
  vtkTypeMacro(vtkPackedImage, vtkUnsignedCharArray);

  /** print object information */
  void PrintSelf(ostream& os, vtkIndent indent);

  /** Set the type of packaging used for this image.*/
  vtkSetMacro(PackType,int);
  /** Get the type of packaging used for this image.*/
  vtkGetMacro(PackType,int);

  /** Set the real size of the packed image.*/
  void SetImageSize(unsigned long imgsize);
  /** Get the real size of the packed image.*/
  int GetImageSize() {return this->GetSize();};

  /**
  Import an image: the specified memory is copied or referenced depending 
  on the copy parameter (0 or 1)*/
  void ImportImage(unsigned char *ptr,unsigned long size,int copy);

  /**
  Return the pointer to the memory allocated for the image*/
  unsigned char *GetImagePointer() {return (unsigned char *)GetPointer(0);};
 
protected:
  /** constructor */
  vtkPackedImage();
  /** destructor */
  ~vtkPackedImage();

  int PackType;
};

#endif
