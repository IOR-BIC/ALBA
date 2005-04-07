/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTreeApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 18:37:23 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testTreeApp_H__
#define __testTreeApp_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "testTreeLogic.h" 

/*
risultato del test :
i tab sembrano funzionare bene -- ad eccezzione del colore di sfondo

riguardo alle gui ci sono 2 problemi
1)la parte sotto alle GUI - flickera, a volte si vede il guiholder, a volte si vede qualcosa d'altro (di un altro colore)
2)restringendo in orizzontale la GUI a volte si invalida la scrollbar verticale 

*/

// ============================================================================
class testTreeApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testTreeLogic *m_logic;

};

DECLARE_APP(testTreeApp)

#endif //__testTreeApp_H__




