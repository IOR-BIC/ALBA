/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewCompoundWindowingTest.cpp,v $
Language:  C++
Date:      $Date: 2010-12-15 15:26:47 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafGUI.h"
#include "medViewCompoundWindowing.h"
#include "medViewCompoundWindowingTest.h"


class dummyView;

class dummyView: public medViewCompoundWindowing
{
public:
	dummyView(){};
	~dummyView(){};

	mafTypeMacro(dummyView, medViewCompoundWindowing);

	void PackageView(){};
	mafView* Copy(mafObserver *Listener,bool lightCopyEnabled = false){return NULL;};

protected:
	mafGUI* CreateGui(){return NULL;};
};

mafCxxTypeMacro(dummyView);

//-------------------------------------------------------------------------
void medViewCompoundWindowingTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  dummyView *sscView = new dummyView();
  sscView->PackageView();
  sscView->Delete();
   
  //check leaks
  CPPUNIT_ASSERT(true);
 
}