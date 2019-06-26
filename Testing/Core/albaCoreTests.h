/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaCoreTests_H__
#define __CPP_UNIT_albaCoreTests_H__



//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVME.h"
#include "albaOp.h"
#include "albaTransform.h"


//-------------------------------------------------------------------------
/** class for testing re-parenting. */
//-------------------------------------------------------------------------
class albaVMEHelper : public albaVME
{
public:
	albaTypeMacro(albaVMEHelper, albaVME);
	void SetId(albaID id) { m_Id = id; };

	void SetMatrix(const albaMatrix &mat) { m_Transform->SetMatrix(mat); Modified(); };
	void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes) { kframes.clear(); };
protected:
	albaVMEHelper();
	virtual ~albaVMEHelper() { albaDEL(m_Transform); };

	albaTransform *m_Transform;

};


//----------------------------------------------------------------------------
class DummyObserver : public albaObserver
	//----------------------------------------------------------------------------
{
public:

	DummyObserver() {};
	~DummyObserver();

	virtual void OnEvent(albaEventBase *alba_event);

	albaEvent* GetEvent(int i){return m_ListEvent[i];};

protected:

	std::vector<albaEvent*> m_ListEvent;
};

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class albaOpDummyHelper: public albaOp
//-------------------------------------------------------------------------
{
public:
	albaOpDummyHelper(wxString label = "DummyOp",  bool canundo = false, int opType = OPTYPE_OP, bool inputPreserving = false);
};

//----------------------------------------------------------------------------
bool RemoveDir(const char *dirName);
//----------------------------------------------------------------------------


#endif
