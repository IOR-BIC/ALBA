/*=========================================================================

 Program: MAF2
 Module: mafOpTest
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class __declspec( dllexport ) mafOpTest : public mafOp
// ============================================================================
{
public:
                mafOpTest(wxString label = "Test");
               ~mafOpTest();
	virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();
  mafTypeMacro(mafOpTest, mafOp);

  bool Accept(mafNode* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  void OpStop(int result);
  
  mafString m_s1;
  mafString m_s2;
  mafString m_string1;
  mafString m_string2;
  mafString m_sopen;
  mafString m_ssave;
  mafString m_sdir;
  mafString m_ButtonString;
  int      m_i1;
  int      m_i2;
  int      m_bool1;
  int      m_bool2;
  int      m_sliderInt1;
  int      m_sliderInt2;
  float    m_f;
  double   m_d1;
  double   m_d2;
  int      m_idx;
  wxColour m_c;
  int      m_iv[3];
  int      m_iv1[3];
  int      m_iv2[3];
  int      m_iv3[3];
  float    m_fv[3];
  double   m_dv[3];
};
#endif // __mafOpTest_H__
