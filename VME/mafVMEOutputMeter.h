/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputMeter.h,v $
  Language:  C++
  Date:      $Date: 2009-02-02 10:23:05 $
  Version:   $Revision: 1.5.2.1 $
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

	/* Get Distance String*/
	const char *GetDistance(){return m_Distance.GetCStr();}

	/* Get Angle String*/
	const char *GetAngle(){return m_Angle.GetCStr();}

protected:
  mafGUI *CreateGui();

  mafString m_Distance;
  mafString m_Angle;

private:
  mafVMEOutputMeter(const mafVMEOutputMeter&); // Not implemented
  void operator=(const mafVMEOutputMeter&); // Not implemented
};

#endif
