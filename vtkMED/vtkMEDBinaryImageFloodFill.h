/*===========================================================================
Program:   Medical
Module:    $RCSfile: vtkMEDImageFillHolesRemoveIslands.h,v $
Language:  C++
Date:      $Date: 2012-01-23 12:50:32 $
Version:   $Revision: 1.1.2.2 $
Authors:   Alberto Losi
=============================================================================
Copyright (c) 2010
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
===========================================================================*/

#ifndef __vtkMEDBinaryImageFloodFill_H__
#define __vvtkMEDBinaryImageFloodFill_H__

#include "vtkMEDConfigure.h"
#include "vtkStructuredPointsToStructuredPointsFilter.h"

class vtkImageData;
class vtkStructuredPoints;

#define OFF_PIXEL 0
#define ON_PIXEL 255

/** vtkMEDBinaryImageFloodFill: Flood fill filter for binary images.
    This filter operate on binary images and fill/erease the area identified by the specified seed.
*/
//---------------------------------------------------------------------------
class VTK_vtkMED_EXPORT vtkMEDBinaryImageFloodFill : public vtkStructuredPointsToStructuredPointsFilter
//---------------------------------------------------------------------------
{
public:

  /** Add collect revision method */
  vtkTypeRevisionMacro(vtkMEDBinaryImageFloodFill,vtkStructuredPointsToStructuredPointsFilter);

  /** Dynamic ctor */
  static vtkMEDBinaryImageFloodFill *New();

  /** Set the filter to fill the "picked" area */
  inline void SetFill(){ReplaceValue = ON_PIXEL; Threshold[0] = OFF_PIXEL; Threshold[1] = (ON_PIXEL - OFF_PIXEL) / 2;};

  /** Set the filter to erease the "picked" area */
  inline void SetErease(){ReplaceValue = OFF_PIXEL; Threshold[0] = (ON_PIXEL - OFF_PIXEL) / 2; Threshold[1] = ON_PIXEL;};


  /** Set the filter to erease the "picked" area */
  inline void SetFillErease(bool erease){if(!erease){SetFill();}else{SetErease();}};

  /** Set the seed that identify the area */
  vtkSetVectorMacro(Seed, int, 3);

  /** Get the seed that identify the area */
  vtkGetVectorMacro(Seed, int, 3);

protected:

  /** Execute this filter */
  void Execute();

  unsigned char ReplaceValue; //> ON_PIXEL for fill OFF_PIXEL for erease
  unsigned char Threshold[2]; //> threshold for connectivity threshold filter
  int Seed[3];                //> point that identify the area

private:

  /** Default ctor */
  vtkMEDBinaryImageFloodFill();

  /** Default dtor */
  ~vtkMEDBinaryImageFloodFill();

  /** Itk pipeline for flood fill */
  template <unsigned int ImageDimension>
  vtkStructuredPoints *FloodFill(vtkStructuredPoints *input);

};

#endif