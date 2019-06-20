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

#include "albaAttribute.h"
#include "vtkALBASmartPointer.h"
#include "vtkAssembly.h"

/** attribute class for attaching vtkActor to VME */
class albaClientData : public albaAttribute
{
public:
	albaTypeMacro(albaClientData,albaAttribute);

	vtkALBAAutoPointer<vtkAssembly> m_Prop3D;

	virtual void DeepCopy(const albaAttribute *a) {Superclass::DeepCopy(a); m_Prop3D=((albaClientData *)a)->m_Prop3D;}
	virtual bool Equals(const albaAttribute *a) const {return Superclass::Equals(a)&&m_Prop3D==((albaClientData *)a)->m_Prop3D;}
};

#endif
