/*=========================================================================

 Program: MAF2
 Module: mafPipeSurface
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeSurface_H__
#define __mafPipeSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipeGenericPolydata.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class MAF_EXPORT mafPipeSurface : public mafPipeGenericPolydata
{
public:
	mafTypeMacro(mafPipeSurface, mafPipeGenericPolydata);

	mafPipeSurface();
	virtual     ~mafPipeSurface();

protected:

	vtkPolyData* GetInputAsPolyData();
};  
#endif // __mafPipeSurface_H__
