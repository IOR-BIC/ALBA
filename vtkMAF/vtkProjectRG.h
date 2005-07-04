/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkProjectRG.h,v $
  Language:  C++
  Date:      $Date: 2005-07-04 17:19:09 $
  Version:   $Revision: 1.1 $


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
// .NAME vtkProjectRG - makes a voxel projection of structured points dataset along axis directions

// .SECTION Description
// vtkProjectRG makes a voxel projection of structured points dataset 
// along axis directions considering opacity.  The output of
// this filter is a structured points dataset. The filter treats input data
// of any topological dimension (i.e., point, line, image, or volume) and 
// generates image output data.
//
// To use this filter set the projection mode  ivar. 
// specify a rectangular region in the data. (Note that these are 0-offset.)
// You can also specify a sampling rate to subsample the data.
//
// Typical applications of this filter are to produce an image from a volume
// for image processing or visualization.


// .SECTION See Also
// vtkGeometryFilter vtkExtractGeometry vtkExtractGrid

#ifndef __vtkProjectRG_h
#define __vtkProjectRG_h

#include "vtkRectilinearGridToRectilinearGridFilter.h"
#include "vtkMAFConfigure.h"

#define VTK_PROJECT_FROM_X 1
#define VTK_PROJECT_FROM_Y 2
#define VTK_PROJECT_FROM_Z 3


class VTK_vtkMAF_EXPORT vtkProjectRG : public vtkRectilinearGridToRectilinearGridFilter
{
public:
  vtkTypeRevisionMacro(vtkProjectRG,vtkRectilinearGridToRectilinearGridFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkProjectRG *New();

  /**
  Control the projection direction.*/
  void SetProjectionModeToX()
    {this->SetProjectionMode(VTK_PROJECT_FROM_X);};
 
  void SetProjectionModeToY()
    {this->SetProjectionMode(VTK_PROJECT_FROM_Y);};
 
  void SetProjectionModeToZ()
    {this->SetProjectionMode(VTK_PROJECT_FROM_Z);};
 
  char *GetProjectionModeAsString(void)
  {
     switch (this->ProjectionMode) {
        case VTK_PROJECT_FROM_X: return "X";
        case VTK_PROJECT_FROM_Y: return "Y";
     }
     return "Z";
  };

  vtkSetMacro(ProjectionMode,int);
  vtkGetMacro(ProjectionMode,int);
    

protected:
  
  vtkProjectRG() ;
 ~vtkProjectRG() {};
  vtkProjectRG(const vtkProjectRG&);
  void operator=(const vtkProjectRG&);

  void ExecuteInformation();
  void Execute();

  int ProjectionMode;  
};


#endif




