/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmdClientMAF.h,v $
Language:  C++
Date:      $Date: 2006-03-28 16:56:32 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdClientMAF_h
#define __mmdClientMAF_h

#include "mafDevice.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class ClientUnit;

/** Device implementing interface for PLASTIC hub.
  mmdClientMAF is a class providing interface for MAFServer (socket comunication).
  @sa mafDevice
*/

class mmdClientMAF : public mafDevice
{
public:
  mafTypeMacro(mmdClientMAF, mafDevice); 

  virtual void OnEvent(mafEventBase *maf_event);

  //----------------------------------------------------------------------------
  //    GUI Constants
  //----------------------------------------------------------------------------
  enum MAF_CLIENT_WIDGET_ID
  {
  	ID_SERVER_HOSTNAME = Superclass::ID_LAST,
    ID_PORT_NUMBER,
    ID_LAST
  };

  /** internal function to create device GUI for settings */
  virtual void CreateGui();

protected:
  mmdClientMAF();
  virtual ~mmdClientMAF();

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  ClientUnit *m_Client;
  mafString   m_ServerHost;
  int         m_PortNumber;
private:
  mmdClientMAF(const mmdClientMAF&);  // Not implemented.
  void operator=(const mmdClientMAF&);  // Not implemented.

};
#endif
