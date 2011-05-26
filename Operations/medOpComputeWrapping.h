/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpComputeWrapping.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:00:03 $
  Version:   $Revision: 1.1.2.3 $
  Authors:   Anupam Agrawal and Hui Wei
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpComputeWrapping_H__
#define __medOpComputeWrapping_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEComputeWrapping;
class mafGui;
class mafEvent;

/** 
  class name: medOpComputeWrapping
  Operation used to create a medVMEComputeWrapping used to wrap muscle action lines.
*/
class MED_EXPORT medOpComputeWrapping: public mafOp
{
public:
  /** constructor */
  medOpComputeWrapping(const wxString &label = "CreateWrappedMeter");
  /** destructor */
  ~medOpComputeWrapping();

  /** RTTI macro*/
  mafTypeMacro(medOpComputeWrapping, mafOp);

  /** clone the object and retrieve a copy*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);
  /** Builds operation's interface. */
  void OpRun();
  /** Execute the operation. */
  void OpDo();

protected: 
  medVMEComputeWrapping *m_Meter;
};
#endif
