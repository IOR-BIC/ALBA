/*=========================================================================
Program: ALBA
Module: mafOpImporterDicomSliceAccHelper
Authors: Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __mafOpImporterDicomSliceAccHelper_h
#define __mafOpImporterDicomSliceAccHelper_h

#include <memory.h>

#include "mafConfigure.h"
#include <vtkRectilinearGridSource.h>
#include "vtkRectilinearGrid.h"
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>

class MAF_EXPORT mafOpImporterDicomSliceAccHelper
{
public:
	mafOpImporterDicomSliceAccHelper();
	virtual ~mafOpImporterDicomSliceAccHelper();


  /**
  Set the slice into the RectilinearGrid*/
  void SetSlice(int slice_num,vtkImageData *slice, double* unRotatedOrigin);
  void SetSlice(int slice_num, vtkStructuredPoints * slice,double* unRotatedOrigin)
  {
	  SetSlice(slice_num, (vtkImageData *)slice, unRotatedOrigin);
  }
 
 
	vtkDataSet* GetNewOutput();

	/** Sets NumOfSlices */
	void SetNumOfSlices(int numOfSlices) { m_NumOfSlices = numOfSlices; }

	
	/** Sets Dims */
	void SetDims(int *dims);

protected:

	/** Sets Origin */
	void SetOrigin(double *origin);

	/** Sets Spacing */
	void SetSpacing(double *spacing);

  bool m_Allocated;
  int m_NumOfSlices;
  int m_Dims[3];
  double m_Spacing[3];
  double m_Origin[3];
  int m_DataType;
	double *m_Zcoord;
	vtkDataArray *m_Scalars;
	
	
  //Allocate the memory for Rectilinear Grid on the base of the dimensions
  //and sets the coordinate vectors calculated from the spacing
  void Allocate();
  
};
#endif