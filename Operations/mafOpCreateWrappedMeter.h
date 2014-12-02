/*=========================================================================

 Program: MAF2
 Module: mafOpCreateWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateWrappedMeter_H__
#define __mafOpCreateWrappedMeter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEWrappedMeter;
class mafGui;
class mafEvent;

/** 
  class name: mafOpCreateWrappedMeter
  Operation that create a mafVMEWrappedMeter for wrap muscle action lines.
*/
class MAF_EXPORT mafOpCreateWrappedMeter: public mafOp
{
public:
  /** constructor */
  mafOpCreateWrappedMeter(const wxString &label = "CreateWrappedMeter");
  /** destructor */
  ~mafOpCreateWrappedMeter(); 

  /** RTTI macro*/
  mafTypeMacro(mafOpCreateWrappedMeter, mafOp);

  /** clone the object and retrieve a copy*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);
  /** Builds operation's interface. */
  void OpRun();
  /** Execute the operation. */
  void OpDo();

protected: 
  mafVMEWrappedMeter *m_Meter;
};
#endif
