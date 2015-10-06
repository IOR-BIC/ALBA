/*=========================================================================

 Program: MAF2
 Module: mafProgressBarHelperTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafProgressBarHelperTest.h"

#include <cppunit/config/SourcePrefix.h>
#include "mafProgressBarHelper.h"

//----------------------------------------------------------------------------
void mafProgressBarHelperTest::BeforeTest()
{
	//Reset All Counters
	m_CloseEventNum=m_InitEventNum=m_UpdateEventNum=0;
}


//----------------------------------------------------------------------------
void mafProgressBarHelperTest::OnEvent(mafEventBase *maf_event)
{
	//Event Management
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
			case PROGRESSBAR_SHOW:
				//Init Event Counter update
				m_InitEventNum++;
			break;
			case PROGRESSBAR_HIDE:
				//Close Event Counter update
				m_CloseEventNum++;
			break;
			case PROGRESSBAR_SET_VALUE:
				//Update Event Counter update
				m_UpdateEventNum++;
			break;
		}
	}
}

//-------------------------------------------------------------------------
void mafProgressBarHelperTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
	//Create and Destroy mafProgressBarHelper object
	mafProgressBarHelper *helper;
	helper = new mafProgressBarHelper(this);
	delete helper;
}

//----------------------------------------------------------------------------
void mafProgressBarHelperTest::InitProgressBarTest()
{
	//Init Test
	mafProgressBarHelper helper(this);

	helper.InitProgressBar();

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==1);
	CPPUNIT_ASSERT(m_UpdateEventNum==0);
	CPPUNIT_ASSERT(m_CloseEventNum==0);
}

//----------------------------------------------------------------------------
void mafProgressBarHelperTest::UpdateProgressBarTest()
{
	mafProgressBarHelper helper(this);

	helper.InitProgressBar();

	CPPUNIT_ASSERT(m_UpdateEventNum==0);
	
	helper.UpdateProgressBar(10);
	CPPUNIT_ASSERT(m_UpdateEventNum==1);

	//An update with the same value should not fire the update event
	helper.UpdateProgressBar(10);
	CPPUNIT_ASSERT(m_UpdateEventNum==1);

	//An update with the higher value should fire the update event
	helper.UpdateProgressBar(15);
	CPPUNIT_ASSERT(m_UpdateEventNum==2);

	//An update with the higher value should fire the update event
	helper.UpdateProgressBar(15);
	CPPUNIT_ASSERT(m_UpdateEventNum==2);

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==1);
	CPPUNIT_ASSERT(m_CloseEventNum==0);
}

//----------------------------------------------------------------------------
void mafProgressBarHelperTest::CloseProgressBarTest()
{
	mafProgressBarHelper helper(this);

	helper.InitProgressBar();
	helper.CloseProgressBar();

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==1);
	CPPUNIT_ASSERT(m_UpdateEventNum==0);
	CPPUNIT_ASSERT(m_CloseEventNum==1);

	//Auto close on destroy test
	mafProgressBarHelper *helper2;
	helper2 = new mafProgressBarHelper(this);
	helper2->InitProgressBar();
	delete helper2;

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==2);
	CPPUNIT_ASSERT(m_UpdateEventNum==0);
	CPPUNIT_ASSERT(m_CloseEventNum==2);
}

//----------------------------------------------------------------------------
void mafProgressBarHelperTest::GetSetTextModeTest()
{
	mafProgressBarHelper helper(this);

	//Default mode should be false
	CPPUNIT_ASSERT(helper.GetTextMode()==false);
		
	//Text mode true test
	helper.SetTextMode(true);
	CPPUNIT_ASSERT(helper.GetTextMode()==true);

	//text mode false test
	helper.SetTextMode(false);
	CPPUNIT_ASSERT(helper.GetTextMode()==false);
}

//----------------------------------------------------------------------------
void mafProgressBarHelperTest::TextModeTest()
{
	//In text mode no events should be fired 
	mafProgressBarHelper helper(this);

	helper.SetTextMode(true);
	helper.InitProgressBar();
	helper.UpdateProgressBar(10);
	helper.CloseProgressBar();

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==0);
	CPPUNIT_ASSERT(m_UpdateEventNum==0);
	CPPUNIT_ASSERT(m_CloseEventNum==0);
}
