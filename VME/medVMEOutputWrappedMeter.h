/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEOutputWrappedMeter.h,v $
  Language:  C++
  Date:      $Date: 2007-08-20 13:47:36 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEOutputWrappedMeter_h
#define __medVMEOutputWrappedMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmgGui;

/** NULL output for VME node with a VTK image output data.
  medVMEOutputWrappedMeter is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT medVMEOutputWrappedMeter : public mafVMEOutputPolyline
{
public:
  medVMEOutputWrappedMeter();
  virtual ~medVMEOutputWrappedMeter();

  mafTypeMacro(medVMEOutputWrappedMeter,mafVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  mmgGui *CreateGui();

  mafString m_Distance;
  mafString m_Angle;
	std::vector<mafString> m_MiddlePoints;

private:
  medVMEOutputWrappedMeter(const medVMEOutputWrappedMeter&); // Not implemented
  void operator=(const medVMEOutputWrappedMeter&); // Not implemented
};

#endif
