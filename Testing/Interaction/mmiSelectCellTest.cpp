/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiSelectCellTest.cpp,v $
Language:  C++
Date:      $Date: 2007-03-20 21:25:23 $
Version:   $Revision: 1.1 $
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

#include "mmiSelectCellTest.h"
#include "mmiSelectCell.h"

void mmiSelectCellTest::setUp()
{
 
}

void mmiSelectCellTest::tearDown()
{

}

void mmiSelectCellTest::TestFixture()
{

}

void mmiSelectCellTest::TestConstructor()
{
  mmiSelectCell *selectCell = mmiSelectCell::New();
  selectCell->Delete();
}
