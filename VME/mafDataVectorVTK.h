/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataVectorVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-07-08 16:32:28 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafDataVectorVTK_h
#define __mafDataVectorVTK_h

#include "mafDataVector.h"
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
