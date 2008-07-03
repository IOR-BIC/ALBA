/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFImageMapToWidgetColors.cxx,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:28:23 $
  Version:   $Revision: 1.2 $

=========================================================================*/
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

#include "vtkMAFTransferFunction2D.h"
#include "vtkMAFImageMapToWidgetColors.h"


vtkCxxRevisionMacro(vtkMAFImageMapToWidgetColors, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkMAFImageMapToWidgetColors);

template<typename type> static inline type clip(type x, type xmin, type xmax) { if (x < xmin) return xmin; if (x > xmax) return xmax; return x; }

//----------------------------------------------------------------------------
// Constructor sets default values
vtkMAFImageMapToWidgetColors::vtkMAFImageMapToWidgetColors() 
{
  this->TransferFunction = NULL;
  this->SetNumberOfThreads(1);
  this->GradientCache = NULL;
}


vtkMAFImageMapToWidgetColors::~vtkMAFImageMapToWidgetColors() 
{
  if (this->TransferFunction != NULL) 
    this->TransferFunction->Delete();
  if (this->GradientCache != NULL)
    delete [] this->GradientCache;
}

//----------------------------------------------------------------------------
void vtkMAFImageMapToWidgetColors::ExecuteData(vtkDataObject *output) 
{
  vtkImageData *outData = (vtkImageData *)(output);
 
  if (this->DataWasPassed) 
  {
    outData->GetPointData()->SetScalars(NULL);
    this->DataWasPassed = 0;
  }

  // prepare gradients
  int extent[6];
  output->GetUpdateExtent(extent);
  const int sx = extent[1] - extent[0] + 1, sy = extent[3] - extent[2] + 1, sz = extent[5] - extent[4] + 1;
  const int newCacheSize = sx * sy * sz;
  bool  newCache = false;
  if (this->GradientCache == NULL || this->GradientCacheSize < newCacheSize) 
  {
    this->GradientCacheSize = newCacheSize;
    delete [] this->GradientCache;
    this->GradientCache = new double [this->GradientCacheSize];
    newCache = true;
  }
 
  // calculate gradients
  if (newCache || this->GradientCacheMTime < this->GetInput()->GetMTime() || memcmp(this->GradientExtent, extent, sizeof(extent)) != 0) 
  {
    memcpy(this->GradientExtent, extent, sizeof(extent));
    void *inPtr = this->GetInput()->GetScalarPointerForExtent(this->GradientExtent);
    switch (this->GetInput()->GetScalarType()) 
    {
      case VTK_CHAR:
        this->UpdateGradientCache((char*)inPtr);
        break;
      case VTK_UNSIGNED_CHAR:
        this->UpdateGradientCache((unsigned char*)inPtr);
        break;
      case VTK_SHORT:
        this->UpdateGradientCache((short*)inPtr);
        break;
      case VTK_UNSIGNED_SHORT:
        this->UpdateGradientCache((unsigned short*)inPtr);
        break;
      default:
        vtkErrorMacro(<< "Execute: Unknown ScalarType");
        return;
    }
    this->GradientCacheMTime.Modified();
  }

  this->vtkImageToImageFilter::ExecuteData(output);
}

//----------------------------------------------------------------------------
template<class T> void vtkMAFImageMapToWidgetColors::UpdateGradientCache(T *dataPointer) 
{
  vtkImageData *imageData = this->GetInput();

  int inDims[3], outDims[3] = { this->GradientExtent[1] - this->GradientExtent[0] + 1, this->GradientExtent[3] - this->GradientExtent[2] + 1, this->GradientExtent[5] - this->GradientExtent[4] + 1};
  imageData->GetDimensions(inDims);
  const int lastIndex[3] = { inDims[0] - 1, inDims[1] - 1, inDims[2] - 1};
  int contIncrementX, contIncrementY, contIncrementZ;
  imageData->GetContinuousIncrements(this->GradientExtent, contIncrementX, contIncrementY, contIncrementZ);
  const int* inc = imageData->GetIncrements();
  const int numberOfInputComponents  = imageData->GetNumberOfScalarComponents();

  double ispacing[3];
  imageData->GetSpacing(ispacing);
  ispacing[0] = 1.f / ispacing[0];
  ispacing[1] = 1.f / ispacing[1];
  ispacing[2] = 1.f / ispacing[2];
  

  double *gradientPointer = this->GradientCache;
  for (int z = this->GradientExtent[4]; z <= this->GradientExtent[5]; z++) 
  {
    const double idz = (z > 0 && z < lastIndex[2]) ? (2.f * ispacing[2]) : ispacing[2];
    const int iz1 = (z < lastIndex[2]) ? inc[2] : 0, iz0 = (z > 0) ? -inc[2] : 0;

    for (int y = this->GradientExtent[2]; y <= this->GradientExtent[3]; y++) 
    {
      const double idy = (y > 0 && y < lastIndex[1]) ? (2.f * ispacing[1]) : ispacing[1];
      const int iy1 = (y < lastIndex[1]) ? inc[1] : 0, iy0 = (y > 0) ? -inc[1] : 0;

      for (int x = this->GradientExtent[0]; x <= this->GradientExtent[1]; x++) 
      {
        const double idx = (x > 0 && x < lastIndex[0]) ? (2.f * ispacing[0]) : ispacing[0];
        const int ix1 = (x < lastIndex[0]) ? inc[0] : 0, ix0 = (x > 0) ? -inc[0] : 0;

        const double gx  = double(dataPointer[ix1] - dataPointer[ix0]) * idx;
        const double gy  = double(dataPointer[iy1] - dataPointer[iy0]) * idy;
        const double gz  = double(dataPointer[iz1] - dataPointer[iz0]) * idz;
        gradientPointer[0] = sqrt(gx * gx + gy * gy + gz * gz); 
        gradientPointer++;
        dataPointer += numberOfInputComponents;
      }
      dataPointer += contIncrementY;
    }
    dataPointer += contIncrementZ;
  }
}

