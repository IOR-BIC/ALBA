/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEOutputComputeWrapping.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:30:59 $
  Version:   $Revision: 1.1.2.3 $
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
#include "medDefines.h"
#include "mafVMEOutputPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

/** 
  class name: medVMEOutputComputeWrapping
  NULL output for VME node with a VTK image output data.
  medVMEOutputComputeWrapping is the output produced by a node generating an output
  with a VTK dataset.
*/
class MED_EXPORT medVMEOutputComputeWrapping : public mafVMEOutputPolyline
{
public:
  /** constructor */
  medVMEOutputComputeWrapping();
  /** destructor */
  virtual ~medVMEOutputComputeWrapping();

  /** RTTI macro*/
  mafTypeMacro(medVMEOutputComputeWrapping,mafVMEOutputPolyline);

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

protected:
  /** create the gui of the output panel for this vme output type*/
  mafGUI *CreateGui();

  mafString m_Distance;
  mafString m_Angle;
  std::vector<mafString> m_MiddlePoints;

private:
  /** copy constructor not implemented */
  medVMEOutputComputeWrapping(const medVMEOutputComputeWrapping&);
  /** operator = not implemented */
  void operator=(const medVMEOutputComputeWrapping&); // Not implemented
};

#endif
