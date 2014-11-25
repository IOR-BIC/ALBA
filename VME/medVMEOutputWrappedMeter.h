/*=========================================================================

 Program: MAF2Medical
 Module: medVMEOutputWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medVMEOutputWrappedMeter_h
#define __medVMEOutputWrappedMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVMEOutputPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

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
  mafGUI *CreateGui();

  mafString m_Distance;
  mafString m_Angle;
	std::vector<mafString> m_MiddlePoints;

private:
  medVMEOutputWrappedMeter(const medVMEOutputWrappedMeter&); // Not implemented
  void operator=(const medVMEOutputWrappedMeter&); // Not implemented
};

#endif
