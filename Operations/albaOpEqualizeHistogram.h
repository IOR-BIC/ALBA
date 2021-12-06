/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEqualizeHistogram
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpEqualizeHistogram_H__
#define __albaOpEqualizeHistogram_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEVolumeGray;
class albaGUIDialog;
class albaGUIHistogramWidget;

/** 
class name : albaOpEqualizeHistogram
*/
class ALBA_EXPORT albaOpEqualizeHistogram: public albaOp
{
public:

  /** constructor. */
  albaOpEqualizeHistogram(const wxString &label = "Equalize Histogram");
  /** destructor. */
  ~albaOpEqualizeHistogram(); 

  /** Precess events coming from other objects */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** RTTI macro */
  albaTypeMacro(albaOpEqualizeHistogram, albaOp);

  /*virtual*/ albaOp* Copy();

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);

  /** Perform the equalization of the histogram */
  void Algorithm();

  /** Create the operation gui */
  void CreateGui();

  void CreateHistogramDialog();
  void DeleteHistogramDialog();

  double m_Alpha;
  double m_Beta;
  int m_Radius[3];

  albaVMEVolumeGray *m_VolumeInput;
  albaVMEVolumeGray *m_VolumeOutput;

  albaGUIDialog *m_Dialog;
  albaGUIHistogramWidget *m_Histogram;


};
#endif
