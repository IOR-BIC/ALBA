/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeLayout.h,v $
  Language:  C++
  Date:      $Date: 2008-04-23 09:07:25 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafNodeLayout_h
#define __mafNodeLayout_h

#include "mafNode.h"
#include "mafRoot.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmaApplicationLayout;

/** mafNodeLayout - a VME is the node to store application layout list. 
  @sa mafNodeLayout mafNode */
class MAF_EXPORT mafNodeLayout : public mafNode
{
public:
  mafTypeMacro(mafNodeLayout,mafNode);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** Return the application layout attribute.*/
  mmaApplicationLayout *GetLayout();

protected:
  mafNodeLayout();
  virtual ~mafNodeLayout() {}

  mmaApplicationLayout *m_AppLayout;

private:
  mafNodeLayout(const mafNodeLayout&); // Not implemented
  void operator=(const mafNodeLayout&); // Not implemented
};
#endif
