/*=========================================================================

 Program: MAF2Medical
 Module: iPoseApp
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __iPoseApp_H__
#define __iPoseApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafLogicWithManagers;

class iPoseApp : public wxApp
{
public:
  bool OnInit();
  int  OnExit();


  ////Called when the application is in the idle state
  //virtual void OnIdle(wxIdleEvent& event);  
  //DECLARE_EVENT_TABLE()

protected:
  mafLogicWithManagers *m_Logic;

};
DECLARE_APP(iPoseApp)
#endif 
