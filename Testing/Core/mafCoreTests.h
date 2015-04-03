/*=========================================================================

 Program: MAF2
 Module: mafWizardTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafCoreTests_H__
#define __CPP_UNIT_mafCoreTests_H__



//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafNode.h"
#include "mafOp.h"


// Helper class used to build a fake application needed by the wxConfig
// to store settings into the registry with the same name of the application
// ===============================================================================
class TestApp : public wxApp
	// ===============================================================================
{
public:
	bool OnInit();
	int  OnExit();
};
DECLARE_APP(TestApp)

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
//-------------------------------------------------------------------------
class mafNodeHelper: public mafNode
{
public:
	mafTypeMacro(mafNodeHelper,mafNode);
	void SetId(mafID id) {m_Id = id;};
};

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class mafNodeBHelper: public mafNode
	//-------------------------------------------------------------------------
{
public:
	mafTypeMacro(mafNodeBHelper,mafNode);
};

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
	//----------------------------------------------------------------------------
{
public:

	DummyObserver() {};
	~DummyObserver();

	virtual void OnEvent(mafEventBase *maf_event);

	mafEvent* GetEvent(int i){return m_ListEvent[i];};

protected:

	std::vector<mafEvent*> m_ListEvent;
};

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class mafOpDummyHelper: public mafOp
//-------------------------------------------------------------------------
{
public:
	mafOpDummyHelper(wxString label = "DummyOp",  bool canundo = false, int opType = OPTYPE_OP, bool inputPreserving = false);
};

//----------------------------------------------------------------------------
bool RemoveDir(const char *dirName);
//----------------------------------------------------------------------------


#endif
