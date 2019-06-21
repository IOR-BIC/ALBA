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
#include "albaVMESurface.h"

/** a vme test  class used to test event reception. */
class albaVMESurfaceTestClass: public albaVMESurface
{
public:
	albaVMESurfaceTestClass(const char *name=NULL):Name(name) {}
	albaTypeMacro(albaVMESurfaceTestClass,albaVMESurface);

	virtual void OnEvent(albaEventBase *event) {Name = "CATCHED";};

	albaString     Name;
};

#endif
