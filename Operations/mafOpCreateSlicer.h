/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSlicer
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateSlicer_H__
#define __mafOpCreateSlicer_H__

#include "mafOp.h"
#include "mafVMEVolume.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESlicer;

//----------------------------------------------------------------------------
// mafOpCreateSlicer :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpCreateSlicer: public mafOp
{
public:
  mafOpCreateSlicer(const wxString &label = "CreateSlicer");
 ~mafOpCreateSlicer(); 

  mafTypeMacro(mafOpCreateSlicer, mafOp);

  mafOp* Copy();

  bool Accept(mafVME *node);
  void OpRun();
  void OpDo();

  static bool VolumeAccept(mafVME* node) {return(node != NULL  && node->IsMAFType(mafVMEVolume));};

protected: 
  mafVMESlicer *m_Slicer;
  mafVME      *m_SlicedVME;
};
#endif
