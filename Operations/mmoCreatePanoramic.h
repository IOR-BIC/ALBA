/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreatePanoramic.h,v $
  Language:  C++
  Date:      $Date: 2007-01-19 15:29:03 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCreatePanoramic_H__
#define __mmoCreatePanoramic_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEAdvancedProber;
class mafGui;
class mafEvent;
//----------------------------------------------------------------------------
// mmoCreatePanoramic :
//----------------------------------------------------------------------------
/** */
class mmoCreatePanoramic: public mafOp
{
public:
  mmoCreatePanoramic(wxString label);
 ~mmoCreatePanoramic(); 
  mafOp* Copy();

  bool Accept(mafNode *node);
  void OpRun();
  void OpDo();

protected: 
  mafVMEAdvancedProber *m_Panoramic;
};
#endif
