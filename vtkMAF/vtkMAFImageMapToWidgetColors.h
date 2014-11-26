/*=========================================================================

 Program: MAF2
 Module: vtkMAFImageMapToWidgetColors
 Authors: Alexander Savenko, Mel Krokos
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFImageMapToWidgetColors - a special filter for demonstrating which part of the image is covered by a 2D transfer function widget
// .SECTION Description

// .SECTION See Also
// vtkVolumeProperty2 vtkMAFTransferFunction2D

#ifndef __vtkMAFImageMapToWidgetColors_h
#define __vtkMAFImageMapToWidgetColors_h

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------
#include "mafConfigure.h"

#include "vtkImageMapToColors.h"
#include "vtkMAFTransferFunction2D.h"

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
class vtkDataObject;
class vtkImageData;


class MAF_EXPORT vtkMAFImageMapToWidgetColors : public vtkImageMapToColors 
{
public:
  static vtkMAFImageMapToWidgetColors *New();
  vtkTypeRevisionMacro(vtkMAFImageMapToWidgetColors,vtkImageMapToColors);

  /**
  Set / Get transfer function*/
  vtkSetObjectMacro(TransferFunction, vtkMAFTransferFunction2D);
  vtkGetObjectMacro(TransferFunction, vtkMAFTransferFunction2D);

  /**
  Set / Get widget index*/
  vtkSetMacro(WidgetIndex, int);
  vtkGetMacro(WidgetIndex, int);

  /**
  Set / Get the Window to use -> modulation will be performed on the 
  color based on (S - (L - W/2))/W where S is the scalar value, L is
  the level and W is the window.*/
  vtkSetMacro( Window, double );
  vtkGetMacro( Window, double );
  
  /**
  Set / Get the Level to use -> modulation will be performed on the 
  color based on (S - (L - W/2))/W where S is the scalar value, L is
  the level and W is the window.*/
  vtkSetMacro( Level, double );
  vtkGetMacro( Level, double );
  
  unsigned long GetMTime();

protected:
  vtkMAFImageMapToWidgetColors();
  ~vtkMAFImageMapToWidgetColors();

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){this->vtkImageMapToColors::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
                       int extent[6], int id);
  void ExecuteData(vtkDataObject *output);
  template<class T> void UpdateGradientCache(T *dataPointer);

  vtkMAFTransferFunction2D *TransferFunction;

  int   WidgetIndex;

  double Window;
  double Level;

  double       *GradientCache;
  int          GradientCacheSize;
  vtkTimeStamp GradientCacheMTime;
  int          GradientExtent[6];

  template <class T> void Execute(vtkImageData *inData,  T *inPtr,
                                  vtkImageData *outData, unsigned char *outPtr, int outExt[6]);

private:
  vtkMAFImageMapToWidgetColors(const vtkMAFImageMapToWidgetColors&);  // Not implemented.
  void operator=(const vtkMAFImageMapToWidgetColors&);  // Not implemented.
};
#endif
