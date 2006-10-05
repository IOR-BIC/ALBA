
/*=======================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: mafVMERawMotionData.h,v $
  Language:  C++
  Date:      $Date: 2006-10-05 08:46:10 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni (perticoni@tecno.ior.it)
  Project:   Multimod Project (http://www.ior.it/multimod/)

=========================================================================
Copyright (c) 2001 B3C - BioComputing Compentence Centre 
(http://www.cineca.it/B3C/)

Redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the 
following conditions are met: 

* Redistributions of source code must retain the above 
copyright notice, this list of conditions and the following 
disclaimer.

* Redistributions in binary form must reproduce the above 
copyright notice, this list of conditions and the following 
disclaimer in the documentation and/or other materials 
provided with the distribution.

* Neither name of the Multimod Consortium or its partner 
institutions nor the names of any contributors may be used 
to endorse or promote products derived from this software 
without specific prior written permission.

* Modified source versions must be plainly marked as such, 
and must not be misrepresented
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THIS SOFTWARE IS INTENDED FOR EDUCATIONAL AND 
INVESTIGATIONAL PURPOSES ONLY. THUS, NO MEDICAL DECISION 
SHOULD BE BASED ON RESULTS PROVIDED BY THE SOFTWARE. UNDER 
NO CIRCUMSTANCES SHALL SOFTWARE BE USED AS A CLINICAL 
DIAGNOSTIC TOOL.

=========================================================================*/
// .NAME mafVMERawMotionData
//
// .SECTION Description
//  Read ascii files used in movement analysis
//  represented by a N x (M*3) matrix where
//  N is the number of time steps and M is the 
//  number of landmarks
//
// .SECTION ToDo


#ifndef __mafVMERawMotionData_h
#define __mafVMERawMotionData_h

#include "mafVMEGroup.h"
#include <fstream>

class mafVMELandmarkCloud;



class  mafVMERawMotionData : public mafVMEGroup  // era VTK_SVTK_EXPORT
{
public:

  mafTypeMacro(mafVMERawMotionData,mafVMEGroup);
  //void PrintSelf(ostream& os, vtkIndent indent) ;
  
  /**
  // Set RAW motion data  Dictionary file name*/
  void SetDictionaryFileName(const char *name);
  
  /**
  // Get RAW motion data  Dictionary file name*/
  const char *GetDictionaryName() {return this->DictionaryFileName.GetCStr();}

  /**
  // Set RAW motion data file name*/
  void mafVMERawMotionData::SetFileName(const char *name);
  
  /**
  // Get RAW motion data file name*/
  const char *GetFileName() {return this->FileName.GetCStr(); }	
  
  /**
  //Parse RAW motion data file and fill VME Tree*/
  int Read();

	/**
	//Use DictionaryOn() to use a dictionary otherwise
	//only one segment will be created and all the landmarks 
	//will be appended to it.*/
	  void SetDictionary(int Dict){Dictionary = Dict;}
	  int GetDictionary(){return Dictionary;}

	  void DictionaryOn () { this->SetDictionary((int)1);}
	  void DictionaryOff () { this->SetDictionary((int)0);}

protected:

  mafVMERawMotionData();
  virtual ~mafVMERawMotionData();

  // By default, UpdateInformation calls this method. Subclasses should fill
  // the output structure information.
  //virtual int ExecuteInformation();

  //virtual int ExecuteData(mafVMEItem *item); //virtual int ExecuteData(mflVMEItem *item);

  mafString DictionaryFileName; //era mflString
  mafString FileName; //era mflString
  int Dictionary;
  mafVMELandmarkCloud *v_current_dlc;
	mafVMELandmarkCloud *v_dlc;

private:
  mafVMERawMotionData(const mafVMERawMotionData&);  // Not implemented.
  void operator=(const mafVMERawMotionData&);  // Not implemented.

  
};

#endif


