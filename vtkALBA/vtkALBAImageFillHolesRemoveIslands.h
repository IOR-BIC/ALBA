/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAImageFillHolesRemoveIslands
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAImageFillHolesRemoveIslands_H__
#define __vtkALBAImageFillHolesRemoveIslands_H__

#include "albaConfigure.h"
#include "vtkALBAStructuredPointsAlgorithm.h"

class vtkImageData;
class vtkImageData;

#define OFF_PIXEL 0
#define ON_PIXEL 255

/** vtkALBAImageFillHolesRemoveIslands: Fill holes or remove island of the specified size (or less) from the given image data
vtkALBAImageFillHolesRemoveIslands is a vtkStructuredPointsToStructuredPointsFilte that fill holes or remove islands from the specified vtkStructuredPoint
that must be a binary image represented by a vtkUCharArray with values of 0 or 255 only.
*/
//---------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAImageFillHolesRemoveIslands : public vtkALBAStructuredPointsAlgorithm
//---------------------------------------------------------------------------
{
public:

  enum ALGORITHMS
  {
    FILL_HOLES,
    REMOVE_ISLANDS,
    INVALID_ALGORITHM,
  };

  /** Add collect revision method */
  vtkTypeMacro(vtkALBAImageFillHolesRemoveIslands,vtkALBAStructuredPointsAlgorithm);

  /** Dynamic ctor */
  static vtkALBAImageFillHolesRemoveIslands *New();

  /** Set the filter algorithm */
  void SetAlgorithm(int algorithm);

  /** Get the filter algorithm */
  inline int GetAlgorithm(){return Algorithm;};

  /** Set the filter algorithm to fill holes */
  inline void SetAlgorithmToFillHoles(){Algorithm = FILL_HOLES; DiscriminationPixelValue = ON_PIXEL;};

  /** Set the filter algorithm to remove islands */
  inline void SetAlgorithmToRemoveIslands(){Algorithm = REMOVE_ISLANDS; DiscriminationPixelValue = OFF_PIXEL;};

  /** Set The edge size of the areas that will be considered holes or islands */
  vtkSetMacro(EdgeSize,int);

  /** Get the edge size of the areas that will be considered holes or islands */
  vtkGetMacro(EdgeSize,int);

  /** Set if peninsula regions are removed or not */
  vtkSetMacro(RemovePeninsulaRegions,bool);

  /** Get if peninsula regions are removed or not  */
  vtkGetMacro(RemovePeninsulaRegions,bool);

protected:

  /** Execute this filter */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  int Algorithm;                            //> fill holes or remove islands
  unsigned int EdgeSize;                    //> maximum holes/islands size
  unsigned char DiscriminationPixelValue;   //> ON_PIXEL for fill holes, OFF_PIXEL for remove islands
  bool RemovePeninsulaRegions;              //> determine if penisnula pixel are removed or not

private:

  /** Default ctor */
  vtkALBAImageFillHolesRemoveIslands();

  /** Default dtor */
  ~vtkALBAImageFillHolesRemoveIslands();
};

#endif