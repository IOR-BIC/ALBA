/*=========================================================================

 Program: MAF2
 Module: testCheckTreeGui
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testCheckTreeGui_H__
#define __testCheckTreeGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGUIGui.h"
#include "mafGUIGuiHolder.h"
#include "mafObserver.h"

// ============================================================================
class testCheckTreeGui : public mafObserver
// ============================================================================
{
public:
                testCheckTreeGui(wxWindow* parent);
  virtual      ~testCheckTreeGui(); 
  virtual void  OnEvent(mafEventBase *maf_event);
  wxWindow*     GetGui();                       
 mafGUIGui         *m_gui;
 mafGUIGuiHolder   *m_guih;
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






