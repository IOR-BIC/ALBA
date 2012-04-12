/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkMEDVolumeSlicerNotInterpolated.h,v $
  Language:  C++
  Date:      $Date: 2012-04-12 12:39:04 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __vtkMEDVolumeSlicerNotInterpolated_H__
#define __vtkMEDVolumeSlicerNotInterpolated_H__

#include "vtkImageData.h"
#include "vtkMEDConfigure.h"
#include "vtkDataSetToImageFilter.h"

//----------------------------------------------------------------------------
class VTK_vtkMED_EXPORT vtkMEDVolumeSlicerNotInterpolated : public vtkDataSetToImageFilter
//----------------------------------------------------------------------------
{
public:

  /** Public enumeration for slice axis directions */
  enum SLICE_AXIS_ID
  {
    SLICE_X,
    SLICE_Y,
    SLICE_Z
  };

  /** */
  static vtkMEDVolumeSlicerNotInterpolated *New();

  /** RTTI Macro */
  vtkTypeRevisionMacro(vtkMEDVolumeSlicerNotInterpolated, vtkDataSetToImageFilter);

  /** Set the slicer output */
  inline void SetOutput(vtkImageData * data) {vtkImageSource::SetOutput(data);};

  /** Set the slice origin */
  vtkSetVector3Macro(SliceOrigin,double);

  /** Get the slice origin */
  vtkGetVector3Macro(SliceOrigin,double);

  /** Set the slice direction */
  vtkSetMacro(SliceAxis,int);

protected:

  /** ctor */
  vtkMEDVolumeSlicerNotInterpolated();

  /** dtor */
  ~vtkMEDVolumeSlicerNotInterpolated();

  /** By default, UpdateInformation calls this method to copy information
  unmodified from the input to the output.*/
  virtual void ExecuteInformation();

   /**
   This method is the one that should be used by subclasses, right now the 
   default implementation is to call the backwards compatibility method */
   /*virtual*/void ExecuteData(vtkDataObject *output);

   /** Create geometry for the slice. */
   virtual void ExecuteData(vtkImageData *output);

  double SliceOrigin[3];            //< Origin of the slice
  int SliceAxis;                    //< Axis on which slicing is performed (only "perpendicular" slicing is allowed)
  int NumberOfComponents;           //< Number of scalar components
  int BaseIndex;                    //< Starting point of the slice
  int SliceDimensions[2];           //< 2D dimensions of the slice (in number of points)
  int InputDimensions[3];           //< Input volume dimensions
  double InputSpacing[3];           //< Input volume spacing
private:


};
#endif //#ifndef __vtkMEDVolumeSlicerNotInterpolated_H__