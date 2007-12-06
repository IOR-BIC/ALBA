/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFPolyDataToSinglePolyLine.h,v $
  Language:  C++
  Date:      $Date: 2007-12-06 09:12:40 $
  Version:   $Revision: 1.1 $
  Authors:   Alessandro Chiarini
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

#ifndef __vtkMAFPolyDataToSinglePolyLine_h
#define __vtkMAFPolyDataToSinglePolyLine_h

#include "vtkMAFConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"
//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMAFPolyDataToSinglePolyLine : public vtkPolyDataToPolyDataFilter
//----------------------------------------------------------------------------
{
public:
  //vtkTypeRevisionMacro(vtkFixedCutter,vtkCutter);

           vtkMAFPolyDataToSinglePolyLine();
  virtual ~vtkMAFPolyDataToSinglePolyLine();
  static vtkMAFPolyDataToSinglePolyLine* New();

protected:
  void Execute();

private:
	vtkMAFPolyDataToSinglePolyLine(const vtkMAFPolyDataToSinglePolyLine&);  // Not implemented.
  void operator=(const vtkMAFPolyDataToSinglePolyLine&);  // Not implemented.
};
#endif
