/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafObserversListTest.cpp,v $
Language:  C++
Date:      $Date: 2008-06-23 13:01:57 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafObserversListTest.h"

#include "mafEventSource.cpp"


//------------------------------------------------------------------------------
void mafObserversListTest::TestStaticAllocation()
//------------------------------------------------------------------------------
{
  mafObserversList observerList;	
}
//------------------------------------------------------------------------------
void mafObserversListTest::TestDynamicAllocation()
//------------------------------------------------------------------------------
{
  mafObserversList *observerList;
  observerList = new mafObserversList;

  delete observerList;
}