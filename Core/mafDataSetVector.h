/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataSetVector.h,v $
  Language:  C++
  Date:      $Date: 2005-03-02 00:32:21 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafDataSetVector_h
#define __mafDataSetVector_h

#include "mafTimeMap.txx"
#include "mafDataSet.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

/** a dynamic associative sorted array of datasets indexed by their "timestamp".
  
  @todo
  -
*/

class MAF_EXPORT mafDataSetVector : public mafTimeMap<mafDataSet>
{
public:
  mafDataSetVector();  
  virtual ~mafDataSetVector();

  // set as parent mafObject since type macro does not work for template classes
  mafTypeMacro(mafDataSetVector,mafObject); 
  
protected:
};

#endif
