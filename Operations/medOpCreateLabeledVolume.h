/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpCreateLabeledVolume.h,v $
  Language:  C++
  Date:      $Date: 2007-10-24 08:48:16 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpCreateLabeledVolume_H__
#define __medOpCreateLabeledVolume_H__

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
/** */
class medOpCreateLabeledVolume: public mafOp
{
public:
  medOpCreateLabeledVolume(const wxString &label = "Create labeled volume");
  ~medOpCreateLabeledVolume(); 

  mafTypeMacro(medOpCreateLabeledVolume, mafOp);

  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  medVMELabeledVolume *m_LabeledVolume;
};
#endif
