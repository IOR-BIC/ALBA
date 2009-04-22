/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogTransferFunction2D.h,v $
  Language:  C++
  Date:      $Date: 2009-04-22 09:43:04 $
  Version:   $Revision: 1.4.2.1 $
  Authors:   Alexander Savenko
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUIDialogTransferFunction2D_H__
#define __mafGUIDialogTransferFunction2D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafGUIDialog.h"

#include "vtkProp.h"
#include "vtkMAFTransferFunction2D.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class wxNotebook;

class mafVME;
class mmaVolumeMaterial;
class mafRWIBase;
class mafGUI;
class mafGUIRangeSlider;

class vtkViewport;
class vtkRenderWindow;
class vtkRenderer;
class vtkVolumeProperty2;
class vtkMAFTransferFunction2D;
class vtkMAFAdaptiveVolumeMapper;
class vtkImageMapper;
class vtkMAFImageMapToWidgetColors;
class vtkImageResample;
class vtkActor;
class vtkActor2D;
class vtkVolume;
class vtkTextActor;
class vtkCriticalSection;
class vtkWidgetActor;
class vtkInteractorStyleWidget;
class vtkInteractorStylePreviewImage;

//----------------------------------------------------------------------------
// mafGUIDialogTransferFunction2D :
//----------------------------------------------------------------------------
/** This dialog is used to define a 2D transfer function to make a volume Ray Cast.
@sa vtkMAFAdaptiveVolumeRendering mafVisualPipeVolumeRayCasting*/
class mafGUIDialogTransferFunction2D : public mafGUIDialog
{
  friend class vtkInteractorStylePreviewImage;
  friend class vtkInteractorStyleWidget;

public:
								mafGUIDialogTransferFunction2D();  // reads materials from file
               ~mafGUIDialogTransferFunction2D();  // store materials to file
	void ShowModal(mafVME *vme);
  void OnEvent(mafEventBase *maf_event);
	
protected:
  void CreateGUI();
  void ResizePreviewWindow();
  void InitializeControls();
  void SetControlsRange();
  void CreateWidgetList();
  void UpdatePreview();  

	/** Load volume transfer function from vme tag. */
	void LoadTransferFunction();
	
	/** Save volume transfer function to vme tag. */
	void SaveTransferFunction();

  // create/update widgets
  void UpdateWidgets();

  void OnIdle(wxIdleEvent& event);
  void OnSize(wxSizeEvent& event) { this->ResizePreviewWindow(); event.Skip(); }
  void OnCommand(wxCommandEvent &event) { this->OnEvent(&mafEvent(this, event.GetId())); }
  void OnMotion(wxMouseEvent &event) { m_StatusBar->SetStatusText("", 0); m_StatusBar->SetStatusText("", 1); }
  void SetWidget(const tfWidget &widget);

  // vme  
  mafVME                *m_Vme;
  mmaVolumeMaterial     *m_Material;
	vtkVolumeProperty2  	*m_VolumeProperty;
  vtkMAFTransferFunction2D *m_TransferFunction;
  
  // slice preview
  wxNotebook            *m_PreviewBook;
  mafRWIBase    				*m_SliceRwi;
  vtkRenderWindow       *m_SliceWindow;
  vtkTextActor          *m_WaitActor;
  vtkRenderer           *m_SliceRenderer;
  vtkImageResample      *m_SliceResampler;
  vtkImageMapper        *m_SliceMapper;
  vtkMAFImageMapToWidgetColors *m_SliceFilter;
  vtkActor2D            *m_SliceActor;

  // 3d preview
  mafRWIBase    				  *m_Rwi3D;
  vtkRenderer             *m_Renderer3D;
  vtkRenderWindow         *m_Window3D;
  vtkMAFAdaptiveVolumeMapper *m_Mapper3D;
  vtkVolume               *m_Volume3D;

  // graph/widget window
  mafRWIBase  		*m_GraphRwi;
  vtkRenderer     *m_GraphRenderer;
  vtkRenderWindow *m_GraphWindow;
  vtkWidgetActor  *m_WidgetActor;

  vtkCriticalSection *m_CriticalSection;

  tfWidget   m_Widget;
  wxListBox *m_WidgetList;
  int        m_CurrentWidget;
  
  int             m_SliceNumber;
  mafGUIRangeSlider *m_WindowingSlider;
  double          m_SliceLevel;
  double          m_SliceWinowing;
  wxString        m_WidgetName;
  wxColour        m_WidgetColor;
  wxString        m_Filename;
  mafGUIRangeSlider *m_ValueSlider;
  mafGUIRangeSlider *m_GradientSlider;

