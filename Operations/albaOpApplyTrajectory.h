/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpApplyTrajectory
 Authors: Roberto Mucci, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpApplyTrajectory_H__
#define __albaOpApplyTrajectory_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// albaOpApplyTrajectory :
//----------------------------------------------------------------------------
/** Import an ACII file containig roto-tranlation information and apply a trajectory to a static VME. 
    Modified 30/11/2010 by Simone Brazzale:
    Added possibility to apply roto-translation from a time-varying VME to a static VME */

class ALBA_EXPORT albaOpApplyTrajectory : public albaOp
{
public:
  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum GUI_IDS
  {
    ID_OPEN_FILE = MINID,
    ID_SELECT_VME,
  };

  /** Contructor. */
	albaOpApplyTrajectory(const wxString &label = "Apply trajectory");

  /** Desctructor. */
	~albaOpApplyTrajectory(); 

  /** type macro for RTTI and instance creation*/
  albaTypeMacro(albaOpApplyTrajectory, albaOp);

  /** Copy the operation. */
	albaOp* Copy();

  /** Returns true for the vme from which the trajectories should be red. (Callback function) */
  static bool AcceptInputVME(albaVME* node);

	/** Builds operation's interface. */
	void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Undo operation. */
  void OpUndo(); 

  /** Stop the operation. */
  void OpStop(int result);

  /** Wait for events */
  void OnEvent(albaEventBase *alba_event);

  /** Create GUI. */
  void CreateGui();

  /** Read the file.
  the format of the file admits some specifics.
  1) Ignore the first line
  2) the first element of each line is Time, then x y z rx ry rz 
  3) the separator is a tab or a blank space
  */
  int Read();

  /** Apply transform from a time-varying VME */
  int ApplyTrajectoriesFromVME();

  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_File = file_name;};

  /** Set the VME from which apply the transformation */
  void SetVME(albaVME* vme){m_VME = vme;};

protected:

	/** Return true for the vme to which the trajectories should be applied. */
	bool InternalAccept(albaVME* vme);

  albaMatrix *m_OriginalMatrix;
  albaString m_FileDir;
	albaString m_File;
  albaVME* m_VME;
};
#endif
