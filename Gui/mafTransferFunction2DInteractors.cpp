
/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTransferFunction2DInteractors.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:30:13 $
  Version:   $Revision: 1.3 $
  Authors:   Alexander Savenko
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUIDialogTransferFunction2D.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// VTK
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"

#include "vtkCellPicker.h"
#include "vtkPointPicker.h"
#include "vtkTexture.h"
#include "vtkRectilinearGrid.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkImageMapper.h"
#include "vtkMAFImageMapToWidgetColors.h"
#include "vtkPolyDataMapper.h"

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))
template<typename type> static inline type clip(type x, type xmin, type xmax) { if (x < xmin) return xmin; if (x > xmax) return xmax; return x; }

///////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
void vtkInteractorStylePreviewImage::OnLeftButtonDown()
//------------------------------------------------------------------------
{
  if (m_Dialog->m_SlicePipeStatus != mafGUIDialogTransferFunction2D::PipePlugged)
    return;

  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  double value = 0, gradientNorm = 0;
  if (!this->FindValueAndGradient(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1], value, gradientNorm))
    return; // image was not picked

  this->m_ActiveButton[0] = true; 
  this->m_StartPosition[0][0] = x; 
  this->m_StartPosition[0][1] = y; 
  
  // position widgets
  this->m_Widget = this->m_Dialog->m_Widget;
  double range = (m_Dialog->m_DataRange[1] - m_Dialog->m_DataRange[0]) * 0.04f;
  this->m_Widget.Range[0][0] = value - range;
  this->m_Widget.Range[0][1] = value + range;
  this->m_Widget.Range[0][2] = value;
  range = (m_Dialog->m_GradientRange[1] - m_Dialog->m_GradientRange[0]) * 0.04f;
  this->m_Widget.Range[1][0] = gradientNorm - range;
  this->m_Widget.Range[1][1] = gradientNorm + range;
  this->m_Widget.Range[1][2] = gradientNorm;
  this->m_Dialog->SetWidget(this->m_Widget);
}

//------------------------------------------------------------------------
void vtkInteractorStylePreviewImage::OnMiddleButtonDown()
//------------------------------------------------------------------------
{
  if (m_Dialog->m_SlicePipeStatus != mafGUIDialogTransferFunction2D::PipePlugged)
    return;

  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->m_ActiveButton[1] = true; 
  this->m_StartPosition[1][0] = x; 
  this->m_StartPosition[1][1] = y; 
}

//------------------------------------------------------------------------
void vtkInteractorStylePreviewImage::OnRightButtonDown()  
//------------------------------------------------------------------------
{
  if (m_Dialog->m_SlicePipeStatus != mafGUIDialogTransferFunction2D::PipePlugged)
    return;

  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->m_ActiveButton[2] = true; 
  this->m_StartPosition[2][0] = x; 
  this->m_StartPosition[2][1] = y; 
}

//------------------------------------------------------------------------
void vtkInteractorStylePreviewImage::OnMouseMove()
//------------------------------------------------------------------------
{
  if (m_Dialog->m_SlicePipeStatus != mafGUIDialogTransferFunction2D::PipePlugged)
    return;

  int x = this->Interactor->GetEventPosition()[0], y = this->Interactor->GetEventPosition()[1];

  // print current value and gradient
  double value = 0, gradientNorm = 0;
  if (this->FindValueAndGradient(x, y, value, gradientNorm)) 
  {
    wxString label;
    label.Printf("%.0f %.0f", value, gradientNorm);
    m_Dialog->m_StatusBar->SetStatusText(label, 0);

    m_Dialog->m_WidgetActor->SetCurrentPosition(value, gradientNorm);
    m_Dialog->m_GraphRenderer->GetRenderWindow()->Render();
  }
  else 
  {
    m_Dialog->m_StatusBar->SetStatusText("", 0);
  }

  // handle dragging 
  if (this->m_ActiveButton[0]) 
  {
    double drange = (m_Dialog->m_DataRange[1] - m_Dialog->m_DataRange[0]);
    double grange = (m_Dialog->m_GradientRange[1] - m_Dialog->m_GradientRange[0]);

    double dx = 0.5f * drange * double(x - this->m_StartPosition[0][0]) / m_Dialog->m_SliceRenderer->GetSize()[0];
    double dy = 0.5f * grange * double(y - this->m_StartPosition[0][1]) / m_Dialog->m_SliceRenderer->GetSize()[1];

    // modify range
    tfWidget widget = this->m_Dialog->m_Widget;
    widget.Range[0][0] = clip(this->m_Widget.Range[0][0] - dx, m_Dialog->m_DataRange[0] - (double)0.04 * drange, this->m_Widget.Range[0][2]);
    widget.Range[0][1] = clip(this->m_Widget.Range[0][1] + dx, this->m_Widget.Range[0][2], m_Dialog->m_DataRange[1] + (double)0.04 * drange);
    
    widget.Range[1][0] = clip(this->m_Widget.Range[1][0] - dy, m_Dialog->m_GradientRange[0] - (double)0.04 * grange, this->m_Widget.Range[1][2]);
    widget.Range[1][1] = clip(this->m_Widget.Range[1][1] + dy, this->m_Widget.Range[1][2], m_Dialog->m_GradientRange[1] + (double)0.04 * grange);
    this->m_Dialog->SetWidget(widget);
  }
}

