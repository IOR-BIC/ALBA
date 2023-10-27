/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProgressBarHelperTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>
#include "albaProgressBarHelperTest.h"
#include "albaProgressBarHelper.h"

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::BeforeTest()
{
	//Reset All Counters
	m_CloseEventNum=m_InitEventNum=m_UpdateEventNum=0;
}


//----------------------------------------------------------------------------
void albaProgressBarHelperTest::OnEvent(albaEventBase *alba_event)
{
	//Event Management
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
void albaProgressBarHelperTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
	//Create and Destroy albaProgressBarHelper object
	albaProgressBarHelper *helper;
	helper = new albaProgressBarHelper(this);
	delete helper;
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::InitProgressBarTest()
{
	//Init Test
	albaProgressBarHelper helper(this);

	helper.InitProgressBar("",false);

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==1);
	CPPUNIT_ASSERT(m_UpdateEventNum==0);
	CPPUNIT_ASSERT(m_CloseEventNum==0);

	//Double Init
	helper.InitProgressBar("",false);

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==2);
	CPPUNIT_ASSERT(m_UpdateEventNum==1);
	CPPUNIT_ASSERT(m_CloseEventNum==1);
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::UpdateProgressBarTest()
{
	albaProgressBarHelper helper(this);

	helper.InitProgressBar("",false);

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
void albaProgressBarHelperTest::CloseProgressBarTest()
{
	albaProgressBarHelper helper(this);

	helper.InitProgressBar("",false);
	helper.CloseProgressBar();

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==1);
	CPPUNIT_ASSERT(m_UpdateEventNum==1);
	CPPUNIT_ASSERT(m_CloseEventNum==1);

	//Auto close on destroy test
	albaProgressBarHelper *helper2;
	helper2 = new albaProgressBarHelper(this);
	helper2->InitProgressBar("",false);
	delete helper2;

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==2);
	CPPUNIT_ASSERT(m_UpdateEventNum=2);
	CPPUNIT_ASSERT(m_CloseEventNum==2);
}

//----------------------------------------------------------------------------
void albaProgressBarHelperTest::GetSetTextModeTest()
{
	albaProgressBarHelper helper(this);

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
void albaProgressBarHelperTest::TextModeTest()
{
	//In text mode no events should be fired 
	albaProgressBarHelper helper(this);

	helper.SetTextMode(true);
	helper.InitProgressBar("",false);
	helper.UpdateProgressBar(10);
	helper.CloseProgressBar();

	//Check event counters
	CPPUNIT_ASSERT(m_InitEventNum==0);
	CPPUNIT_ASSERT(m_UpdateEventNum==0);
	CPPUNIT_ASSERT(m_CloseEventNum==0);
}
