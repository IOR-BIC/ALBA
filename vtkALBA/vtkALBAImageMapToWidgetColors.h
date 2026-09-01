/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAImageMapToWidgetColors
 Authors: Alexander Savenko, Mel Krokos
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBAImageMapToWidgetColors - a special filter for demonstrating which part of the image is covered by a 2D transfer function widget
// .SECTION Description

// .SECTION See Also
// vtkVolumeProperty2 vtkALBATransferFunction2D

#ifndef __vtkALBAImageMapToWidgetColors_h
#define __vtkALBAImageMapToWidgetColors_h

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------
#include "albaConfigure.h"

#include "vtkImageMapToColors.h"
#include "vtkALBATransferFunction2D.h"

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
class vtkDataObject;
class vtkImageData;


class ALBA_EXPORT vtkALBAImageMapToWidgetColors : public vtkImageMapToColors 
{
public:
  static vtkALBAImageMapToWidgetColors *New();
  vtkTypeMacro(vtkALBAImageMapToWidgetColors,vtkImageMapToColors);

  /**
  Set / Get transfer function*/
  vtkSetObjectMacro(TransferFunction, vtkALBATransferFunction2D);
  vtkGetObjectMacro(TransferFunction, vtkALBATransferFunction2D);

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
  
	vtkMTimeType GetMTime();

protected:
  vtkALBAImageMapToWidgetColors();
  ~vtkALBAImageMapToWidgetColors();

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
                       int extent[6], int id);
  void ExecuteData(vtkDataObject *output);
  template<class T> void UpdateGradientCache(T *dataPointer);

  vtkALBATransferFunction2D *TransferFunction;

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
  vtkALBAImageMapToWidgetColors(const vtkALBAImageMapToWidgetColors&);  // Not implemented.
  void operator=(const vtkALBAImageMapToWidgetColors&);  // Not implemented.
};
#endif
