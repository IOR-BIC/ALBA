/*=========================================================================

 Program: MAF2
 Module: mafOpCreateLabeledVolume
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateLabeledVolume_H__
#define __mafOpCreateLabeledVolume_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMELabeledVolume;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mafOpCreateLabeledVolume :
//----------------------------------------------------------------------------
/** 
class name mafOpCreateLabeledVolume
Create a mafVMELabeledVolume.
*/
class MAF_EXPORT mafOpCreateLabeledVolume: public mafOp
{
public:
  /** constructor */
  mafOpCreateLabeledVolume(const wxString &label = "Create labeled volume");
  /** destructor */
  ~mafOpCreateLabeledVolume(); 

  /** RTTI macro */
  mafTypeMacro(mafOpCreateLabeledVolume, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
  /*virtual*/  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafVME*node);

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();
  
  /** Execute the operation. */
  /*virtual*/ void OpDo();

protected: 
  mafVMELabeledVolume *m_LabeledVolume;
};
#endif
