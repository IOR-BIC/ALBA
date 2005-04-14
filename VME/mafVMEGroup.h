/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGroup.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 18:16:22 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEGroup_h
#define __mafVMEGroup_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGenericAbstract.h"
#include "mafVMEOutputNULL.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** mafVMEGroup - a VME use to create hierarchical assemblies of VME's.
  mafVMEGroup is a specialized VME inheriting the VME-Generic features to internally
  store pose matrices, used to create assemblies of VMEs and direct cinematic. 
  @sa mafVME mafMatrixVector */
class MAF_EXPORT mafVMEGroup : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEGroup,mafVMEGenericAbstract);

  /** Return the right type of output.*/  
  mafVMEOutputNULL *GetVTKOutput() {return (mafVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  virtual mafVMEOutput *GetOutput();
  
protected:
  mafVMEGroup();
  virtual ~mafVMEGroup();

private:
  mafVMEGroup(const mafVMEGroup&); // Not implemented
  void operator=(const mafVMEGroup&); // Not implemented
};

#endif
