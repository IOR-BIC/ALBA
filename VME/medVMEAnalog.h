/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEAnalog.h,v $
  Language:  C++
  Date:      $Date: 2008-01-09 10:18:52 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEAnalog_h
#define __medVMEAnalog_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEScalarMatrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEOutputScalarMatrix;


/** VME child of mafVMEScalarMatrix, for EMG data. Refactor CreateGui and Print methods not to show parameters used for VTK representation */
class MAF_EXPORT medVMEAnalog : public mafVMEScalarMatrix
{
public:
  mafTypeMacro(medVMEAnalog, mafVMEScalarMatrix);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

protected:
  medVMEAnalog();
  virtual ~medVMEAnalog();

  bool IsAnimated();

  void GetTimeBounds(mafTimeStamp tbounds[2]); 

  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** Internally used to create a new instance of the GUI.*/
  virtual mmgGui *CreateGui();

  mafTimeStamp    m_CurrentTime;  ///< the time parameter for generation of the output

};
#endif
