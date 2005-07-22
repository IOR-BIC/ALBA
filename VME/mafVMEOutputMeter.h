/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputMeter.h,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:49:52 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputMeter_h
#define __mafVMEOutputMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmgGui;

/** NULL output for VME node with a VTK image output data.
  mafVMEOutputMeter is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputMeter : public mafVMEOutputPolyline
{
public:
  mafVMEOutputMeter();
  virtual ~mafVMEOutputMeter();

  mafTypeMacro(mafVMEOutputMeter,mafVMEOutputPolyline);

protected:
  mmgGui *CreateGui();

  mafString m_Distance;

private:
  mafVMEOutputMeter(const mafVMEOutputMeter&); // Not implemented
  void operator=(const mafVMEOutputMeter&); // Not implemented
};

#endif
