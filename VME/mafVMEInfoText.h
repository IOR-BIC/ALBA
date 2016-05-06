/*=========================================================================

 Program: MAF2
 Module: mafVMEInfoText
 Authors: Fedor Moiseev
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVMEInfoText_h
#define __mafVMEInfoText_h

#include "mafVMEGenericAbstract.h"
#include "mafVMEOutputNULL.h"

class MAF_EXPORT mafVMEInfoText : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEInfoText,mafVMEGenericAbstract);

  /** Return the right type of output.*/  
  mafVMEOutputNULL *GetVTKOutput() {return (mafVMEOutputNULL *)GetOutput();}

  /** Return the output. This create the output object on demand. */  
  virtual mafVMEOutput *GetOutput();

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  enum INFOTEX_WIDGET_ID
  {
    ID_SCALE_FACTOR = Superclass::ID_LAST,
    ID_LAST
  };

  /** Copy the contents of another VME-RefSys into this one. */
  virtual int DeepCopy(mafVME *a);

  /** Compare with another VME-RefSys. */
  virtual bool Equals(mafVME *vme);

  void SetPosShow(bool show, int index);
  bool GetPosShow(int index);
  void SetPosLabel(const mafString& label, int index);
  const char *GetPosLabel(int index);
  const char *GetPosText(int index);

  void AddString(const mafString& str){m_Strings.push_back(str);Modified();}
  int GetNumberOfStrings() {return m_Strings.size();}
  //void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){kframes.clear();}

  void SetTimeStamp(mafTimeStamp t);

  /** return icon */
  static char** GetIcon();

protected:
  mafVMEInfoText();
  virtual ~mafVMEInfoText();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
  
  mafString              m_PositionText[3];


private:

  mafVMEInfoText(const mafVMEInfoText&); // Not implemented
  void operator=(const mafVMEInfoText&); // Not implemented

  bool                   m_PosShow[3];
  mafString              m_PosLabels[3];
  std::vector<mafString> m_Strings;
};

#endif
