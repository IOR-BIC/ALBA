/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testCheckTreeApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-12 14:00:07 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testCheckTreeApp_H__
#define __testCheckTreeApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testCheckTreeLogic.h" 

/*
risultato del test :
i tab sembrano funzionare bene -- ad eccezzione del colore di sfondo

riguardo alle gui ci sono 2 problemi
1)la parte sotto alle GUI - flickera, a volte si vede il guiholder, a volte si vede qualcosa d'altro (di un altro colore)
2)restringendo in orizzontale la GUI a volte si invalida la scrollbar verticale 

*/

// ============================================================================
class testCheckTreeApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testCheckTreeLogic *m_logic;

};

DECLARE_APP(testCheckTreeApp)

#endif //__testCheckTreeApp_H__





