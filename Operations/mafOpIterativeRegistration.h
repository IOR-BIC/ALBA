/*=========================================================================

 Program: MAF2
 Module: mafOpIterativeRegistration
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpIterativeRegistration_H__
#define __mafOpIterativeRegistration_H__


//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafEvent; 
class mafGUILandmark;
class vtkPoints;
class vtkMatrix4x4;

//----------------------------------------------------------------------------
// mafOpIterativeRegistration :
//----------------------------------------------------------------------------
 
/** Perform point based registration in iterative way

  @sa
  - mafGUILandmark 
 
  @todo
  - Code cleaning and improve documentation
*/

class MAF_EXPORT mafOpIterativeRegistration : public mafOp
{
  public:
    mafOpIterativeRegistration(wxString label = "Iterative Registration");
    ~mafOpIterativeRegistration(); 

    virtual void OnEvent(mafEventBase *maf_event);

    mafTypeMacro(mafOpIterativeRegistration, mafOp);
    
    /** 
    Return a copy of itself, this needs to put the operation into the undo stack. */
    mafOp* Copy();
  	
    /** 
    Return true for the acceptable vme type. */
    bool Accept(mafNode* node);
  
    /** 
    Builds operation's interface. */
    void OpRun();
  
    /** 
    Execute the operation. */
    void OpDo();
  
    /** 
    Undo the operation. */
    void OpUndo();

    static bool VmeAccept(mafNode* node) {return(node != NULL);};
  
  protected:
    //void OnRegister(mafEvent &e);
    void OnRegister(mafEventBase *maf_event);

    /** 
    This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
    void OpStop(int result);

    /** Create GUI binding Guilandmark */
    void CreateGui();

    //void OnEventThis(mafEvent &e);
    void OnEventThis(mafEventBase *maf_event);
    //void OnEventGuiSource(mafEvent &e);
    void OnEventGuiSource(mafEventBase *maf_event);
    //void OnEventGuiTarget(mafEvent &e);
    void OnEventGuiTarget(mafEventBase *maf_event);

    int Register();

    double m_CurrentTime;

    mafVME *m_SourceVME;
    mafVME *m_TargetVME;
 
    mafGUILandmark *m_GuiLandmark[2];

    mafString m_SourceVmeName;
    mafString m_TargetVmeName;

    vtkMatrix4x4* m_RegistrationMatrix;
    vtkMatrix4x4* m_UndoSourceAbsPose;

    /** Test Friend */
    friend class mafOpIterativeRegistrationTest;
};
#endif
