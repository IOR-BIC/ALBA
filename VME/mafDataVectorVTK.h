/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataVectorVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:31 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafDataVectorVTK_h
#define __mafDataVectorVTK_h

#include "mafTimeMap.txx"
#include "mafStorable.h"
#include "mafVMEItem.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


/** a dynamic associative sorted array of datasets indexed by their "timestamp".
  
  @todo
  -
*/
class MAF_EXPORT mafDataVectorVTK : public mafDataVector
{
public:
  mafDataVectorVTK();  
  virtual ~mafDataVectorVTK();
protected:
};

#endif
