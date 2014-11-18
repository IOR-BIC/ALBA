/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkMEDRegionGrowingLocalGlobalThreshold.h,v $

Copyright (c) Matteo Giacomoni
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMEDRegionGrowingLocalGlobalThreshold_h
#define __vtkMEDRegionGrowingLocalGlobalThreshold_h

class vtkImageData;

#include "vtkProcessObject.h"
#include "vtkMAFConfigure.h"

class VTK_vtkMAF_EXPORT vtkMEDRegionGrowingLocalGlobalThreshold : public vtkProcessObject
{
public:
  vtkTypeRevisionMacro(vtkMEDRegionGrowingLocalGlobalThreshold,vtkProcessObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Creates an instance of vtkMEDRegionGrowingLocalGlobalThreshold with the following
  static vtkMEDRegionGrowingLocalGlobalThreshold *New();

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
  vtkMEDRegionGrowingLocalGlobalThreshold();

  /** Destructor */
  ~vtkMEDRegionGrowingLocalGlobalThreshold();

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
  vtkMEDRegionGrowingLocalGlobalThreshold(const vtkMEDRegionGrowingLocalGlobalThreshold&);  // Not implemented.

  /** Operator = */
  void operator=(const vtkMEDRegionGrowingLocalGlobalThreshold&);  // Not implemented.

  vtkImageData *Input;
  vtkImageData *Output;

  int OutputScalarType; ///<Default VTK_SHORT

  friend class vtkMEDRegionGrowingLocalGlobalThresholdTest;
};

#endif
