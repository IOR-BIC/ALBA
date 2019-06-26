/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testCheckTreeGui
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testCheckTreeGui_H__
#define __testCheckTreeGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGUIGui.h"
#include "albaGUIGuiHolder.h"
#include "albaObserver.h"

// ============================================================================
class testCheckTreeGui : public albaObserver
// ============================================================================
{
public:
                testCheckTreeGui(wxWindow* parent);
  virtual      ~testCheckTreeGui(); 
  virtual void  OnEvent(albaEventBase *alba_event);
  wxWindow*     GetGui();                       
 albaGUIGui         *m_gui;
 albaGUIGuiHolder   *m_guih;
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






