/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: itkRawMotionImporterUtility.h,v $
  Language:  C++
  Date:      $Date: 2012-04-06 09:04:52 $
  Version:   $Revision: 1.1.2.2 $
  Thanks:    Thanks to Sebastien Barre who developed this class. Thanks to
             Tim Hutton too for the idea.

Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

// .NAME itkRawMotionImporterUtility - Implementation of the ICP algorithm utility.
// .SECTION Description

#ifndef __itkRawMotionImporterUtility_h
#define __itkRawMotionImporterUtility_h
//-----------------------------------------------
// Include:
//-----------------------------------------------
#include "medCommonDefines.h"
#include <vnl/vnl_matrix.h>

/**
  Class Name: itkRawMotionImporterUtility.
  Utility class for reading matrix from file.  Depends from vnl library.
*/
class MED_COMMON_EXPORT itkRawMotionImporterUtility
{
public:
  /** constructor. */
  itkRawMotionImporterUtility(){};
  /** destructor. */
  ~itkRawMotionImporterUtility() {};
  
  
  /** Read the matrix by means of the VNL library. */
  int ReadMatrix(vnl_matrix<double> &M, const char *fname);
};

#endif