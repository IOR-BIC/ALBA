/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: iPoseApp.h,v $
  Language:  C++
  Date:      $Date: 2010-10-20 13:58:36 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
