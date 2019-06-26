/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERawMotionData
 Authors: Stefano Perticoni - porting Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMERawMotionData_h
#define __albaVMERawMotionData_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMEGroup.h"
#include <fstream>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMELandmarkCloud;

/** albaVMERawMotionData - 
*/
class ALBA_EXPORT albaVMERawMotionData : public albaVMEGroup  
{
public:

  albaTypeMacro(albaVMERawMotionData,albaVMEGroup);
  
  /** Set RAW motion data  Dictionary file name*/
  void SetDictionaryFileName(const char *name);
  
  /** Get RAW motion data  Dictionary file name*/
  const char *GetDictionaryName() {return this->m_DictionaryFileName.GetCStr();}

  /** Set RAW motion data file name*/
  void albaVMERawMotionData::SetFileName(const char *name);
  
  /** Get RAW motion data file name*/
  const char *GetFileName() {return this->m_FileName.GetCStr(); }	
  
  /** Parse RAW motion data file and fill VME Tree*/
  int Read();

	/**
	Use DictionaryOn() to use a dictionary otherwise
	only one segment will be created and all the landmarks 
	will be appended to it.*/
	void SetDictionary(int Dict){m_Dictionary = Dict;}
	int GetDictionary(){return m_Dictionary;}

	void DictionaryOn () { this->SetDictionary((int)1);}
	void DictionaryOff () { this->SetDictionary((int)0);}

protected:

  albaVMERawMotionData();
  virtual ~albaVMERawMotionData();

  // By default, UpdateInformation calls this method. Subclasses should fill
  // the output structure information.
  //virtual int ExecuteInformation();

  //virtual int ExecuteData(albaVMEItem *item); //virtual int ExecuteData(mflVMEItem *item);

  albaString m_DictionaryFileName; 
  albaString m_FileName; 
  int m_Dictionary;

private:
  albaVMERawMotionData(const albaVMERawMotionData&);  // Not implemented.
  void operator=(const albaVMERawMotionData&);  // Not implemented.

  
};

#endif


