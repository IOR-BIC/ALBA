/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputWrappedMeter_h
#define __albaVMEOutputWrappedMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMEOutputPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaGUI;

/** NULL output for VME node with a VTK image output data.
  albaVMEOutputWrappedMeter is the output produced by a node generating an output
  with a VTK dataset.
*/
class ALBA_EXPORT albaVMEOutputWrappedMeter : public albaVMEOutputPolyline
{
public:
  albaVMEOutputWrappedMeter();
  virtual ~albaVMEOutputWrappedMeter();

  albaTypeMacro(albaVMEOutputWrappedMeter,albaVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  albaGUI *CreateGui();

  albaString m_Distance;
  albaString m_Angle;
	std::vector<albaString> m_MiddlePoints;

private:
  albaVMEOutputWrappedMeter(const albaVMEOutputWrappedMeter&); // Not implemented
  void operator=(const albaVMEOutputWrappedMeter&); // Not implemented
};

#endif
