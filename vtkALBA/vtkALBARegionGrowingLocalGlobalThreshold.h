/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkALBARegionGrowingLocalGlobalThreshold.h,v $

Copyright (c) Matteo Giacomoni
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBARegionGrowingLocalGlobalThreshold_h
#define __vtkALBARegionGrowingLocalGlobalThreshold_h

class vtkImageData;

#include "albaConfigure.h"
#include "vtkDataObject.h"

class ALBA_EXPORT vtkALBARegionGrowingLocalGlobalThreshold : public vtkDataObject
{
public:
  vtkTypeMacro(vtkALBARegionGrowingLocalGlobalThreshold,vtkDataObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates an instance of vtkALBARegionGrowingLocalGlobalThreshold with the following
  static vtkALBARegionGrowingLocalGlobalThreshold *New();

  vtkGetMacro(LowerLabel,double);
  vtkSetMacro(LowerLabel,double);

  vtkGetMacro(UpperLabel,double);
  vtkSetMacro(UpperLabel,double);

  vtkGetMacro(LowerThreshold,double);
  vtkSetMacro(LowerThreshold,double);

  vtkGetMacro(UpperThreshold,double);
  vtkSetMacro(UpperThreshold,double);

  vtkGetMacro(OutputScalarType,int);
  vtkSetMacro(OutputScalarType,int);

  /** Set the input data */
  void SetInput(vtkImageData *UserSetInput) {this->Input = UserSetInput;};  

  /** Process the algorithm */
  void Update();

  /** Get the output data */
  vtkImageData* GetOutput(){return Output;};

protected:
  /** Constructor */
  vtkALBARegionGrowingLocalGlobalThreshold();

  /** Destructor */
  ~vtkALBARegionGrowingLocalGlobalThreshold();

  double LowerLabel;
  double UpperLabel;
  double LowerThreshold;
  double UpperThreshold;

  /** Compute the standard deviation of the scalar of the 26 voxel near of the index */
  double ComputeStandardDeviation(int index, int indexNearest[26] , double mean, int &error, vtkImageData *imBordered);

  /** Compute the mean value of the scalar of the 26 voxel near of the index */
  double ComputeMeanValue(int index, int indexNearest[26] , int &error, vtkImageData *imBordered);

  /** Generate a border to allow a correct compute of the mean value and standard deviation */
  void BorderCreate(vtkImageData *imToApplyBorder);

  /** Compute the index of the 26 nearest points */
  void ComputeIndexNearstPoints(int index, int indexNearest[26] , int &error, vtkImageData *imBordered);

private:
  /** Copy constructor */
  vtkALBARegionGrowingLocalGlobalThreshold(const vtkALBARegionGrowingLocalGlobalThreshold&);  // Not implemented.

  /** Operator = */
  void operator=(const vtkALBARegionGrowingLocalGlobalThreshold&);  // Not implemented.

  vtkImageData *Input;
  vtkImageData *Output;

  int OutputScalarType; ///<Default VTK_SHORT

  friend class vtkALBARegionGrowingLocalGlobalThresholdTest;
};

#endif
