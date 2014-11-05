/*=========================================================================

 Program: MAF2Medical
 Module: exMedicalApp
 Authors: Matteo Giacomoni - Daniele Giunchi - Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __exMedicalApp_H__
#define __exMedicalApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medLogicWithManagers.h" 

// ============================================================================
class exMedicalApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();
protected:
  medLogicWithManagers *m_Logic;

	void OnFatalException();
};
DECLARE_APP(exMedicalApp)
#endif //__exMedicalApp_H__
