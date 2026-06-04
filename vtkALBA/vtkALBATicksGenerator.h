/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATicksGenerator
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __vtkALBATicksGenerator_h
#define __vtkALBATicksGenerator_h

#include "vtkPolyDataAlgorithm.h"
  
/**
 * VTK algorithm for generating tick marks on geometric objects.
 *
 * vtkALBATicksGenerator is a vtkPolyDataAlgorithm that generates tick marks
 * suitable for annotating or marking positions on polydata geometry. It processes
 * input polydata and produces output polydata containing the generated tick geometry.
 */
class vtkALBATicksGenerator : public vtkPolyDataAlgorithm
{
public:
  /**
   * Create a new instance of vtkALBATicksGenerator.
   * @return A new vtkALBATicksGenerator object.
   */
  static vtkALBATicksGenerator* New();
  vtkTypeMacro(vtkALBATicksGenerator, vtkPolyDataAlgorithm);

protected:
  /** Constructor. */
  vtkALBATicksGenerator();

  /* Destructor. */
  ~vtkALBATicksGenerator();

  /**
   * Process the input data to generate tick marks.
   *
   * This method is called by the VTK pipeline to execute the algorithm.
   * It takes the input polydata and generates the corresponding tick marks.
   *
   * @param request The VTK request object.
   * @param inputVector The input data vector.
   * @param outputVector The output data vector.
   * @return VTK_OK if the execution was successful, VTK_ERROR otherwise.
   */
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkALBATicksGenerator(const vtkALBATicksGenerator&);
  void operator=(const vtkALBATicksGenerator&);
};

#endif