/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpSubdivide.h,v $
  Language:  C++
  Date:      $Date: 2009-01-07 13:42:06 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2008
  CINECA - Interuniversity Consortium (www.cineca.it) 
  SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

  MafMedical Library use license agreement

  The software named MafMedical Library and any accompanying documentation, 
  manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
  This is an open-source copyright as follows:
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, 
  this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation and/or 
  other materials provided with the distribution.
  * Modified source versions must be plainly marked as such, and must not be misrepresented 
  as being the original software.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
  ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medOpSubdivide_H__
#define __medOpSubdivide_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;

//----------------------------------------------------------------------------
// medOpSubdivide :
//----------------------------------------------------------------------------
/** Apply vtkLinearSubdivideFilter to a VME Surface*/
class medOpSubdivide: public mafOp
{
public:
	medOpSubdivide(const wxString &label = "Subdivide");
	~medOpSubdivide(); 
	
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(medOpSubdivide, mafOp);

  enum U_OP_SUBDIVIDE_ID
  {
    ID_MY_OP = MINID,
    // ToDO: add your custom IDs...
    ID_NUMBER_OF_SUBDIVISION,
    ID_PREVIEW,
    ID_SUBDIVIDE,
  };

  /*virtual*/ mafOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(mafNode *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

  void SetNumberOfSubdivision(int number){ m_NumberOfSubdivision = number;}

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);
	
  void Algorithm();

  void CreateGui();

  vtkPolyData *m_OriginalPolydata;
  vtkPolyData *m_ResultPolydata;

  int m_NumberOfSubdivision;
	
};
#endif
