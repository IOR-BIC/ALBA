/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTest.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:46:30 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmoTest_H__
#define __mmoTest_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafNodeGeneric;
// ============================================================================
class mmoTest : public mafOp
// ============================================================================
{
public:
                mmoTest(wxString label);
               ~mmoTest();
	virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  void OpStop(int result);
  
  wxString m_s1;
  wxString m_s2;
  int      m_i;
  float    m_f;
  double   m_d;
  int      m_idx;
  wxColour m_c;
  int      m_iv[3];
  float    m_fv[3];
  double   m_dv[3];
  double   m_dv2[10];

};
#endif // __mmoTest_H__








