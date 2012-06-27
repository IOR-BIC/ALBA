/*=========================================================================

 Program: MAF2
 Module: mafNodeGeneric
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafNodeGeneric.h"
#include "mafAttribute.h"
#include "mafIndent.h"
#include "mafStorage.h"
#include "mafEventIO.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeGeneric)
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
char** mafNodeGeneric::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafNodeGeneric.xpm"
  return mafNodeGeneric_xpm;
}
