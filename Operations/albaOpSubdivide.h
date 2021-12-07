/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSubdivide
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpSubdivide_H__
#define __albaOpSubdivide_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;

//----------------------------------------------------------------------------
// albaOpSubdivide :
//----------------------------------------------------------------------------
/** Apply vtkLinearSubdivideFilter to a VME Surface*/
class ALBA_EXPORT albaOpSubdivide: public albaOp
{
public:
	albaOpSubdivide(const wxString &label = "Subdivide");
	~albaOpSubdivide(); 
	
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpSubdivide, albaOp);

  enum U_OP_SUBDIVIDE_ID
  {
    ID_MY_OP = MINID,
    // ToDO: add your custom IDs...
    ID_NUMBER_OF_SUBDIVISION,
    ID_PREVIEW,
    ID_SUBDIVIDE,
  };

  /*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

  void SetNumberOfSubdivision(int number){ m_NumberOfSubdivision = number;}

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);
	
  void Algorithm();

  void CreateGui();

  vtkPolyData *m_OriginalPolydata;
  vtkPolyData *m_ResultPolydata;

  int m_NumberOfSubdivision;
	
};
#endif
