/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testCheckTreeGui.h,v $
  Language:  C++
  Date:      $Date: 2005-04-12 14:00:08 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testCheckTreeGui_H__
#define __testCheckTreeGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mmgGui.h"
#include "mmgGuiHolder.h"
// ============================================================================
class testCheckTreeGui : public mafEventListener
// ============================================================================
{
public:
                testCheckTreeGui(wxWindow* parent);
  virtual      ~testCheckTreeGui(); 
  virtual void  OnEvent(mafEvent& e);
  wxWindow*     GetGui();                       
 mmgGui         *m_gui;
 mmgGuiHolder   *m_guih;
protected:

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

#endif // __testCheckTreeGui_H__






