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
class albaInteractionBehavior;
class albaBehaviorPicker;
class albaVMELandmark;
class albaVMELandmarkCloud;
class vtkPoints;
class vtkMatrix4x4;

//----------------------------------------------------------------------------
// albaOpIterativeRegistration :
//----------------------------------------------------------------------------
 
/** Perform point based registration in iterative way.
  A left click on the source and on the target VME places a pair of landmarks (a new click moves them);
  Register computes the rigid registration of all the pairs and moves the source VME, and the next clicks
  place a new pair. The landmarks are half transparent and the picking goes through them.
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

    /** Assign the picker of a side (SOURCE or TARGET) to its VME */
    void AttachPicker(int side, albaVME *vme);

    /** A pick on a side creates its landmark, or moves its current landmark */
    void OnVmePicked(int side, albaEvent *e);

    /** Give the VMEs their behaviors again and remove the landmark clouds */
    void Clear();

    int Register();

    double m_CurrentTime;

    albaVME *m_SourceVME;
    albaVME *m_TargetVME;
 
    albaBehaviorPicker *m_Picker[2];           ///< pickers of the source and target VMEs
    albaInteractionBehavior *m_OldBehavior[2]; ///< behaviors of the source and target VMEs before the operation
    albaVMELandmarkCloud *m_Cloud[2];          ///< landmarks of the source and target VMEs
    albaVMELandmark *m_Landmark[2];            ///< current landmarks: a pick moves them, NULL after a registration

    albaString m_SourceVmeName;
    albaString m_TargetVmeName;

    vtkMatrix4x4* m_RegistrationMatrix;
    vtkMatrix4x4* m_UndoSourceAbsPose;

    /** Test Friend */
    friend class albaOpIterativeRegistrationTest;
};
#endif
