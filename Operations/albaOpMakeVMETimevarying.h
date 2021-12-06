/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMakeVMETimevarying
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpMakeVMETimevarying_H__
#define __albaOpMakeVMETimevarying_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaString.h"
#include "albaVME.h"
#include "albaVMEGeneric.h"

//----------------------------------------------------------------------------
// forward references:
//----------------------------------------------------------------------------
class albaEvent;
class albaVME;
class albaString;
class wxListBox;
class albaVMEGeneric;

//----------------------------------------------------------------------------
//albaOpMakeVMETimevarying:
//----------------------------------------------------------------------------
/**
This operation accepts as input a set of VMEs of the same type and representing the same object.
It returns a timevaryng VME representing the object in different (user defined) timestamps.
*/

//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpMakeVMETimevarying: public albaOp
//----------------------------------------------------------------------------
{
public:

  albaOpMakeVMETimevarying(const wxString &label = "Make Timevariyng VME",bool showShadingPlane = false);
	virtual ~albaOpMakeVMETimevarying();
	virtual void OnEvent(albaEventBase *alba_event);
	
  albaTypeMacro(albaOpMakeVMETimevarying, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();
  void OpUndo();

protected:
	
  /** Structure to store an added VME with the associated TimeStamp */
  typedef struct {
    albaVME * m_VME;
    double m_TimeStamp;
  } VmeTimevaryingItem;

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  virtual void OpStop(int result);

  /** GUI */
  void CreateGui();
  /** Update the added VMEs ListBox */
  void UpdateAddedVMEsListBox();//GUI
  /** Implements the condition for accepting node in the dialog */
  static bool DialogAcceptVME(albaVME* node) {return(node != NULL)&&(!node->IsA("albaVMERoot"));};

  /** Return true if the specified VME represent the same object of previously added VMEs */
  bool AcceptVME(albaVME * vme);
  /** Return true if the specified TimeStamp is assigned to none of previously added VMEs */
  bool AcceptTimeStamp(double timestamp);

  /** Add a VME with the specified TimeStamp (increasing sort by TimeStamp)*/
  void AddVME(albaVME * vme, double timestamp);
  /** Delete a VME with the specified index */
  void DeleteVME(int index);

  /** Create the resulting timevariyng VME */
  void albaOpMakeVMETimevarying::Execute();

  albaVMEGeneric * m_VMETimevarying;               //< Resulting timevariyng VME
  albaVME * m_CurrentVME;                          //< Current VME
  albaString m_CurrentVMEName;
  albaString m_VMETimevaryingName;                 //< Resulting timevariyng VME name
  albaString m_VMEsType;                           //< Added (and Resulting) VMEs type (defined by the first added VME)
  albaString m_VMEsOutputType;                     //< Added (and Resulting) VMEs output type (defined by the first added VME)
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
