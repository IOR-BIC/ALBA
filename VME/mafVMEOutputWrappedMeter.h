/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEOutputWrappedMeter_h
#define __mafVMEOutputWrappedMeter_h
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
  mafVMEOutputWrappedMeter is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT mafVMEOutputWrappedMeter : public mafVMEOutputPolyline
{
public:
  mafVMEOutputWrappedMeter();
  virtual ~mafVMEOutputWrappedMeter();

  mafTypeMacro(mafVMEOutputWrappedMeter,mafVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  mafGUI *CreateGui();

  mafString m_Distance;
  mafString m_Angle;
	std::vector<mafString> m_MiddlePoints;

private:
  mafVMEOutputWrappedMeter(const mafVMEOutputWrappedMeter&); // Not implemented
  void operator=(const mafVMEOutputWrappedMeter&); // Not implemented
};

#endif
