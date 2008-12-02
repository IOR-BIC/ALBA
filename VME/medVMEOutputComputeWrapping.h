/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEOutputComputeWrapping.h,v $
  Language:  C++
  Date:      $Date: 2008-12-02 11:57:18 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Anupam Agrawal and Hui Wei
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEOutputComputeWrapping_h
#define __medVMEOutputComputeWrapping_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

/** NULL output for VME node with a VTK image output data.
  medVMEOutputComputeWrapping is the output produced by a node generating an output
  with a VTK dataset.
*/
class MAF_EXPORT medVMEOutputComputeWrapping : public mafVMEOutputPolyline
{
public:
  medVMEOutputComputeWrapping();
  virtual ~medVMEOutputComputeWrapping();

  mafTypeMacro(medVMEOutputComputeWrapping,mafVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  mafGUI *CreateGui();

  mafString m_Distance;
  mafString m_Angle;
	std::vector<mafString> m_MiddlePoints;

private:
  medVMEOutputComputeWrapping(const medVMEOutputComputeWrapping&); // Not implemented
  void operator=(const medVMEOutputComputeWrapping&); // Not implemented
};

#endif
