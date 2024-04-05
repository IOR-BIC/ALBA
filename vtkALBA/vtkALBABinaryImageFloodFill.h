/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBABinaryImageFloodFill
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBABinaryImageFloodFill_H__
#define __vvtkALBABinaryImageFloodFill_H__

#include "albaConfigure.h"
#include "vtkALBAStructuredPointsAlgorithm.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"


class vtkImageData;
class vtkImageData;

#define OFF_PIXEL 0
#define ON_PIXEL 255

/** vtkALBABinaryImageFloodFill: Flood fill filter for binary images.
    This filter operate on binary images and fill/erease the area identified by the specified seed.
*/
//---------------------------------------------------------------------------
class ALBA_EXPORT vtkALBABinaryImageFloodFill : public vtkALBAStructuredPointsAlgorithm
//---------------------------------------------------------------------------
{
public:

  /** Add collect revision method */
  vtkTypeMacro(vtkALBABinaryImageFloodFill,vtkALBAStructuredPointsAlgorithm);

  /** Dynamic ctor */
  static vtkALBABinaryImageFloodFill *New();

  /** Set the filter to fill the "picked" area */
  inline void SetFill(){/*ReplaceValue = ON_PIXEL;*/ Threshold[0] = OFF_PIXEL; Threshold[1] = (ON_PIXEL - OFF_PIXEL) / 2; Erase = false;};

  /** Set the filter to erase the "picked" area */
  inline void SetErase(){/*ReplaceValue = OFF_PIXEL;*/ Threshold[0] = (ON_PIXEL - OFF_PIXEL) / 2; Threshold[1] = ON_PIXEL; Erase = true;};


  /** Set the filter to erase the "picked" area */
  inline void SetFillErase(bool erase){if(!erase){SetFill();}else{SetErase();}};

  /** Set the seed that identify the area */
  //vtkSetVectorMacro(Seed, int, 3);
  vtkSetMacro(Seed, vtkIdType);

  /** Get the seed that identify the area */
  //vtkGetVectorMacro(Seed, int, 3);
  vtkGetMacro(Seed, vtkIdType);

  /** Get the center of the filled area */
  vtkGetMacro(Center, vtkIdType);

protected:

  /** Execute this filter */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  unsigned char ReplaceValue; //> ON_PIXEL
  unsigned char Threshold[2]; //> Threshold for connectivity threshold filter
  vtkIdType Seed;             //> Id of the seed (Point that identify the connected area)
  int ItkSeed[3];             //> Point that identify the area
  bool Erase;                 //> Determine if fill or erase
  vtkIdType Center;           //> Center of the filled region

private:

  /** Default ctor */
  vtkALBABinaryImageFloodFill();

  /** Default dtor */
  ~vtkALBABinaryImageFloodFill();

  /** Itk pipeline for flood fill */
  template <unsigned int ImageDimension>
  vtkImageData *FloodFill(vtkImageData *input);

  /** Compute Itk seed for the given Vtk id (Seed) */
  void ComputeItkSeed();
};

#endif

