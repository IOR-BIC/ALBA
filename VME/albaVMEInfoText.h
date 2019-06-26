/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEInfoText
 Authors: Fedor Moiseev
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMEInfoText_h
#define __albaVMEInfoText_h

#include "albaVMEGenericAbstract.h"
#include "albaVMEOutputNULL.h"

class ALBA_EXPORT albaVMEInfoText : public albaVMEGenericAbstract
{
public:
  albaTypeMacro(albaVMEInfoText,albaVMEGenericAbstract);

  /** Return the right type of output.*/  
  albaVMEOutputNULL *GetVTKOutput() {return (albaVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  virtual albaVMEOutput *GetOutput();

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  enum INFOTEX_WIDGET_ID
  {
    ID_SCALE_FACTOR = Superclass::ID_LAST,
    ID_LAST
  };

  /** Copy the contents of another VME-RefSys into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another VME-RefSys. */
  virtual bool Equals(albaVME *vme);

  void SetPosShow(bool show, int index);
  bool GetPosShow(int index);
  void SetPosLabel(const albaString& label, int index);
  const char *GetPosLabel(int index);
  const char *GetPosText(int index);

  void AddString(const albaString& str){m_Strings.push_back(str);Modified();}
  int GetNumberOfStrings() {return m_Strings.size();}
  //void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes){kframes.clear();}

  void SetTimeStamp(albaTimeStamp t);

  /** return icon */
  static char** GetIcon();

protected:
  albaVMEInfoText();
  virtual ~albaVMEInfoText();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);
  
  albaString              m_PositionText[3];


private:

  albaVMEInfoText(const albaVMEInfoText&); // Not implemented
  void operator=(const albaVMEInfoText&); // Not implemented

  bool                   m_PosShow[3];
  albaString              m_PosLabels[3];
  std::vector<albaString> m_Strings;
};

#endif
