/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFImageMapToWidgetColors.h,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:28:24 $
  Version:   $Revision: 1.2 $
  Authors:   Alexander Savenko, Mel Krokos
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

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
#include "vtkMAFConfigure.h"

#include "vtkImageMapToColors.h"
#include "vtkMAFTransferFunction2D.h"

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
class vtkDataObject;
class vtkImageData;


class VTK_vtkMAF_EXPORT vtkMAFImageMapToWidgetColors : public vtkImageMapToColors 
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
