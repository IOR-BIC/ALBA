/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testTimeBarApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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




