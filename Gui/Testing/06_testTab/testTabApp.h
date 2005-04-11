/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTabApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:37 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testTabApp_H__
#define __testTabApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testTabLogic.h" 

/*
risultato del test :
i tab sembrano funzionare bene -- ad eccezzione del colore di sfondo

riguardo alle gui ci sono 2 problemi
1)la parte sotto alle GUI - flickera, a volte si vede il guiholder, a volte si vede qualcosa d'altro (di un altro colore)
2)restringendo in orizzontale la GUI a volte si invalida la scrollbar verticale 

il problema non si pone quando uso la SideBar - perche non si ridimensiona in orizzontale

*/

// ============================================================================
class testTabApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testTabLogic *m_logic;

};

DECLARE_APP(testTabApp)

#endif //__testTabApp_H__


