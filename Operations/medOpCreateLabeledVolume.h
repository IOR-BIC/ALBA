/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpCreateLabeledVolume.h,v $
  Language:  C++
  Date:      $Date: 2012-04-06 09:09:53 $
  Version:   $Revision: 1.1.2.3 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpCreateLabeledVolume_H__
#define __medOpCreateLabeledVolume_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
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
class MED_OPERATION_EXPORT medOpCreateLabeledVolume: public mafOp
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
