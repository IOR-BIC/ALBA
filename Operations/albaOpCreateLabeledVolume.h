/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateLabeledVolume
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateLabeledVolume_H__
#define __albaOpCreateLabeledVolume_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMELabeledVolume;
class albaGui;
class albaEvent;
//----------------------------------------------------------------------------
// albaOpCreateLabeledVolume :
//----------------------------------------------------------------------------
/** 
class name albaOpCreateLabeledVolume
Create a albaVMELabeledVolume.
*/
class ALBA_EXPORT albaOpCreateLabeledVolume: public albaOp
{
public:
  /** constructor */
  albaOpCreateLabeledVolume(const wxString &label = "Create Labeled Volume");
  /** destructor */
  ~albaOpCreateLabeledVolume(); 

  /** RTTI macro */
  albaTypeMacro(albaOpCreateLabeledVolume, albaOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
  /*virtual*/  albaOp* Copy();



  /** Builds operation's interface. */
  /*virtual*/ void OpRun();
  
  /** Execute the operation. */
  /*virtual*/ void OpDo();

protected: 

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool InternalAccept(albaVME*node);

  albaVMELabeledVolume *m_LabeledVolume;
};
#endif
