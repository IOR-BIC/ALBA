/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTest
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOpTest_H__
#define __albaOpTest_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaNodeGeneric;
// ============================================================================
class albaOpTest : public albaOp
// ============================================================================
{
public:
                albaOpTest(wxString label);
               ~albaOpTest();
	virtual void OnEvent(albaEvent& e);
  albaOp* Copy();

  bool Accept(albaNode* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  void OpStop(int result);
  
  albaString m_s1;
  albaString m_s2;
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
#endif // __albaOpTest_H__








