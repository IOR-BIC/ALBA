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

#ifndef __CPP_UNIT_mafIOTests_H__
#define __CPP_UNIT_mafIOTests_H__


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVME.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
class DummyVme : public mafVME
//----------------------------------------------------------------------------
{
public:
	DummyVme(){};
	~DummyVme(){};

	mafTypeMacro(DummyVme,mafVME);

	/*virtual*/ void SetMatrix(const mafMatrix &mat){};
	/*virtual*/ void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};
	/*virtual*/ mafVMEOutput *GetOutput();

protected:
private:
};

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
	//----------------------------------------------------------------------------
{
public:

	DummyObserver() {m_DummyVme = new DummyVme();};
	~DummyObserver() {delete m_DummyVme;};

	/*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
	DummyVme *m_DummyVme;
};

#endif
