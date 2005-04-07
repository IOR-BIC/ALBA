/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPics.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:34:28 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafPics_H__
#define __mafPics_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------

/* add a picture to the factory -- if id already exist it is overwritten */
extern void mafAddPic(wxString id,char** xpm);

/* shotcut to add a picture to the factory */
#define mafADDPIC(X) mafAddPic( #X , X##_xpm )

/* retrieve a picture from the Factory as a wxBitmap */
extern wxBitmap mafGetBmp(wxString id);

/* retrieve a picture from the Factory as a wxImage */
extern wxImage mafGetImg(wxString id);

/* retrieve a picture from the Factory as a wxIcon */
extern wxIcon mafGetIcon(wxString id);

#endif // __mafPics_H__






