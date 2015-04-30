/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkRectilinearGridToFilterRectilinearGrid.h
  Language:  C++
  Date:      2001/01
  Version:   1.2
  Author:    Roberto Gori (img0@cineca.it), 3.x porting Marco Petrone (m.petrone@cineca.it) 


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
// .NAME vtkMAFRectilinearGridToRectilinearGridFilter - abstract filter class
// .SECTION Description
// vtkMAFRectilinearGridToRectilinearGridFilter is a filter whose subclasses take as
// input rectilinear grid datasets and generate polygonal data on output.

// .SECTION See Also
// vtkRectilinearGridGeometryFilter vtkRectilinearGridOutlineFilter

#ifndef __vtkMAFRectilinearGridToRectilinearGridFilter_h
#define __vtkMAFRectilinearGridToRectilinearGridFilter_h

#include "mafConfigure.h"
#include "vtkRectilinearGridSource.h"
#include "vtkRectilinearGrid.h"
/**
class name: vtkMAFRectilinearGridToRectilinearGridFilter
 vtkMAFRectilinearGridToRectilinearGridFilter is a filter whose subclasses take as
 input rectilinear grid datasets and generate polygonal data on output.
*/
class MAF_EXPORT vtkMAFRectilinearGridToRectilinearGridFilter : public vtkRectilinearGridSource
{
public:
  /** create an instance of the object */
  static vtkMAFRectilinearGridToRectilinearGridFilter *New();
  /** RTTI Macro */
  vtkTypeRevisionMacro(vtkMAFRectilinearGridToRectilinearGridFilter,vtkRectilinearGridSource);

  /**  Set  the input data or filter.*/
  virtual void SetInput(vtkRectilinearGrid *input);
  /**  Get  the input data or filter.*/
  vtkRectilinearGrid *GetInput();

protected:
  /** constructor */
  vtkMAFRectilinearGridToRectilinearGridFilter() {this->NumberOfRequiredInputs = 1;};
  /** destructor */
  ~vtkMAFRectilinearGridToRectilinearGridFilter() {this->SetInput(NULL);};
  /** operator =, not implemented */
  void operator=(const vtkMAFRectilinearGridToRectilinearGridFilter&) {};
  /** Copy Constructor , not implemented */
  vtkMAFRectilinearGridToRectilinearGridFilter(const vtkMAFRectilinearGridToRectilinearGridFilter&) {};

};

#endif




