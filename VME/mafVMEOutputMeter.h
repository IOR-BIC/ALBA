/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputMeter.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.5 $
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
class mafGUI;

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

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  mafGUI *CreateGui();

  mafString m_Distance;
  mafString m_Angle;

private:
  mafVMEOutputMeter(const mafVMEOutputMeter&); // Not implemented
  void operator=(const mafVMEOutputMeter&); // Not implemented
};

#endif
