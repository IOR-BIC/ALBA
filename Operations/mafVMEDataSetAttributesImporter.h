/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEDataSetAttributesImporter.h,v $
  Language:  C++
  Date:      $Date: 2008-04-30 16:55:35 $
  Version:   $Revision: 1.6 $
  Authors:   Stefano Perticoni     
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafVMEDataSetAttributesImporter_h
#define __mafVMEDataSetAttributesImporter_h

class mafVMEGeneric;
#include "mafString.h"

/** 
mafVMEDataSetAttributesImporter

Import generic time varying data set attributes in a vme.
The input vme is modified ie all operations occurs in place.
Actually supports vme mesh, surface and volumes 
*/
class mafVMEDataSetAttributesImporter
{
  
public:
 
  /**
  Construct a mafVMEDataSetAttributesImporter */
  mafVMEDataSetAttributesImporter();
  ~mafVMEDataSetAttributesImporter();
  
  /**
  Set the input vme; BEWARE: this filter works in place! */
  void SetInput(mafVMEGeneric *input) {m_Input = input;};
  mafVMEGeneric *GetInput() {return m_Input;};

  /**
  Get the output after calling Read(); BEWARE: this filter works in place! */
  mafVMEGeneric *GetOutput() {return m_Input;};

  /** Set the dataset attribute filename. If attributes are time varying 
  the filename will be used to find the directory containing attribute files*/
  void SetFileName(const char *filename);
  const char *GetFileName() {return m_FileName;};

  /** Time Varying flag */
  void TimeVaryingOn() {m_TimeVarying = true;};
  void TimeVaryingOff() {m_TimeVarying = false;};
  void SetTimeVaryingFlag(bool flag) {m_TimeVarying = flag;};
  bool GetTimeVaryingFlag() {return m_TimeVarying;};

  /** An id array can be used to map vtk cell or points ID to special ids,
  this is useful for interfacing with finite elements simulation software
  for example the following input attributes file:

  ELEM	EPEL1
  49445	0.0004
  41871	0.0035
  41995	0.0045
  
  has elements with ID 49445, 41871, 41995
  
  if this array is present in vtk data structure:

  ANSYS_ELEMENT_ID 1 3 int
  41995 49740 41871

  it can be used to map vtk ids to ansys one

  vtkId     0        1      2
  ansysId 41995    49740  41871
  */
  void UseIdArrayOn() {m_UseIdArray = true;};
  void UseIdArrayOff() {m_UseIdArray = false;};
  void SetUseIdArrayFlag(bool useIdArray) {m_UseIdArray = useIdArray;};
  bool GetUseIdArrayFlag() {return m_UseIdArray;};

  void SetIdArrayName(const char *idArrayName) {m_IdArrayName = idArrayName; };
  const char *GetIdArrayName() { return m_IdArrayName.GetCStr(); };

  /**
  Set the prefix of the attribute files; the prefix is the 
  string part before the increasing int number, for examples
  for a sequence like "file_0001.lis", "file_0002.lis" and so on, 
  the prefix is "file_".This is needed for time varying
  result files*/
  void SetFilePrefix(const char *filePrefix) {m_FilePrefix = filePrefix; };
  const char *GetFilePrefix() { return m_FilePrefix.GetCStr(); };

  enum ATTRIBUTE_TYPE {POINT_DATA = 0, CELL_DATA};

  /**
  Set the attribute data type to be imported*/ 
  void SetAttributeTypeToPointData() {this->m_AttributeType = POINT_DATA; };
  void SetAttributeTypeToCellData() {this->m_AttributeType = CELL_DATA; };
  void SetAttributeType(int attributeType) {this->m_AttributeType = attributeType; };
  int GetAttributeType() {return this->m_AttributeType; };

  /** 
  Set the timestamps file name*/
  void SetTSFileName(const char *tsFileName) {this->m_TSFileName = tsFileName; };
  const char* GetTSFileName() {return this->m_TSFileName.GetCStr(); };  

  /** 
  Set the UseTSFile ivar. If UseTSFile is true time stamps values for each resul file 
  will be extracted from the text file*/
  void UseTSFileOn() {this->m_UseTSFile = true; };
  void UseTSFileOff() {this->m_UseTSFile = false; };
  void SetUseTSFile(bool useTS ) {this->m_UseTSFile = useTS; };
  bool GetUseTSFile() {return this->m_UseTSFile; };
  
  /** parse the input data file and fill the dataset with the attribute data.
  Returns MAF_OK or MAF_ERROR*/
  int Read();

protected:
  
  bool m_TimeVarying;
  
  /** 
  Set the name of the directory containing attribute data files; this is needed for time varying
  attribute data files*/
  void SetResultsDir(const char *resDir) {m_ResultsDir = resDir; };
  const char *GetResultsDir() { return m_ResultsDir.GetCStr(); };

 
  /** 
  Set the extension of the result files ie the part following
  the increasing int number. This is needed for time varying
  result files*/
  void SetFileExtension(const char *ext) {m_FileExtension = ext; };
  const char *GetFileExtension() {return m_FileExtension.GetCStr(); };

  int SplitFileName();
	
  /** file access info */
  mafString m_FileName;
  mafString m_ResultsDir;
  mafString m_FileBaseName;
  mafString m_FilePrefix;
  mafString m_FileExtension;
  mafString m_TSFileName;
  
  /** the input vme */
  mafVMEGeneric *m_Input;

  /** use timestamp file*/
  bool m_UseTSFile;

  /** use data array for attributes id */
  bool m_UseIdArray;
  mafString m_IdArrayName;

  /** result type*/
  int m_AttributeType;

private:
  
};


#endif

