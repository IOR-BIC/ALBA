/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputMeter
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputMeter_h
#define __albaVMEOutputMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaGUI;

/** NULL output for VME node with a VTK image output data.
  albaVMEOutputMeter is the output produced by a node generating an output
  with a VTK dataset.
*/
class ALBA_EXPORT albaVMEOutputMeter : public albaVMEOutputPolyline
{
public:
  albaVMEOutputMeter();
  virtual ~albaVMEOutputMeter();

  albaTypeMacro(albaVMEOutputMeter,albaVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

	/* Get Distance String*/
	const char *GetDistance(){return m_Distance.GetCStr();}

	/* Get Angle String*/
	const char *GetAngle(){return m_Angle.GetCStr();}

protected:
  albaGUI *CreateGui();

  albaString m_Distance;
  albaString m_Angle;

private:
  albaVMEOutputMeter(const albaVMEOutputMeter&); // Not implemented
  void operator=(const albaVMEOutputMeter&); // Not implemented
};

#endif
