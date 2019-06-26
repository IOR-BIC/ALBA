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
#include "albaReferenceCounted.h"



//creation of test object
class albaTestObject: public albaReferenceCounted
{
public:
	albaTestObject():Number(NULL) {};
	~albaTestObject() {if (Number) *Number=0;};
	int  *Number;
	albaTypeMacro(albaTestObject,albaReferenceCounted);
};


//----------------------------------------------------------------------------
class albaFooObject: public albaReferenceCounted
//----------------------------------------------------------------------------
{
public:
	albaTypeMacro(albaFooObject,albaReferenceCounted);
	albaFooObject():Flag(NULL) {}
	~albaFooObject() {if (Flag) *Flag=false;}
	void Print(std::ostream &out) {out<<"Foo";}
	int *Flag;
};


#endif
