/*=========================================================================

 Program: MAF2Medical
 Module: medOpSubdivide
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpSubdivide_H__
#define __medOpSubdivide_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;

//----------------------------------------------------------------------------
// medOpSubdivide :
//----------------------------------------------------------------------------
/** Apply vtkLinearSubdivideFilter to a VME Surface*/
class MED_OPERATION_EXPORT medOpSubdivide: public mafOp
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
