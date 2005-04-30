/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar3D2DPicker.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:55 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAvatar3D2DPicker_h
#define __mafAvatar3D2DPicker_h

#ifdef __GNUG__
    #pragma interface "mafAvatar3D2DPicker.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafAvatar3D.h"

class vtkConeSource;
class vtkAxes;
class vtkPolyDataMapper;
class vtkActor;
class vtkAssembly;
//class vtkCaptionActor2D;
class vtkConeSource;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkCellPicker;


/** Create an avatar with a 3D Cone
  This avatar implements a n hybrid 2D/3D avatar, switching between 2D and 3D.
  When in 2D mode the avatar sends 2D events, when in 3D sends 3D events.
  @sa mafAvatar3D
*/
class mafAvatar3D2DPicker : public mafAvatar3D
{
public:
  vtkTypeMacro(mafAvatar3D2DPicker,mafAvatar3D);
  static mafAvatar3D2DPicker *New();
  static const char *GetTypeName() {return "mafAvatar3D2DPicker";}
  static vtkObject *NewObjectInstance() {return new mafAvatar3D2DPicker;}

  /** pick in the scene with this avatar, give the avatar pose */
  virtual int Pick(mflMatrix *pose=NULL);

  /** Set the extent of the canvas within which the avatar behaves as a 2D avatar */
  vtkSetVector4Macro(Canvas2DExtent,double);
  
  /** Get the extent of the canvas within which the avatar behaves as a 2D avatar */
  vtkGetVector4Macro(Canvas2DExtent,double);

protected:
  mafAvatar3D2DPicker();
  virtual ~mafAvatar3D2DPicker();

  /** process a move event */
  virtual void OnMove3DEvent(mflEventInteraction *e);

  virtual int InternalStore(mflXMLWriter *writer);
  virtual int InternalRestore(vtkXMLDataElement *node,vtkXMLDataParser *parser);

  virtual void CreateSettings();

  /** This is used to allow nested serialization of subclasses.
    This function is called by Store and is reimplemented in subclasses.
    Each subclass can Open/Close its own subelements which are
    closed inside the "Device" XML element. */
  //virtual int InternalStore(mflXMLWriter *writer);

  //virtual int InternalRestore(vtkXMLDataElement *node,vtkXMLDataParser *parser);

  double Canvas2DExtent[4];

  vtkConeSource     *ConeCursor; 
  vtkAxes           *CursorAxes; 
  vtkPolyDataMapper *CursorMapper;
  vtkActor          *CursorActor;
  vtkPolyDataMapper *CursorAxesMapper;
  vtkActor          *CursorAxesActor;

  //vtkCaptionActor2D *Cursor2D;
  vtkConeSource       *Cursor2D;
  vtkPolyDataMapper2D *CursorMapper2D;
  vtkActor2D          *CursorActor2D;


private:
  mafAvatar3D2DPicker(const mafAvatar3D2DPicker&);  // Not implemented.
  void operator=(const mafAvatar3D2DPicker&);  // Not implemented.
};

#endif 