  wxStatusBar *m_StatusBar;

  double m_DataRange[2];
  double m_GradientRange[2];

  // pipeline status
  enum { PipeNotReady, PipeReady, PipePlugged};
  
	bool m_DataReady;
  int  m_SlicePipeStatus;
  int  m_PipeStatus3D;

  static VTK_THREAD_RETURN_TYPE CreatePipe(void *ptr);

DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// vtkWidgetActor :
//----------------------------------------------------------------------------
class vtkWidgetActor : public vtkProp 
{
public:
  static vtkWidgetActor *New() { return new vtkWidgetActor(); }
  vtkTypeMacro(vtkWidgetActor, vtkProp);

  int RenderOverlay (vtkViewport *);

  void SetTransferFunction(vtkMAFTransferFunction2D *tf);
  void SetViewportRange(double xmin, double xmax, double ymin, double ymax) { ViewportRange[0] = xmin; ViewportRange[1] = xmax; ViewportRange[2] = ymin; ViewportRange[3] = ymax; }
  const double *GetViewportRange() const { return this->ViewportRange; }

  void SetSelection(int index) { this->SelectedWidget = index; }
  void HighlightPoint(int widget, int point) { this->HighlightedWidget = widget; this->HighlightedPoint = point; }

  void SetCurrentPosition(double value, double gradient) { this->CurrentPoint[0] = value; this->CurrentPoint[1] = gradient; }

  int Pick(vtkViewport *viewport, int x, int y, int &pickedPoint);

protected:
  vtkWidgetActor();
 ~vtkWidgetActor();

  vtkWidgetActor(const vtkWidgetActor&); // no implementation
  void operator=(const vtkWidgetActor&); // no implementation

  vtkMAFTransferFunction2D *TransferFunction;
  double                 ViewportRange[4]; // xmin, xmax, ymin, ymax
  int                    SelectedWidget;
  int                    HighlightedWidget;
  int                    HighlightedPoint;

  double                  CurrentPoint[2];
};

//////////////// interactor styles //////////////////

#include "vtkInteractorStyleImage.h"

//----------------------------------------------------------------------------
// vtkInteractorStylePreviewImage :
//----------------------------------------------------------------------------
class vtkInteractorStylePreviewImage : public vtkInteractorStyleImage 
{
public:
  static vtkInteractorStylePreviewImage *New() { return new vtkInteractorStylePreviewImage(); }
  vtkTypeMacro(vtkInteractorStylePreviewImage, vtkInteractorStyleImage);

	vtkInteractorStylePreviewImage() { this->m_Dialog = NULL; this->m_ActiveButton[0] = this->m_ActiveButton[1] = this->m_ActiveButton[2] = false; }

  void SetDialog(mafGUIDialogTransferFunction2D *dialog) { this->m_Dialog = dialog;  }

  void OnMouseMove();
  void OnLeftButtonUp() { this->m_ActiveButton[0] = false; }
  void OnMiddleButtonUp() { this->m_ActiveButton[1] = false; }
  void OnRightButtonUp() { this->m_ActiveButton[2] = false; }

  void OnLeftButtonDown();
  void OnMiddleButtonDown();
  void OnRightButtonDown();

protected:
  vtkInteractorStylePreviewImage(const vtkInteractorStylePreviewImage&);
  void operator=(const vtkInteractorStylePreviewImage&);

  bool m_ActiveButton[3];
  int  m_StartPosition[3][2];

  tfWidget m_Widget;

  mafGUIDialogTransferFunction2D *m_Dialog;
    
  bool FindValueAndGradient(int x, int y, double &value, double &gradient);
};

//----------------------------------------------------------------------------
// vtkInteractorStyleWidget :
//----------------------------------------------------------------------------
class vtkInteractorStyleWidget : public vtkInteractorStylePreviewImage 
{
public:
  static vtkInteractorStyleWidget *New() { return new vtkInteractorStyleWidget(); }
  vtkTypeMacro(vtkInteractorStyleWidget, vtkInteractorStylePreviewImage);

  vtkInteractorStyleWidget() { ; }
 ~vtkInteractorStyleWidget() { ; }

  void OnMouseMove();
  void OnLeftButtonDown();

protected:
  int SelectedPoint;

  vtkInteractorStyleWidget(const vtkInteractorStyleWidget&);
  void operator=(const vtkInteractorStyleWidget&);
};
#endif  // _mafGUIDialogTransferFunction2D_H_
