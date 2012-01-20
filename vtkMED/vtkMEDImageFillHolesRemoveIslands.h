/*===========================================================================
Program:   Medical
Module:    $RCSfile: vtkMEDImageFillHolesRemoveIslands.h,v $
Language:  C++
Date:      $Date: 2012-01-20 15:28:07 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
=============================================================================
Copyright (c) 2010
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
===========================================================================*/

#ifndef __vtkMEDImageFillHolesRemoveIslands_H__
#define __vtkMEDImageFillHolesRemoveIslands_H__

#include "vtkMEDConfigure.h"
#include "vtkStructuredPointsToStructuredPointsFilter.h"

class vtkImageData;
class vtkStructuredPoints;

#define OFF_PIXEL 0
#define ON_PIXEL 255

/** vtkMEDImageFillHolesRemoveIslands: Fill holes or remove island of the specified size (or less) from the given image data
vtkMEDImageFillHolesRemoveIslands is a vtkStructuredPointsToStructuredPointsFilte that fill holes or remove islands from the specified vtkStructuredPoint
that must be a binary image represented by a vtkUCharArray with values of 0 or 255 only.
*/
//---------------------------------------------------------------------------
class VTK_FILTERING_EXPORT vtkMEDImageFillHolesRemoveIslands : public vtkStructuredPointsToStructuredPointsFilter
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
  vtkTypeRevisionMacro(vtkMEDImageFillHolesRemoveIslands,vtkStructuredPointsToStructuredPointsFilter);

  /** Dynamic ctor */
  static vtkMEDImageFillHolesRemoveIslands *New();

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

protected:

  /** Execute this filter */
  void Execute();

  /** Initialize this filter */
  //void Initialize();

  int Algorithm;
  unsigned int EdgeSize;
  unsigned char DiscriminationPixelValue;

private:

  /** Default ctor */
  vtkMEDImageFillHolesRemoveIslands();

  /** Default dtor */
  ~vtkMEDImageFillHolesRemoveIslands();

};

#endif