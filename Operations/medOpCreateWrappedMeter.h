/*=========================================================================

 Program: MAF2
 Module: medOpCreateWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpCreateWrappedMeter_H__
#define __medOpCreateWrappedMeter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEWrappedMeter;
class mafGui;
class mafEvent;

/** 
  class name: medOpCreateWrappedMeter
  Operation that create a medVMEWrappedMeter for wrap muscle action lines.
*/
class MAF_EXPORT medOpCreateWrappedMeter: public mafOp
{
public:
  /** constructor */
  medOpCreateWrappedMeter(const wxString &label = "CreateWrappedMeter");
  /** destructor */
  ~medOpCreateWrappedMeter(); 

  /** RTTI macro*/
  mafTypeMacro(medOpCreateWrappedMeter, mafOp);

  /** clone the object and retrieve a copy*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);
  /** Builds operation's interface. */
  void OpRun();
  /** Execute the operation. */
  void OpDo();

protected: 
  medVMEWrappedMeter *m_Meter;
};
#endif
