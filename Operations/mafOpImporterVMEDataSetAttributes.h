/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVMEDataSetAttributes
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterVMEDataSetAttributes_H__
#define __mafOpImporterVMEDataSetAttributes_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;
class mafEvent;
class mafVMEDataSetAttributesImporter;

//----------------------------------------------------------------------------
// mafOpImporterVMEDataSetAttributes :
//----------------------------------------------------------------------------
/** 
Generic attribute importer operation for vme datasets (timevarying also); this is a wrapper
for mafVMEDataSetAttributesImporter utility class. 

  @sa mafOpImporterVMEDataSetAttributesTest mafVMEDataSetAttributesImporter mafVMEDataSetAttributesImporterTest

*/
class MAF_EXPORT mafOpImporterVMEDataSetAttributes: public mafOp
{
public:

	mafOpImporterVMEDataSetAttributes(const wxString &label = "mafOpImporterVMEDataSetAttributes");
	~mafOpImporterVMEDataSetAttributes(); 
	
  mafTypeMacro(mafOpImporterVMEDataSetAttributes, mafOp);

  /** Set the dataset attribute filename. If attributes are time varying 
  the filename will be used to find the directory containing attribute files*/
  void SetFileName(const char *filename);

  /** The attributes are time varying? */
  void TimeVaryingOn();
  void TimeVaryingOff();

  /**
  Set the prefix of the attribute files; the prefix is the 
  string part before the increasing int number, for examples
  for a sequence like "file_0001.lis", "file_0002.lis" and so on, 
  the prefix is "file_".This is needed for time varying
  result files*/
  void SetFilePrefix(const char *filePrefix);

  /** 
  Set the timestamps file name*/
  void SetTSFileName(const char *tsFileName);
  
  /** 
  Are we using the time stamp file?*/
  void SetUseTSFile(bool useTSFile);

  void SetUseIdArrayFlag(bool useIdArray) {m_UseIdArray = useIdArray;};
  bool GetUseIdArrayFlag() {return m_UseIdArray;};

  void SetIdArrayName(const char *idArrayName) {m_IdArrayName = idArrayName; };
  const char *GetIdArrayName() { return m_IdArrayName.GetCStr(); };

  /** 
  Set the attribute data type*/
  void SetDataTypeToPointData();
  void SetDataTypeToCellData();

  /** parse the input data file and fill the dataset with the attribute data.
  Returns MAF_OK or MAF_ERROR*/
  int Read();

  /** copy the operation */
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

  /** Builds operation's interface. */
	void OpRun();
  
  /** Events handling */
  virtual void OnEvent(mafEventBase *maf_event);
 
protected:

  /** file access info */
  mafString m_FileName;
  mafString m_FilePrefix;
  mafString m_TSFileName;

  /** is the attribute data time varying? */
  int m_TimeVarying;

  /** use timestamp file*/
  int m_UseTSFile;

  /** result type*/
  int m_AttributeType;
  
  mafVMEDataSetAttributesImporter *m_Importer;

  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);
	void EnableTimeVaryingGui( bool enable );

  /** use data array for cell id */
  int m_UseIdArray;
  mafString m_IdArrayName;

};
#endif
