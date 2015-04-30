/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFRGSliceAccumulate.h,v $
  Language:  C++
  Date:      $Date: 2009-03-11 11:40:06 $
  Version:   $Revision: 1.1.2.1 $


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
// .NAME vtkMAFRGSliceAccumulate - read vtk rectilinear grid data file
// .SECTION Description
// vtkMAFRGSliceAccumulate is a source object that reads ASCII or binary 
// rectilinear grid data files in vtk format. See text for format details.

// .SECTION Caveats
// Binary files written on one system may not be readable on other systems.

#ifndef __vtkMAFRGSliceAccumulate_h
#define __vtkMAFRGSliceAccumulate_h

#include <memory.h>

#include "mafConfigure.h"
#include <vtkRectilinearGridSource.h>
#include "vtkRectilinearGrid.h"
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>

class MAF_EXPORT vtkMAFRGSliceAccumulate : public vtkRectilinearGridSource
{
public:
  vtkTypeRevisionMacro(vtkMAFRGSliceAccumulate,vtkRectilinearGridSource);
  void PrintSelf(ostream& os, vtkIndent indent);

	static vtkMAFRGSliceAccumulate *New();

  /**
  Add Slice to RectilinearGrid*/
  void AddSlice(vtkImageData *);
  void AddSlice(vtkStructuredPoints * slice)
  {
	  AddSlice((vtkImageData *)slice);
  }
   
  /**
  Set the slice into the RectilinearGrid*/
  void SetSlice(int ,vtkImageData *, double* orientation);
  void SetSlice(int slice_num, vtkStructuredPoints * slice, double* orientation)
  {
	  SetSlice(slice_num, (vtkImageData *)slice, orientation);
  }
  
  /**
  Set the building axes for the volume*/
	void BuildVolumeOnAxes(int axes) {BuildingAxes = axes;};
	void BuildVolumeOnAxesX() {BuildingAxes = 0;};
	void BuildVolumeOnAxesY() {BuildingAxes = 1;};
	void BuildVolumeOnAxesZ() {BuildingAxes = 2;};

  /**
  Dimensions of rectilinearGrid*/
  vtkSetVector3Macro(Dimensions, int);
  vtkGetVector3Macro(Dimensions, int);

  /**
  Spacing of the slice in RectilinearGrid*/
  vtkSetVector3Macro(Spacing, double);
  vtkGetVector3Macro(Spacing, double);

  /** */
  vtkSetVector3Macro(Origin, double);
  vtkGetVector3Macro(Origin, double);

  /**
  Get the number of slices in RegtilinearGrid*/
  vtkSetMacro(NumberOfSlices, int);
  vtkGetMacro(NumberOfSlices, int);

  /** */
  vtkSetMacro(DataType, int);
  vtkGetMacro(DataType, int);

  vtkRectilinearGrid* GetOutput(){return Slices;};

protected:
  bool Allocated;
  int NumberOfSlices;
  int Dimensions[3];
  double Spacing[3];
  double Origin[3];
  double m_rotationmatrix[3][3];
  int DataType;
	int BuildingAxes;

	vtkMAFRGSliceAccumulate();
  ~vtkMAFRGSliceAccumulate();
	vtkMAFRGSliceAccumulate(const vtkMAFRGSliceAccumulate&);
  void operator=(const vtkMAFRGSliceAccumulate&);

  vtkSetObjectMacro(Slices, vtkRectilinearGrid);
  vtkGetObjectMacro(Slices, vtkRectilinearGrid);

  //Allocate the memory for Rectilinear Grid on the base of the dimensions
  //and sets the coordinate vectors calculated from the spacing
  void Allocate();
  
  vtkRectilinearGrid * Slices;

};
#endif
