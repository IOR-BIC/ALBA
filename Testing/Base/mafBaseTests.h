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
#include "mafReferenceCounted.h"



//creation of test object
class mafTestObject: public mafReferenceCounted
{
public:
	mafTestObject():Number(NULL) {};
	~mafTestObject() {if (Number) *Number=0;};
	int  *Number;
	mafTypeMacro(mafTestObject,mafReferenceCounted);
};


//----------------------------------------------------------------------------
class mafFooObject: public mafReferenceCounted
//----------------------------------------------------------------------------
{
public:
	mafTypeMacro(mafFooObject,mafReferenceCounted);
	mafFooObject():Flag(NULL) {}
	~mafFooObject() {if (Flag) *Flag=false;}
	void Print(std::ostream &out) {out<<"Foo";}
	int *Flag;
};


#endif
