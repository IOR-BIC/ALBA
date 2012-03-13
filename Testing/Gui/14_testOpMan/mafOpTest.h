/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpTest.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:00:04 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafOpTest_H__
#define __mafOpTest_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafNodeGeneric;
// ============================================================================
class mafOpTest : public mafOp
// ============================================================================
{
public:
                mafOpTest(wxString label);
               ~mafOpTest();
	virtual void OnEvent(mafEvent& e);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  void OpStop(int result);
  
  mafString m_s1;
  mafString m_s2;
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
#endif // __mafOpTest_H__








