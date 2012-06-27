/*=========================================================================

 Program: MAF2
 Module: mmoTest
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
#endif // __mmoTest_H__








