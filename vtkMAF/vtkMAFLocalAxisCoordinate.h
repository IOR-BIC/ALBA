/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFLocalAxisCoordinate.h,v $
  Language:  C++
  Date:      $Date: 2009-11-17 09:32:20 $
  Version:   $Revision: 1.2.2.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002/2003 
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3
  40033 Casalecchio di Reno (BO)
  Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.
=========================================================================*/
#ifndef __vtkMAFLocalAxisCoordinate_h
#define __vtkMAFLocalAxisCoordinate_h

#include "vtkMAFConfigure.h"

#include "vtkCoordinate.h"
#include "vtkProp3D.h"
#include "vtkMatrix4x4.h"
#include "vtkDataSet.h"

class vtkViewport;

/**
class name: vtkMAFLocalAxisCoordinate.
*/
class VTK_vtkMAF_EXPORT vtkMAFLocalAxisCoordinate : public vtkCoordinate
{
public:
  //vtkTypeRevisionMacro(vtkMAFLocalAxisCoordinate,vtkCoordinate);
  /** destructor */
  virtual ~vtkMAFLocalAxisCoordinate();
  /** create an instance of the object */
  static vtkMAFLocalAxisCoordinate* New();

  /** macro Set for DataSet member */
  vtkSetObjectMacro(DataSet,vtkDataSet);
  /** macro Get for DataSet member */
  vtkGetObjectMacro(DataSet,vtkDataSet);

  /** macro Set for Mactrix member */
  vtkSetObjectMacro(Matrix,vtkMatrix4x4);
  /** macro Get for Mactrix member */
  vtkGetObjectMacro(Matrix,vtkMatrix4x4);

  /** used only when the coordinate system is VTK_USERDEFINED  */
  virtual double *GetComputedUserDefinedValue(vtkViewport *viewport);

protected:
  /** constructor */
  vtkMAFLocalAxisCoordinate();

private:
  vtkDataSet *DataSet;
  vtkMatrix4x4 *Matrix;
  
  /** Copy Constructor , not implemented */
  vtkMAFLocalAxisCoordinate(const vtkMAFLocalAxisCoordinate&);
  /** operator =, not implemented */
  void operator=(const vtkMAFLocalAxisCoordinate&);
};
#endif
