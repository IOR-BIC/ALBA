/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpDecomposeTimeVarVME.h,v $
  Language:  C++
  Date:      $Date: 2007-11-20 09:36:17 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2007 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
class mmgRollOut;
class mafVMEGroup;

//////
class mafVMEAFRefSys;
class mafGui;
class mafEvent;
class mafIntGraphHyer;
class mafVMELandmarkCloud;
class vtkPoints;


//----------------------------------------------------------------------------
// mmoRefSys :
//----------------------------------------------------------------------------
/** */
class mafOpDecomposeTimeVarVME: public mafOp
{
public:
  mafOpDecomposeTimeVarVME(const wxString& label = "TimeReduce");
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
  MODE_FRAMES = 0,
  MODE_INTERVAL = 1,
  MODE_PERIODICITY = 2.*/
  void SelectMode(int mode);

  /** Set the timestamos interval. */
  void SetInterval(double from, double to);

  /** Set the period used for periodicity mode */
  void SetPeriodicity(int period);

  /** Create a static VME from a particular timestamp of the input VME. */
  void CreateStaticVME(mafTimeStamp time);

  enum DECOMPOSE_MODE_WIDGET_ID
  {
    CHANGE_MODE = MINID,
    ID_LIST_FRAMES,
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
    MODE_FRAMES = 0,
    MODE_INTERVAL,
    MODE_PERIODICITY,
  };

  enum MATERIAL_TAG_ID
  {
    MAT_NAME = 0,
    MAT_AMBIENT_R,
    MAT_AMBIENT_G,
    MAT_AMBIENT_B,
    MAT_AMBIENT_INTENSITY,
    MAT_DIFFUSE_R,
    MAT_DIFFUSE_G,
    MAT_DIFFUSE_B,
    MAT_DIFFUSE_INTENSITY,
    MAT_SPECULAR_R,
    MAT_SPECULAR_G,
    MAT_SPECULAR_B,
    MAT_SPECULAR_INTENSITY,
    MAT_SPECULAR_POWER,
    MAT_OPACITY,
    MAT_REPRESENTATION,
    MAT_NUM_COMP
  };

protected: 
  /** Additional Gui. */
  void ModeGui();

  mmgRollOut *m_RollOutFrames;
  mmgRollOut *m_RollOutInterval;
  mmgRollOut *m_RollOutPeriodicity;

private:
  mafVMEGroup         *m_Group;
  mafVMELandmarkCloud *m_Cloud;

  int m_InsertMode;
  int m_Periodicity;
  int m_NumberFrames;

  double m_IntervalFrom;
  double m_IntervalTo;
  double m_Frame;

  std::vector<mafString> m_FrameLabel;

  wxListBox *m_FramesListBox; 
  long m_ItemId;
};
#endif
