/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testSideBarGui
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testSideBarGui_H__
#define __testSideBarGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIHolder.h"
#include "albaObserver.h"
// ============================================================================
class testSideBarGui : public albaObserver
// ============================================================================
{
public:
                testSideBarGui(wxWindow* parent);
  virtual      ~testSideBarGui(); 
  virtual void  OnEvent(albaEventBase *alba_event);
  wxWindow*     GetGui();                       
 albaGUI         *m_gui;
 albaGUIHolder   *m_guih;
protected:

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

#endif // __testSideBarGui_H__




