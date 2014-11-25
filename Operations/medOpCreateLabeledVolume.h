/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateLabeledVolume
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpCreateLabeledVolume_H__
#define __medOpCreateLabeledVolume_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMELabeledVolume;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// medOpCreateLabeledVolume :
//----------------------------------------------------------------------------
/** 
class name medOpCreateLabeledVolume
Create a medVMELabeledVolume.
*/
class MAF_EXPORT medOpCreateLabeledVolume: public mafOp
{
public:
  /** constructor */
  medOpCreateLabeledVolume(const wxString &label = "Create labeled volume");
  /** destructor */
  ~medOpCreateLabeledVolume(); 

  /** RTTI macro */
  mafTypeMacro(medOpCreateLabeledVolume, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
  /*virtual*/  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode *node);

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();
  
  /** Execute the operation. */
  /*virtual*/ void OpDo();

protected: 
  medVMELabeledVolume *m_LabeledVolume;
};
#endif
