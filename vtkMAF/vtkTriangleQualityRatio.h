/*=========================================================================

 Program: MAF2Medical
 Module: vtkTriangleQualityRatio
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkTriangleQualityRatio_h
#define __vtkTriangleQualityRatio_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "vtkObject.h"
#include "mafConfigure.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;

/**
    class name: vtkTriangleQualityRatio
    This class check the quality of each triangle of a polydata, according to a simple algorithm:
    qualityLocal = 2.0 * sqrt(3.0)/0.5 * perimeter * longestEdge / area;
    and assign each value as a scalar to the correspondent triangle.
*/
class MAF_EXPORT vtkTriangleQualityRatio : public vtkObject 
{

public:
  /** create instance of the object*/
	static vtkTriangleQualityRatio *New();

  /** RTTI macro */
	vtkTypeMacro(vtkTriangleQualityRatio,vtkObject);

	/** To get average value */
	double GetMeanRatio() {return this->MeanRatio;};

	/**To get the max value */
	double GetMaxRatio() {return this->MaxRatio;};

	/** To get the min value */
	double GetMinRatio() {return this->MinRatio;};

	/** Set a PolyData as input */
	void SetInput(vtkPolyData *UserSetInput) {this->Input = UserSetInput;};  

  /** classical update method */
	void Update();

  /** retrieve the polydata with quality scalars in the cells*/
	vtkPolyData* GetOutput(){return Output;};

protected:
  /** object constructor */
	vtkTriangleQualityRatio();
  /** object destructor */
	~vtkTriangleQualityRatio();

	double MeanRatio;
	double MaxRatio;
	double MinRatio;

	vtkPolyData *Input;
	vtkPolyData *Output;
};

#endif