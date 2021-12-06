/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpIterativeRegistration
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpIterativeRegistration_H__
#define __albaOpIterativeRegistration_H__


//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaEvent; 
class albaGUILandmark;
class vtkPoints;
class vtkMatrix4x4;

//----------------------------------------------------------------------------
// albaOpIterativeRegistration :
//----------------------------------------------------------------------------
 
/** Perform point based registration in iterative way

  @sa
  - albaGUILandmark 
 
  @todo
  - Code cleaning and improve documentation
*/

class ALBA_EXPORT albaOpIterativeRegistration : public albaOp
{
  public:
    albaOpIterativeRegistration(wxString label = "Iterative Registration");
    ~albaOpIterativeRegistration(); 

    virtual void OnEvent(albaEventBase *alba_event);

    albaTypeMacro(albaOpIterativeRegistration, albaOp);
    
    /** 
    Return a copy of itself, this needs to put the operation into the undo stack. */
    albaOp* Copy();
  
    /** 
    Builds operation's interface. */
    void OpRun();
  
    /** 
    Execute the operation. */
    void OpDo();
  
    /** 
    Undo the operation. */
    void OpUndo();

    static bool VmeAccept(albaVME* node) {return(node != NULL);};
  
  protected:

		/** Return true for the acceptable vme type. */
		bool InternalAccept(albaVME*node);

    //void OnRegister(albaEvent &e);
    void OnRegister(albaEventBase *alba_event);

    /** 
    This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
    void OpStop(int result);

    /** Create GUI binding Guilandmark */
    void CreateGui();

    //void OnEventThis(albaEvent &e);
    void OnEventThis(albaEventBase *alba_event);
    //void OnEventGuiSource(albaEvent &e);
    void OnEventGuiSource(albaEventBase *alba_event);
    //void OnEventGuiTarget(albaEvent &e);
    void OnEventGuiTarget(albaEventBase *alba_event);

    int Register();

    double m_CurrentTime;

    albaVME *m_SourceVME;
    albaVME *m_TargetVME;
 
    albaGUILandmark *m_GuiLandmark[2];

    albaString m_SourceVmeName;
    albaString m_TargetVmeName;

    vtkMatrix4x4* m_RegistrationMatrix;
    vtkMatrix4x4* m_UndoSourceAbsPose;

    /** Test Friend */
    friend class albaOpIterativeRegistrationTest;
};
#endif
