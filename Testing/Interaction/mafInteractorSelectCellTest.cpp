/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafInteractorSelectCellTest.cpp,v $
Language:  C++
Date:      $Date: 2009-12-17 11:48:24 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
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

#include "mafInteractorSelectCellTest.h"
#include "mafInteractorSelectCell.h"

void mafInteractorSelectCellTest::setUp()
{
 
}

void mafInteractorSelectCellTest::tearDown()
{

}

void mafInteractorSelectCellTest::TestFixture()
{

}

void mafInteractorSelectCellTest::TestConstructor()
{
  mafInteractorSelectCell *selectCell = mafInteractorSelectCell::New();
  selectCell->Delete();
}
