/*=========================================================================

 Program: MAF2
 Module: mafOpMakeVMETimevarying
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpMakeVMETimevarying_H__
#define __mafOpMakeVMETimevarying_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "mafString.h"
#include "mafVME.h"
#include "mafVMEGeneric.h"

//----------------------------------------------------------------------------
// forward references:
//----------------------------------------------------------------------------
class mafEvent;
class mafVME;
class mafString;
class wxListBox;
class mafVMEGeneric;

//----------------------------------------------------------------------------
//mafOpMakeVMETimevarying:
//----------------------------------------------------------------------------
/**
This operation accepts as input a set of VMEs of the same type and representing the same object.
It returns a timevaryng VME representing the object in different (user defined) timestamps.
*/

//----------------------------------------------------------------------------
class MAF_EXPORT mafOpMakeVMETimevarying: public mafOp
//----------------------------------------------------------------------------
{
public:

  mafOpMakeVMETimevarying(const wxString &label = "Make Timevariyng VME",bool showShadingPlane = false);
	virtual ~mafOpMakeVMETimevarying();
	virtual void OnEvent(mafEventBase *maf_event);
	
  mafTypeMacro(mafOpMakeVMETimevarying, mafOp);

  mafOp* Copy();

  bool Accept(mafNode* vme);
  void OpRun();
  void OpDo();
  void OpUndo();

protected:

  /** Structure to store an added VME with the associated TimeStamp */
  typedef struct {
    mafVME * m_VME;
    double m_TimeStamp;
  } VmeTimevaryingItem;

  virtual void OpStop(int result);

  /** GUI */
  void CreateGui();
  /** Update the added VMEs ListBox */
  void UpdateAddedVMEsListBox();//GUI
  /** Implements the condition for accepting node in the dialog */
  static bool DialogAcceptVME(mafNode* node) {return(node != NULL)&&(!node->IsA("mafVMERoot"));};

  /** Return true if the specified VME represent the same object of previously added VMEs */
  bool AcceptVME(mafVME * vme);
  /** Return true if the specified TimeStamp is assigned to none of previously added VMEs */
  bool AcceptTimeStamp(double timestamp);

  /** Add a VME with the specified TimeStamp (increasing sort by TimeStamp)*/
  void AddVME(mafVME * vme, double timestamp);
  /** Delete a VME with the specified index */
  void DeleteVME(int index);

  /** Create the resulting timevariyng VME */
  void mafOpMakeVMETimevarying::Execute();

  mafVMEGeneric * m_VMETimevarying;               //< Resulting timevariyng VME
  mafVME * m_CurrentVME;                          //< Current VME
  mafString m_CurrentVMEName;
  mafString m_VMETimevaryingName;                 //< Resulting timevariyng VME name
  mafString m_VMEsType;                           //< Added (and Resulting) VMEs type (defined by the first added VME)
  mafString m_VMEsOutputType;                     //< Added (and Resulting) VMEs output type (defined by the first added VME)
  int m_VMEsVTKDataType;                          //< Added (and Resulting) VMEs vtk data type (defined by the first added VME)

  wxListBox * m_AddedVMEsListBox;                 //< List of added VMEs | GUI

  //int m_NumOfAddedVMEs;                           //< Number of added VMEs    
  double m_CurrentVMETimeStamp;                   //< TimeStamp associated to the VME that will be added
  double m_CurrentVMETimeStampFromVME;            //< TimeStamp that is already associated to the timestamp
  double m_VMETimestampIncrement;                 //< Define the increment of the timestamps
  int m_CurrentVMETimestampModality;              //< Define when the operation use the m_CurrentVMETimeStamp or m_CurrentVMETimeStampFromVME

  wxStaticText * m_LblVMETimeStampFromVME;
  wxStaticText * m_LblVMETimeStampFromVMEData;    //< Label showing m_CurrentVMETimeStampFromVME

  std::vector <VmeTimevaryingItem *> m_AddedVMEs; //< Vector containing the added VMEs with the associated TimeStamps
};
#endif
