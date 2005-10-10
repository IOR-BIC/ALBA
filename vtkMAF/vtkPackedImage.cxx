/*=========================================================================

  Module:    vtkPackedImage.cxx
  Language:  C++
  Date:      12/1999
  Version:   .1
  Thanks:    

=========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkPackedImage.h"


vtkCxxRevisionMacro(vtkPackedImage, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkPackedImage);

//----------------------------------------------------------------------------
// Construct an instance of vtkPackedImage with no data.
vtkPackedImage::vtkPackedImage()
//----------------------------------------------------------------------------
{
	this->PackType=VTK_IMG_PACK_NONE;
}

//----------------------------------------------------------------------------
vtkPackedImage::~vtkPackedImage()
//----------------------------------------------------------------------------
{
	this->Initialize();
}

//----------------------------------------------------------------------------
void vtkPackedImage::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  vtkUnsignedCharArray::PrintSelf(os,indent);
  os << indent << "PackType: " << this->PackType << endl;
}

//----------------------------------------------------------------------------
void vtkPackedImage::SetImageSize(unsigned long imgsize)
//----------------------------------------------------------------------------
{
	this->SetNumberOfValues(imgsize);
}
//----------------------------------------------------------------------------
void vtkPackedImage::ImportImage(unsigned char *ptr,unsigned long size,int copy)
//----------------------------------------------------------------------------
{
	if (ptr)
	{
		if (copy)
		{
			this->SetImageSize(size);
			memcpy(this->GetImagePointer(),ptr,size);
		}
		else
		{
			this->SetArray(ptr,size,1);
		}
	}
}
