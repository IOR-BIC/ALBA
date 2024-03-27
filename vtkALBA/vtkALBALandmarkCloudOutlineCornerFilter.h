/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAPointsOutlineCornerFilter.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen, Gianluigi Crimi
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBAPointsOutlineCornerFilter - create wireframe outline corners for a Landmark Cloud
// .SECTION Description
// This filters differ from vtkOutlineCornerFilter because uses all point of the input source instead of 
// the subset that is contained in cell structure, this is because the landmark cloud manage show
// of the landmarks by adding/eliminating the cells on the output, but a landmark show/hide should not change
// the outline of the landmark. 
// This filter also manage the radius of the cloud by enlarging the points bounds of the size of the cloud radius
// in each dimension.
#ifndef __vtkALBAPointsOutlineCornerFilter_h
#define __vtkALBAPointsOutlineCornerFilter_h

#include "vtkPolyDataAlgorithm.h"
class vtkOutlineCornerSource;

class vtkALBALandmarkCloudOutlineCornerFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkALBALandmarkCloudOutlineCornerFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct outline corner filter with default corner factor = 0.2
  static vtkALBALandmarkCloudOutlineCornerFilter *New();

  // Description:
  // Set/Get the factor that controls the relative size of the corners
  // to the length of the corresponding bounds
  vtkSetClampMacro(CornerFactor, double, 0.001, 0.5);
  vtkGetMacro(CornerFactor, double);

	vtkGetMacro(CloudRadius, double);
	vtkSetMacro(CloudRadius, double);

protected:
  vtkALBALandmarkCloudOutlineCornerFilter();
  ~vtkALBALandmarkCloudOutlineCornerFilter();

  vtkOutlineCornerSource *OutlineCornerSource;

	/** Execute method */
	int RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

	/** only check if input is not null */
	int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  double CornerFactor;
	double CloudRadius;
private:
  vtkALBALandmarkCloudOutlineCornerFilter(const vtkALBALandmarkCloudOutlineCornerFilter&);  // Not implemented.
  void operator=(const vtkALBALandmarkCloudOutlineCornerFilter&);  // Not implemented.
};

#endif
