/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEEmg.h,v $
  Language:  C++
  Date:      $Date: 2007-07-04 13:44:37 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEEmg_h
#define __medVMEEmg_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEScalar.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEOutputScalar;


/** VME child of mafVMEScalar, for EMG data. Refactor CreateGui and Print methods not to show parameters used for VTK representation */
class MAF_EXPORT medVMEEmg : public mafVMEScalar
{
public:
  mafTypeMacro(medVMEEmg, mafVMEScalar);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

protected:
  medVMEEmg();
  virtual ~medVMEEmg();

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

};
#endif
