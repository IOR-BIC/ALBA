/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecomposeTimeVarVME
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpDecomposeTimeVarVME_H__
#define __albaOpDecomposeTimeVarVME_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class wxListBox;
class albaVME;
class albaGUIRollOut;
class albaVMEGroup;
class albaVMELandmarkCloud;
class albaGUI;
class albaEvent;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaString);
EXPORT_STL_VECTOR(ALBA_EXPORT,albaVMELandmarkCloud*);
EXPORT_STL_VECTOR(ALBA_EXPORT,albaVME*);
#endif

//----------------------------------------------------------------------------
// albaOpRefSys :
//----------------------------------------------------------------------------
/** Operation to create static VMEs from a time varing VME. User can choose single timestamps,
    timestamp interval, or set a periodicity by means extract static VMEs */
class ALBA_EXPORT albaOpDecomposeTimeVarVME: public albaOp
{
public:
  albaOpDecomposeTimeVarVME(const wxString& label = "Decompose time varying VME");
 ~albaOpDecomposeTimeVarVME(); 

  virtual void OnEvent(albaEventBase *alba_event);
  albaOp* Copy();

  void OpRun();

  /** Get the number of frames inserted by the user and for each
       call CreateStaticVME(). */
  int UpdateFrames();

  /** Append a frame to the listbox */
  void AppendFrame(char *string);

  /** Delete a frame from the listbox */
  void DeleteFrame(int frameId);

  /** Select the operation mode:
  MODE_NONE = 0,
  MODE_FRAMES = 1,
  MODE_INTERVAL = 2,
  MODE_PERIODICITY = 3.*/
  void SelectMode(int mode);

  /** Set the timestamps interval. */
  void SetInterval(double from, double to);

  /** Set the period used for periodicity mode */
  void SetPeriodicity(int period);

  /** Create a static VME from a particular timestamp of the input VME. */
  void CreateStaticVME(albaTimeStamp time);

  /** Enable Ok widget */
  void EnableWidget(bool enable);

  enum DECOMPOSE_MODE_WIDGET_ID
  {
    ID_LIST_FRAMES = MINID,
    ID_INSERT_FRAME,
    ID_REMOVE_FRAME,
    ID_FRAME,
    CHANGE_VALUE_FRAMES,
    CHANGE_VALUE_INTERVAL,
    CHANGE_VALUE_PERIODICITY,

    ID_ROLLOUT_FRAMES,
    ID_ROLLOUT_INTERVAL,
    ID_ROLLOUT_PERIODICITY,
  };

  enum DECOMPOSE_MODE_TYPE_ID
  {
    MODE_NONE = 0,
    MODE_FRAMES,
    MODE_INTERVAL,
    MODE_PERIODICITY,
  };

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Additional Gui. */
  void ModeGui();

  albaGUIRollOut *m_RollOutFrames;
  albaGUIRollOut *m_RollOutInterval;
  albaGUIRollOut *m_RollOutPeriodicity;

private:
  albaGUI *m_GuiFrames;
  albaGUI *m_GuiInterval; 
  albaGUI *m_GuiPeriodicity; 
  albaVMEGroup         *m_Group;
  albaVMELandmarkCloud *m_Cloud;

  int m_InsertMode;
  int m_Periodicity;
  int m_NumberFrames;

  double m_IntervalFrom;
  double m_IntervalTo;
  double m_Frame;

  std::vector<albaVME*> m_VectorVME;
  std::vector<albaVMELandmarkCloud*> m_VectorCloud;
  std::vector<albaString> m_FrameLabel;

  wxListBox *m_FramesListBox; 
  long m_ItemId;
};
#endif
