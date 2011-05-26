/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpCreateWrappedMeter.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:00:03 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpCreateWrappedMeter_H__
#define __medOpCreateWrappedMeter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
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
class MED_EXPORT medOpCreateWrappedMeter: public mafOp
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
