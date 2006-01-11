/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoBonemat.h,v $
  Language:  C++
  Date:      $Date: 2006-01-11 14:05:24 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoBonemat_H__
#define __mmoBonemat_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;

//----------------------------------------------------------------------------
// mmoBonemat :
//----------------------------------------------------------------------------
/** */
class mmoBonemat: public mafOp
{
public:
  mmoBonemat(wxString label);
 ~mmoBonemat(); 
  mafOp* Copy();
	void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();
  
  /** Set the configuration file name */
  void SetConfigurationFileName(const char* name);  

  /** Get the configuration file name */
  const char* GetConfigurationFileName();

  /** Set the InputMesh file name */
  void SetInputMeshFileName(const char* name);  

  /** Get the InputMesh file name */
  const char* GetInputMeshFileName();

  /** Set the OutputMesh file name */
  void SetOutputMeshFileName(const char* name);  

  /** Get the OutputMesh file name */
  const char* GetOutputMeshFileName();

  /** Set the InputCT file name */
  void SetInputCTFileName(const char* name);  

  /** Get the InputCT file name */
  const char* GetInputCTFileName();


  /** Set the Frequency file name */
  void SetFrequencyFileName(const char* name);  

  /** Get the Frequency file name */
  const char* GetFrequencyFileName();

  /** Create the operation graphical user interface*/
  void CreateGui();

  /** Execute the procedure that maps TAC values on the finite element mesh */
  int Execute();

  /** Read configuration file and fill in member variables. Member variables are updated only if
  all files can be opened and in this case the value 0 is returned otherwise -1 is returned*/
  int OpenConfigurationFile();
  
  /** Write the configuration file */
  int SaveConfigurationFileAs();
  int SaveConfigurationFile(const char *fileName);

  void OnOpenConfigurationFileButton();
  void OnSaveConfigurationFileButton();
  void OnSaveConfigurationFileAsButton();
  void OnOpenInputMeshButton();
  void OnOpenInputTacButton();
  void OnOutputMeshName();
  void OnOutputFrequencyFileName();
  void OnExecute();
  
protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  mafVME   *m_Vme; 

  mafString m_ConfigurationFileName;
  mafString m_InputMeshFileName;
  mafString m_InputCTFileName;
  wxString m_OutputMeshFileName;
  wxString m_FrequencyFileName;
  
  double m_FirstExponentialCoefficientsVector[3];
  double m_SecondExponentialCoefficientsVector[3];
  double m_HUThreshold;
  double m_CalibrationFirstPoint[2];
  double m_CalibrationSecondPoint[2];
  int m_StepsNumber; 
  double m_GapValue;



};
#endif