//------------------------------------------------------------------------
bool vtkInteractorStylePreviewImage::FindValueAndGradient(int x, int y, double &value, double &gradientNorm) 
//------------------------------------------------------------------------
{
  const int *actorCoordinates = m_Dialog->m_SliceActor->GetActualPositionCoordinate()->GetComputedDisplayValue(this->m_Dialog->m_SliceRenderer);
  const int *imageSize = m_Dialog->m_SliceMapper->GetInput()->GetDimensions();
  x -= actorCoordinates[0];
  y -= actorCoordinates[1];
  if (x >= 0 && y >= 0 && x < imageSize[0] && y < imageSize[1]) 
  {
    vtkImageData *data = m_Dialog->m_SliceFilter->GetInput();
    const int *origImageSize = data->GetDimensions();
    x = int(double(x) / imageSize[0] * origImageSize[0] + 0.5f);
    y = int(double(y) / imageSize[1] * origImageSize[1] + 0.5f);
    int z = m_Dialog->m_SliceMapper->GetZSlice();
    value = data->GetScalarComponentAsDouble(x, y, z, 0);
    double gradient[3];
    data->GetPointGradient(x, y, z, data->GetPointData()->GetScalars(), gradient);
    gradientNorm =  sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
    return true;
  }
  else 
  {
    return false;
  }
}

