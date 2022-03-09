/*=========================================================================
Program:   LHP
Module:    $RCSfile: lhpOpComputeHausdorffDistance.h,v $
Language:  C++
Date:      $Date: 2011-08-05 09:11:04 $
Version:   $Revision: 1.1.2.3 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __lhpOpComputeHausdorffDistance_H__
#define __lhpOpComputeHausdorffDistance_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaOp.h"
#include "albaVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaGUIDialog;
class albaOpImporterSTL;
class albaVMEMesh;
class albaVMEVolumeGray;

class vtkImageData;
class vtkMAFContourVolumeMapper;
class vtkPolyData;


class LHP_OPERATIONS_EXPORT lhpOpComputeHausdorffDistance: public albaOp
{
public:
  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum GUI_IDS
  {
    ID_OK = MINID, 
    ID_CANCEL,
  };

  /** constructor. */
  lhpOpComputeHausdorffDistance(const wxString &label = "Compute Hausdorff Distance");
  /** destructor. */
  ~lhpOpComputeHausdorffDistance(); 

  /** Precess events coming from other objects */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** RTTI macro */
  albaTypeMacro(lhpOpComputeHausdorffDistance, albaOp);

  /*virtual*/ albaOp* Copy();

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(albaVME *node);

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

  static bool SurfaceAccept(albaVME* node) { return(node != NULL && node->IsA("albaVMESurface"));};

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);

  /** Compute H. Distance. */
  int ComputeDistance();

  /** Create the operation GUI. */
  void CreateGui();

  albaString *m_FilenameSTL1, *m_FilenameSTL2;
  albaString *m_VMEName1, *m_VMEName2;
  albaString *m_OutputDir;

  int m_VmeOrSTL1, m_VmeOrSTL2;

  albaVMESurface *m_SurfaceInput1;
  albaVMESurface *m_SurfaceInput2;
  albaVMESurface *m_SurfaceOutput;

  albaOpImporterSTL *m_STLImporter;
  std::vector<albaVMESurface*> m_ImportedSurfaces;

};
#endif
