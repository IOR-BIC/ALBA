/*=========================================================================

 Program: MAF2
 Module: mafPipeRefSys
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeRefSys_H__
#define __mafPipeRefSys_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipeSurface.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mafPipeRefSys :
//----------------------------------------------------------------------------
class MAF_EXPORT mafPipeRefSys : public mafPipeSurface
{
public:
	mafTypeMacro(mafPipeRefSys,mafPipeSurface);

	mafPipeRefSys();

protected:
};  
#endif // __mafPipeRefSys_H__