//------------------------------------------------------------------------
void vtkInteractorStyleWidget::OnMouseMove() 
//------------------------------------------------------------------------
{
  if (this->m_Dialog->m_SlicePipeStatus != mafGUIDialogTransferFunction2D::PipePlugged)
    return;

  int x = this->Interactor->GetEventPosition()[0], y = this->Interactor->GetEventPosition()[1];
  
  // no dragging
  if (!this->m_ActiveButton[0] && !this->m_ActiveButton[1] && !this->m_ActiveButton[2]) 
  {
    int widgetIndex, pointIndex;
    widgetIndex = this->m_Dialog->m_WidgetActor->Pick(this->m_Dialog->m_GraphRenderer, x, y, pointIndex);
    m_Dialog->m_StatusBar->SetStatusText(widgetIndex >= 0 ? "Click to select the widget" : "", 1);
    this->m_Dialog->m_WidgetActor->HighlightPoint(widgetIndex, pointIndex);
    m_Dialog->m_GraphRenderer->GetRenderWindow()->Render();
  }
  else if (this->m_ActiveButton[0]) 
  { // dragging
    const double sx = this->m_Dialog->m_GraphRenderer->GetSize()[0], sy = this->m_Dialog->m_GraphRenderer->GetSize()[1];
    const double *viewportRange = this->m_Dialog->m_WidgetActor->GetViewportRange();

    double drange[3], grange[3];
    memcpy(drange, this->m_Widget.Range[0], sizeof(drange));
    memcpy(grange, this->m_Widget.Range[1], sizeof(grange));

    if (this->SelectedPoint != -1) 
    {
      const double fx = x / sx * (viewportRange[1] - viewportRange[0]) + viewportRange[0];
      const double fy = y / sy * (viewportRange[3] - viewportRange[2]) + viewportRange[2];
      
      if (this->SelectedPoint == 0) 
      {
        drange[0] = clip(fx, viewportRange[0], drange[2]);
        m_Dialog->m_TransferFunction->SetWidgetValueRange(m_Dialog->m_CurrentWidget, drange);
        grange[1] = clip(fy, grange[2], viewportRange[3]);
        m_Dialog->m_TransferFunction->SetWidgetGradientRange(m_Dialog->m_CurrentWidget, grange);
      }
      else if (this->SelectedPoint == 1) 
      {
        drange[1] = clip(fx, drange[2], viewportRange[1]);
        m_Dialog->m_TransferFunction->SetWidgetValueRange(m_Dialog->m_CurrentWidget, drange);
        grange[1] = clip(fy, grange[2], viewportRange[3]);
        m_Dialog->m_TransferFunction->SetWidgetGradientRange(m_Dialog->m_CurrentWidget, grange);
      }
      else if (this->SelectedPoint == 2) 
      {
        double ratio = clip((fx - drange[2]) / (drange[1] - drange[2]), (double)0.f, (double)1.f);
        m_Dialog->m_TransferFunction->SetWidgetValueRatio(m_Dialog->m_CurrentWidget, ratio);
        grange[0] = clip(fy, (double)0.0, grange[2]);
        m_Dialog->m_TransferFunction->SetWidgetGradientRange(m_Dialog->m_CurrentWidget, grange);
      }
      else if (this->SelectedPoint == 3) 
      {
        double ratio = clip((fx - drange[2]) / (drange[0] - drange[2]), (double)0.f, (double)1.f);
        m_Dialog->m_TransferFunction->SetWidgetValueRatio(m_Dialog->m_CurrentWidget, ratio);
        grange[0] = clip(fy, (double)0.f, grange[2]);
        m_Dialog->m_TransferFunction->SetWidgetGradientRange(m_Dialog->m_CurrentWidget, grange);
      }
      else if (this->SelectedPoint == 4) 
      { // center
        drange[2] = clip(fx, drange[0], drange[1]);
        m_Dialog->m_TransferFunction->SetWidgetValueRange(m_Dialog->m_CurrentWidget, drange);
        grange[2] = clip(fy, grange[0], grange[1]);
        m_Dialog->m_TransferFunction->SetWidgetGradientRange(m_Dialog->m_CurrentWidget, grange);
      }

      m_Dialog->InitializeControls();
      m_Dialog->UpdatePreview();
    }
    else 
    { // the whole widget
      const double dx = (x - this->m_StartPosition[0][0]) / sx * (viewportRange[1] - viewportRange[0]);
      const double dy = (y - this->m_StartPosition[0][1]) / sy * (viewportRange[3] - viewportRange[2]);
      drange[0] = clip(drange[0] + dx, viewportRange[0], viewportRange[1]);
      drange[1] = clip(drange[1] + dx, viewportRange[0], viewportRange[1]);
      drange[2] = clip(drange[2] + dx, viewportRange[0], viewportRange[1]);
      m_Dialog->m_TransferFunction->SetWidgetValueRange(m_Dialog->m_CurrentWidget, drange);
      grange[0] = clip(grange[0] + dy, viewportRange[2], viewportRange[3]);
      grange[1] = clip(grange[1] + dy, viewportRange[2], viewportRange[3]);
      grange[2] = clip(grange[2] + dy, viewportRange[2], viewportRange[3]);
      m_Dialog->m_TransferFunction->SetWidgetGradientRange(m_Dialog->m_CurrentWidget, grange);

      m_Dialog->InitializeControls();
      m_Dialog->UpdatePreview();
    }
  }
}

//------------------------------------------------------------------------
void vtkInteractorStyleWidget::OnLeftButtonDown() 
//------------------------------------------------------------------------
{
  if (m_Dialog->m_SlicePipeStatus != mafGUIDialogTransferFunction2D::PipePlugged)
    return;

  int x = this->Interactor->GetEventPosition()[0], y = this->Interactor->GetEventPosition()[1];
  int widgetIndex = this->m_Dialog->m_WidgetActor->Pick(this->m_Dialog->m_GraphRenderer, x, y, this->SelectedPoint);
  if (widgetIndex >= 0) 
  {
    if (widgetIndex != m_Dialog->m_CurrentWidget && widgetIndex < m_Dialog->m_TransferFunction->GetNumberOfWidgets()) 
    {
      m_Dialog->m_CurrentWidget = widgetIndex;
      m_Dialog->CreateWidgetList();
      m_Dialog->InitializeControls();
      m_Dialog->UpdatePreview();
    }
    m_Dialog->m_StatusBar->SetStatusText(_("Drag to change widget range"), 1);
    this->m_Widget = m_Dialog->m_TransferFunction->GetWidget(widgetIndex);
    this->m_ActiveButton[0] = true; 
    this->m_StartPosition[0][0] = x; 
    this->m_StartPosition[0][1] = y; 
  }
}

