/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurface
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeSurface_H__
#define __albaPipeSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipeGenericPolydata.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkTrivialProducer;

//----------------------------------------------------------------------------
// albaPipeSurface :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeSurface : public albaPipeGenericPolydata
{
public:
	albaTypeMacro(albaPipeSurface, albaPipeGenericPolydata);

	albaPipeSurface();
	virtual     ~albaPipeSurface();

protected:

	vtkAlgorithmOutput* GetPolyDataOutputPort();

	vtkTrivialProducer *m_TrivialProd;
};  
#endif // __albaPipeSurface_H__