//----------------------------------------------------------------------------
unsigned long vtkMAFImageMapToWidgetColors::GetMTime() 
{
  unsigned long t1 = this->vtkImageToImageFilter::GetMTime();
  if (this->TransferFunction) 
  {
    unsigned long t2 = this->TransferFunction->GetMTime();
    if (t2 > t1)
      t1 = t2;
  }
  return t1;
}

//----------------------------------------------------------------------------
void vtkMAFImageMapToWidgetColors::ExecuteInformation(vtkImageData *inData, vtkImageData *outData) 
{
  outData->SetScalarType(VTK_UNSIGNED_CHAR);
  outData->SetNumberOfScalarComponents(3);
}

//----------------------------------------------------------------------------
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
void vtkMAFImageMapToWidgetColors::ThreadedExecute(vtkImageData *inData, vtkImageData *outData, int outExt[6], int id) 
{
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  unsigned char *outPtr = (unsigned char *)outData->GetScalarPointerForExtent(outExt);
  assert(outData->GetScalarType() == VTK_UNSIGNED_CHAR);

  switch (inData->GetScalarType()) 
  {
    case VTK_CHAR:
      this->Execute(inData, (char *)(inPtr), outData, outPtr, outExt);
      break;
    case VTK_UNSIGNED_CHAR:
      this->Execute(inData, (unsigned char *)(inPtr), outData, outPtr, outExt);
      break;
    case VTK_SHORT:
      this->Execute(inData, (short *)(inPtr), outData, outPtr, outExt);
      break;
    case VTK_UNSIGNED_SHORT:
      this->Execute(inData, (unsigned short *)(inPtr), outData, outPtr, outExt);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
  }
}

//----------------------------------------------------------------------------
// This non-templated function executes the filter for any type of data.
template<class T> void vtkMAFImageMapToWidgetColors::Execute(vtkImageData *inData,  T *inPtr, vtkImageData *outData, unsigned char *outPtr, int outExt[6]) 
{
  // color mapping
  const double shift = this->Window / 2.0 - this->Level;
  const double scale = 255.0 / this->Window;

  // widget opacity mapping
  tfWidget widget;
  bool widgetValid = this->TransferFunction != NULL && this->WidgetIndex >=0 && this->WidgetIndex < this->TransferFunction->GetNumberOfWidgets();
  if (widgetValid) 
  {
    widget = this->TransferFunction->GetWidget(this->WidgetIndex);
    widget.Update();
  }

  // find the region to loop over
  const int extX = outExt[1] - outExt[0] + 1;
  const int extY = outExt[3] - outExt[2] + 1; 
  const int extZ = outExt[5] - outExt[4] + 1;

  // Get increments to march through data 
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  const int numberOfInputComponents  = inData->GetNumberOfScalarComponents();
  const int numberOfOutputComponents = outData->GetNumberOfScalarComponents();
  assert(numberOfOutputComponents == 3);

  // Loop through output pixels
  unsigned char *optr = outPtr;
  const T       *iptr = inPtr;
  const double   *gptr = this->GradientCache;
  for (int z = 0; z < extZ; z++) 
  {
    for (int y = 0; y < extY; y++) 
    {
      for (int x = 0; x < extX; x++) 
      {
        unsigned char originalColor = (unsigned char)clip(int(((*iptr + shift) * scale)), 0, 255);

        if (widgetValid) 
        {
          optr[0] = (unsigned char)(widget.Attenuation(*iptr, *gptr) * 255.0);
          optr[1] = originalColor >> 1;
          optr[2] = originalColor;
        }
        else 
        {
          optr[0] = 0;
          optr[1] = originalColor >> 1;
          optr[2] = originalColor;
        }
        iptr += numberOfInputComponents;
        optr += numberOfOutputComponents;
        gptr += 1;
      }      
      //gptr   += gIncY;
      optr += outIncY;
      iptr  += inIncY;
    }
    //gptr += gIncZ;
    optr += outIncZ;
    iptr += inIncZ;
  }
}