///////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------
vtkWidgetActor::vtkWidgetActor() 
//------------------------------------------------------------------------
{
  this->TransferFunction = NULL;
  this->SelectedWidget = -1;
  this->HighlightedWidget = -1;
  this->HighlightedPoint  = -1;

  this->CurrentPoint[0] = this->CurrentPoint[1] = VTK_DOUBLE_MIN;
}

//------------------------------------------------------------------------
vtkWidgetActor::~vtkWidgetActor() 
//------------------------------------------------------------------------
{
  if (this->TransferFunction)
    this->TransferFunction->Delete();
}

//------------------------------------------------------------------------
void vtkWidgetActor::SetTransferFunction(vtkMAFTransferFunction2D *tf) 
//------------------------------------------------------------------------
{
  if (this->TransferFunction == tf)
    return;
  if (this->TransferFunction)
    this->TransferFunction->Delete();
  this->TransferFunction = tf;
  this->TransferFunction->Register(this);
}

//------------------------------------------------------------------------
int vtkWidgetActor::RenderOverlay (vtkViewport *viewport) 
//------------------------------------------------------------------------
{
  if (this->TransferFunction == NULL)
    return 0;

  // init GL
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  const double sx = viewport->GetSize()[0], sy = viewport->GetSize()[1];
  const int  isx = viewport->GetSize()[0], isy = viewport->GetSize()[1];

  glViewport(0, 0, isx, isy);

  // draw background
  const int numPixels = isx * isy;
  double *opacityTable = new double [numPixels + isx + isy];
  unsigned char *rgbdTable = new unsigned char[4 * numPixels];
  double *vTable = opacityTable + numPixels;
  double *gTable = vTable + isx;
  const double kx = (this->ViewportRange[1] - this->ViewportRange[0]) / sx;
  for (int x = 0; x < isx; x++)
    vTable[x] = double(x) * kx + this->ViewportRange[0];
  const double ky = (this->ViewportRange[3] - this->ViewportRange[2]) / sy;
  for (int y = 0; y < isy; y++)
    gTable[y] = double(y) * ky + this->ViewportRange[2];
  this->TransferFunction->GetTable(isx, vTable, isy, gTable, opacityTable, rgbdTable, false);
  unsigned char *background = (unsigned char *)rgbdTable;
  for (int pos = 0; pos < numPixels; pos++) 
  {
    double a = opacityTable[pos];
    unsigned char *rgb = rgbdTable + (pos << 2);
    *(background++) = (unsigned char)(rgb[0] * a);
    *(background++) = (unsigned char)(rgb[1] * a);
    *(background++) = (unsigned char)(rgb[2] * a);
  }

  glRasterPos3i(-1, -1, 0);
  glDisable(GL_DEPTH_TEST);
  glDrawPixels(isx, isy, GL_RGB, GL_UNSIGNED_BYTE, rgbdTable);
  delete [] opacityTable;
  delete [] rgbdTable;

  glOrtho(this->ViewportRange[0], this->ViewportRange[1], this->ViewportRange[2], this->ViewportRange[3], 0, 2);
  for (int iteration = 0; iteration < 2; iteration++) 
  {
    for (int index = 0; index < this->TransferFunction->GetNumberOfWidgets(); index++) 
    {
      if (iteration == 0 && index != this->SelectedWidget || iteration != 0 && index == this->SelectedWidget)
        continue;

      // define points
      const double (&range)[2][3] = this->TransferFunction->GetWidget(index).Range;
      const double   origRatio = this->TransferFunction->GetWidget(index).Ratio;
      const double   z = this->SelectedWidget == index ? 0.f : -1.f;
      const double   ratio = origRatio + (1.f - origRatio) * (range[1][2] - range[1][0]) / (range[1][1] - range[1][0]);
      
      double points[5][3] = { {range[0][0], range[1][1], z},
        {range[0][1], range[1][1], z},
        {range[0][2] + origRatio * (range[0][1] - range[0][2]), range[1][0], z},
        {range[0][2] + origRatio * (range[0][0] - range[0][2]), range[1][0], z},
        {range[0][2], range[1][2], z}};

      // draw frame: stippled
      if (this->SelectedWidget != index) 
      {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0xf0f0);
        glDisable(GL_DEPTH_TEST);
        glColor3d(0.2, 0.2, 0.2);
        glBegin(GL_LINE_LOOP);
        glVertex3dv(points[0]);
        glVertex3dv(points[1]);
        glVertex3dv(points[2]);
        glVertex3dv(points[3]);
        glEnd();
      }
      
      // draw frame: solid
      glDisable(GL_LINE_STIPPLE);
      glEnable(GL_DEPTH_TEST);
      glColor3d(this->SelectedWidget == index ? 1.0: 0.3, 0.3, 0.3);
      glBegin(GL_LINE_LOOP);
      glVertex3dv(points[0]);
      glVertex3dv(points[1]);
      glVertex3dv(points[2]);
      glVertex3dv(points[3]);
      glEnd();
      
      // draw markers
      for (int i = 0; i < 5; i++) 
      {
        glPointSize((index == this->HighlightedWidget && this->HighlightedPoint == i) ? 9.f : 3.f);
        glBegin(GL_POINTS);
        glVertex3dv(points[i]);
        glEnd();
      }

      // modify z-buffer to hide other lines
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glBegin(GL_QUADS);
      glVertex3dv(points[0]);
      glVertex3dv(points[1]);
      glVertex3dv(points[2]);
      glVertex3dv(points[3]);
      glEnd();
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
  }

  // draw current point
  if (this->CurrentPoint[0] != VTK_DOUBLE_MIN) 
  {
    glColor3d(0.2, 1.0, 0.2);
    glDisable(GL_DEPTH_TEST);
    glPointSize(3.f);
    glBegin(GL_POINTS);
    glVertex2d(this->CurrentPoint[0], this->CurrentPoint[1]);
    glEnd();
    glColor3d(0.2, 0.2, 0.2);
    glPointSize(1.0);
    glBegin(GL_POINTS);
    glVertex2d(this->CurrentPoint[0], this->CurrentPoint[1]);
    glEnd();
    this->CurrentPoint[0] = VTK_DOUBLE_MIN;
  }

  // restore gl state
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glPopAttrib();

  return 1;
}

