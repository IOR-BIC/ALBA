/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGui.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:48 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testGui_H__
#define __testGui_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mmgGui.h"
#include "mmgGuiHolder.h"
// ============================================================================
class testGui : public mafEventListener
// ============================================================================
{
public:
                testGui(wxWindow* parent);
  virtual      ~testGui(); 
  virtual void  OnEvent(mafEvent& e);
  wxWindow*     GetGui();                       
protected:
 mmgGui         *m_gui;
 mmgGuiHolder   *m_guih;

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

#endif // __testGui_H__


