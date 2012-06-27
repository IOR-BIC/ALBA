/*=========================================================================

 Program: MAF2
 Module: testSideBarGui
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testSideBarGui_H__
#define __testSideBarGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUIHolder.h"
#include "mafObserver.h"
// ============================================================================
class testSideBarGui : public mafObserver
// ============================================================================
{
public:
                testSideBarGui(wxWindow* parent);
  virtual      ~testSideBarGui(); 
  virtual void  OnEvent(mafEventBase *maf_event);
  wxWindow*     GetGui();                       
 mafGUI         *m_gui;
 mafGUIHolder   *m_guih;
protected:

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

#endif // __testSideBarGui_H__




