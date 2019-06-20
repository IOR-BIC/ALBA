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

#ifndef __CPP_UNIT_albaIOTests_H__
#define __CPP_UNIT_albaIOTests_H__


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVME.h"
#include "albaObserver.h"

//----------------------------------------------------------------------------
class DummyVme : public albaVME
//----------------------------------------------------------------------------
{
public:
	DummyVme(){};
	~DummyVme(){};

	albaTypeMacro(DummyVme,albaVME);

	/*virtual*/ void SetMatrix(const albaMatrix &mat){};
	/*virtual*/ void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes){};
	/*virtual*/ albaVMEOutput *GetOutput();

protected:
private:
};

//----------------------------------------------------------------------------
class DummyObserver : public albaObserver
	//----------------------------------------------------------------------------
{
public:

	DummyObserver() {m_DummyVme = new DummyVme();};
	~DummyObserver() {delete m_DummyVme;};

	/*virtual*/ void OnEvent(albaEventBase *alba_event);

protected:
	DummyVme *m_DummyVme;
};

#endif
