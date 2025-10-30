/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogTransferFunction2D
 Authors: Alexander Savenko
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIDialogTransferFunction2D_H__
#define __albaGUIDialogTransferFunction2D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"

#include "vtkProp.h"
#include "vtkALBATransferFunction2D.h"
#include <mutex>

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class wxNotebook;

class albaVME;
class mmaVolumeMaterial;
class albaRWIBase;
class albaGUI;
class albaGUIRangeSlider;

class vtkViewport;
class vtkRenderWindow;
class vtkRenderer;
class vtkVolumeProperty2;
class vtkALBATransferFunction2D;
class vtkSmartVolumeMapper;
class vtkImageMapper;
class vtkALBAImageMapToWidgetColors;
class vtkImageResample;
class vtkActor;
class vtkActor2D;
class vtkVolume;
class vtkTextActor;
class vtkWidgetActor;
class vtkInteractorStyleWidget;
class vtkInteractorStylePreviewImage;


//----------------------------------------------------------------------------
// albaGUIDialogTransferFunction2D :
//----------------------------------------------------------------------------
/** This dialog is used to define a 2D transfer function to make a volume Ray Cast.*/
class ALBA_EXPORT albaGUIDialogTransferFunction2D : public albaGUIDialog
{
  friend class vtkInteractorStylePreviewImage;
  friend class vtkInteractorStyleWidget;

public:
								albaGUIDialogTransferFunction2D();  // reads materials from file
               ~albaGUIDialogTransferFunction2D();  // store materials to file
	void ShowModal(albaVME *vme);
  void OnEvent(albaEventBase *alba_event);
	
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
  void OnCommand(wxCommandEvent &event) { this->OnEvent(&albaEvent(this, event.GetId())); }
  void OnMotion(wxMouseEvent &event) { m_StatusBar->SetStatusText("", 0); m_StatusBar->SetStatusText("", 1); }
  void SetWidget(const tfWidget &widget);

  // vme  
  albaVME                *m_Vme;
  mmaVolumeMaterial     *m_Material;
	vtkVolumeProperty2  	*m_VolumeProperty;
  vtkALBATransferFunction2D *m_TransferFunction;
  
  // slice preview
  wxNotebook            *m_PreviewBook;
  albaRWIBase    				*m_SliceRwi;
  vtkRenderWindow       *m_SliceWindow;
  vtkTextActor          *m_WaitActor;
  vtkRenderer           *m_SliceRenderer;
  vtkImageResample      *m_SliceResampler;
  vtkImageMapper        *m_SliceMapper;
  vtkALBAImageMapToWidgetColors *m_SliceFilter;
  vtkActor2D            *m_SliceActor;

  // 3d preview
  albaRWIBase    				  *m_Rwi3D;
  vtkRenderer             *m_Renderer3D;
  vtkRenderWindow         *m_Window3D;
  vtkSmartVolumeMapper*   m_Mapper3D;
  vtkVolume               *m_Volume3D;

  // graph/widget window
  albaRWIBase  		*m_GraphRwi;
  vtkRenderer     *m_GraphRenderer;
  vtkRenderWindow *m_GraphWindow;
  vtkWidgetActor  *m_WidgetActor;

  std::mutex m_CriticalSection;

  tfWidget   m_Widget;
  wxListBox *m_WidgetList;
  int        m_CurrentWidget;
  
  int             m_SliceNumber;
  albaGUIRangeSlider *m_WindowingSlider;
  double          m_SliceLevel;
  double          m_SliceWinowing;
  wxString        m_WidgetName;
  wxColour        m_WidgetColor;
  wxString        m_Filename;
  albaGUIRangeSlider *m_ValueSlider;
  albaGUIRangeSlider *m_GradientSlider;

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
class ALBA_EXPORT vtkWidgetActor : public vtkProp 
{
public:
  static vtkWidgetActor *New() { return new vtkWidgetActor(); }
  vtkTypeMacro(vtkWidgetActor, vtkProp);

  int RenderOverlay (vtkViewport *);

  void SetTransferFunction(vtkALBATransferFunction2D *tf);
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

  vtkALBATransferFunction2D *TransferFunction;
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
class ALBA_EXPORT vtkInteractorStylePreviewImage : public vtkInteractorStyleImage 
{
public:
  static vtkInteractorStylePreviewImage *New() { return new vtkInteractorStylePreviewImage(); }
  vtkTypeMacro(vtkInteractorStylePreviewImage, vtkInteractorStyleImage);

	vtkInteractorStylePreviewImage() { this->m_Dialog = NULL; this->m_ActiveButton[0] = this->m_ActiveButton[1] = this->m_ActiveButton[2] = false; }

  void SetDialog(albaGUIDialogTransferFunction2D *dialog) { this->m_Dialog = dialog;  }

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

  albaGUIDialogTransferFunction2D *m_Dialog;
    
  bool FindValueAndGradient(int x, int y, double &value, double &gradient);
};

//----------------------------------------------------------------------------
// vtkInteractorStyleWidget :
//----------------------------------------------------------------------------
class ALBA_EXPORT vtkInteractorStyleWidget : public vtkInteractorStylePreviewImage 
{
public:
  static vtkInteractorStyleWidget *New() { return new vtkInteractorStyleWidget(); }
  vtkTypeMacro(vtkInteractorStyleWidget, vtkInteractorStylePreviewImage);

  vtkInteractorStyleWidget() { ; }
 ~vtkInteractorStyleWidget() { ; }

  void OnMouseMove();
  void OnLeftButtonDown();

protected:
  int m_SelectedPoint;

  vtkInteractorStyleWidget(const vtkInteractorStyleWidget&);
  void operator=(const vtkInteractorStyleWidget&);
};
#endif  // _albaGUIDialogTransferFunction2D_H_
