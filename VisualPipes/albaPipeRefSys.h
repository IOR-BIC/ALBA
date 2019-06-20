/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeRefSys
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeRefSys_H__
#define __albaPipeRefSys_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipeSurface.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// albaPipeRefSys :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeRefSys : public albaPipeSurface
{
public:
	albaTypeMacro(albaPipeRefSys,albaPipeSurface);

	albaPipeRefSys();

protected:
};  
#endif // __albaPipeRefSys_H__
