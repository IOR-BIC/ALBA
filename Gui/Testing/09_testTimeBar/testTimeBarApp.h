/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTimeBarApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:25:07 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testTimeBarApp_H__
#define __testTimeBarApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testTimeBarLogic.h" 
/*
risultato del test :
i tab sembrano funzionare bene -- ad eccezzione del colore di sfondo

riguardo alle gui ci sono 2 problemi
1)la parte sotto alle GUI - flickera, a volte si vede il guiholder, a volte si vede qualcosa d'altro (di un altro colore)
2)restringendo in orizzontale la GUI a volte si invalida la scrollbar verticale 

*/

// ============================================================================
class testTimeBarApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testTimeBarLogic *m_logic;

};

DECLARE_APP(testTimeBarApp)

#endif //__testTimeBarApp_H__




