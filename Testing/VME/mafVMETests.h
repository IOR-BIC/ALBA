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

#include "mafAttribute.h"
#include "vtkMAFSmartPointer.h"
#include "vtkAssembly.h"

/** attribute class for attaching vtkActor to VME */
class mafClientData : public mafAttribute
{
public:
	mafTypeMacro(mafClientData,mafAttribute);

	vtkMAFAutoPointer<vtkAssembly> m_Prop3D;

	virtual void DeepCopy(const mafAttribute *a) {Superclass::DeepCopy(a); m_Prop3D=((mafClientData *)a)->m_Prop3D;}
	virtual bool Equals(const mafAttribute *a) const {return Superclass::Equals(a)&&m_Prop3D==((mafClientData *)a)->m_Prop3D;}
};


#endif
