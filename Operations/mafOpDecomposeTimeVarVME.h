/*=========================================================================

 Program: MAF2
 Module: mafOpDecomposeTimeVarVME
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpDecomposeTimeVarVME_H__
#define __mafOpDecomposeTimeVarVME_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class wxListBox;
class mafVME;
class mafGUIRollOut;
class mafVMEGroup;
class mafVMELandmarkCloud;
class mafGUI;
class mafEvent;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafString);
EXPORT_STL_VECTOR(MAF_EXPORT,mafVMELandmarkCloud*);
EXPORT_STL_VECTOR(MAF_EXPORT,mafVME*);
#endif

//----------------------------------------------------------------------------
// mafOpRefSys :
//----------------------------------------------------------------------------
/** Operation to create static VMEs from a time varing VME. User can choose single timestamps,
    timestamp interval, or set a periodicity by means extract static VMEs */
class MAF_EXPORT mafOpDecomposeTimeVarVME: public mafOp
{
public:
  mafOpDecomposeTimeVarVME(const wxString& label = "Decompose time varying VME");
 ~mafOpDecomposeTimeVarVME(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);  

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
  void CreateStaticVME(mafTimeStamp time);

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
  /** Additional Gui. */
  void ModeGui();

  mafGUIRollOut *m_RollOutFrames;
  mafGUIRollOut *m_RollOutInterval;
  mafGUIRollOut *m_RollOutPeriodicity;

private:
  mafGUI *m_GuiFrames;
  mafGUI *m_GuiInterval; 
  mafGUI *m_GuiPeriodicity; 
  mafVMEGroup         *m_Group;
  mafVMELandmarkCloud *m_Cloud;

  int m_InsertMode;
  int m_Periodicity;
  int m_NumberFrames;

  double m_IntervalFrom;
  double m_IntervalTo;
  double m_Frame;

  std::vector<mafVME*> m_VectorVME;
  std::vector<mafVMELandmarkCloud*> m_VectorCloud;
  std::vector<mafString> m_FrameLabel;

  wxListBox *m_FramesListBox; 
  long m_ItemId;
};
#endif
