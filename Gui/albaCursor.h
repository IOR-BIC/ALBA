/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCursor_H__
#define __albaCursor_H__

#include "albaDefines.h"
#include "wx/cursor.h"

enum CursorTypes
{
	ALBA_CURSOR_ROTATE = wxCURSOR_MAX,
};

/**
  Helper class in the albaGUI module to set the global application cursor.
  The class is non-instantiable and exposes a single static method `setcursor`.
*/
class ALBA_EXPORT albaCursor
{
public:
  // Set the application cursor. Uses wxWidgets' wxSetCursor internally.
  static void SetCursor(wxWindow *window, const int cursor);
  
  // Call this before application exit to avoid memoryleaks
	static void DeleteCursors();

private:
	static void InitCursors(); // Initializes cursor resources, if needed.

  // Non-instantiable
  albaCursor() = delete;
  ~albaCursor() = delete;
};

#endif // __albaCursor_H__