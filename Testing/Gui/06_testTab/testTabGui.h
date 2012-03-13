/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTabGui.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testTabGui_H__
#define __testTabGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUIHolder.h"
#include "mafObserver.h"

// ============================================================================
class testTabGui : public mafObserver
// ============================================================================
{
public:
                testTabGui(wxWindow* parent);
  virtual      ~testTabGui(); 
  virtual void  OnEvent(mafEventBase *maf_event);
  wxWindow*     GetGui();                       
protected:
 mafGUI         *m_gui;
 mafGUIHolder   *m_guih;

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

#endif // __testTabGui_H__