//------------------------------------------------------------------------
int vtkWidgetActor::Pick(vtkViewport *viewport, int x, int y, int &pickedPoint) 
//------------------------------------------------------------------------
{
  const double sx = viewport->GetSize()[0], sy = viewport->GetSize()[1];
  const double fx = x / sx * (this->ViewportRange[1] - this->ViewportRange[0]) + this->ViewportRange[0];
  const double fy = y / sy * (this->ViewportRange[3] - this->ViewportRange[2]) + this->ViewportRange[2];

  const double dx = 7.f * (this->ViewportRange[1] - this->ViewportRange[0]) / sx;
  const double dy = 7.f * (this->ViewportRange[3] - this->ViewportRange[2]) / sy;

  pickedPoint = -1;
  for (int iteration = 0; iteration < 2; iteration++) 
  {
    for (int index = 0; index < this->TransferFunction->GetNumberOfWidgets(); index++) 
    {
      if (iteration == 0 && index != this->SelectedWidget || iteration != 0 && index == this->SelectedWidget)
        continue;

      // intersect with widget
      const tfWidget &widget = this->TransferFunction->GetWidget(index);

      // test points
      const double (&range)[2][3] = this->TransferFunction->GetWidget(index).Range;
      const double origRatio = this->TransferFunction->GetWidget(index).Ratio;
      const double z = this->SelectedWidget == index ? 0.f : -1.f;
      const double ratio = origRatio + (1.f - origRatio) * (range[1][2] - range[1][0]) / (range[1][1] - range[1][0]);
      
      double points[5][3] = { {range[0][0], range[1][1], z},
        {range[0][1], range[1][1], z},
        {range[0][2] + origRatio * (range[0][1] - range[0][2]), range[1][0], z},
        {range[0][2] + origRatio * (range[0][0] - range[0][2]), range[1][0], z},
        {range[0][2], range[1][2], z}};

      // test intersection
      for(pickedPoint = 4; pickedPoint >= 0; pickedPoint--) 
      {
        if (fabs(points[pickedPoint][0] - fx) < dx && fabs(points[pickedPoint][1] - fy) < dy)
          break;
      }

      if (widget.Attenuation(fx, fy) == 0.0 && pickedPoint == -1)
         continue;

      return index;
    }
  }

  return -1;      
}
