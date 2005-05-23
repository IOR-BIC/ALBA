/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTransferFunction2D.cxx,v $
  Language:  C++
  Date:      $Date: 2005-05-23 12:12:05 $
  Version:   $Revision: 1.1 $

=========================================================================*/
#include "vtkObjectFactory.h"

#include "vtkTransferFunction2D.h"

vtkCxxRevisionMacro(vtkTransferFunction2D, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkTransferFunction2D);
vtkCxxRevisionMacro(vtkVolumeProperty2, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkVolumeProperty2);


#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))
template<typename type> static inline type clip(type x, type xmin, type xmax) { if (x < xmin) return xmin; if (x > xmax) return xmax; return x; }

static const int L = 0;
static const int R = 1;
static const int C = 2;

//-----------------------------------------------------------------
vtkTransferFunction2D::vtkTransferFunction2D() {
  this->Initialize();
  }


//-----------------------------------------------------------------
vtkTransferFunction2D::~vtkTransferFunction2D() {
  }


//-----------------------------------------------------------------
void vtkTransferFunction2D::ShallowCopy( vtkDataObject *o) {
  vtkTransferFunction2D *f = vtkTransferFunction2D::SafeDownCast(o);

  if (f != NULL) {
    this->NumberOfWidgets = f->NumberOfWidgets;
    memcpy(this->Widgets, f->Widgets, sizeof(this->Widgets));
    }

  // Do the superclass
  this->vtkDataObject::ShallowCopy(o);
  this->Modified();
  }


//-----------------------------------------------------------------
void vtkTransferFunction2D::Initialize() {
  this->NumberOfWidgets = 0;

  this->UpdateRange[0] = VTK_FLOAT_MIN;
  this->UpdateRange[1] = VTK_FLOAT_MAX;

  this->RangeValid = false;
  }


//-----------------------------------------------------------------
void vtkTransferFunction2D::Modified(float from, float to) {
  this->RangeValid = false;
  this->UpdateRange[0] = from;
  this->UpdateRange[1] = to;
  vtkDataObject::Modified();
  }


//-----------------------------------------------------------------
bool vtkTransferFunction2D::CheckWidget(const tfWidget &widget) {
  // check the widget
  if (widget.Ratio < 0. || widget.Ratio > 2.f ||
      widget.Range[0][C] < widget.Range[0][L] || widget.Range[0][C] > widget.Range[0][R] ||
      widget.Range[1][C] < widget.Range[1][L] || widget.Range[1][C] > widget.Range[1][R] ||
      widget.Opacity > 1.f || widget.Opacity < 0.f ||
      widget.Diffuse > 1.f || widget.Diffuse < 0.f)
    return false;
  
  return true;
  }


//-----------------------------------------------------------------
int vtkTransferFunction2D::AddWidget(const tfWidget &widget) {
  if (this->NumberOfWidgets >= MAX_NUMBER_OF_WIDGETS || !CheckWidget(widget))
    return -1;
  
  this->Widgets[this->NumberOfWidgets] = widget;
  if (this->Widgets[this->NumberOfWidgets].Name[0] == '\0')
    sprintf(this->Widgets[this->NumberOfWidgets].Name, "widget #%d", this->NumberOfWidgets);
  this->Widgets[this->NumberOfWidgets].Update();
  
  this->Modified(widget.Range[0][L], widget.Range[0][R]);

  return this->NumberOfWidgets++;
  }

//-----------------------------------------------------------------
bool vtkTransferFunction2D::RemoveWidget(int index) {
  if (index >= this->NumberOfWidgets || index < 0)
    return false;

  this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
  this->NumberOfWidgets--;
  for ( ; index < this->NumberOfWidgets; index++)
    this->Widgets[index] = this->Widgets[index + 1];
  return true;
  }

//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidget(int index, const tfWidget &widget) {
  if (index < 0 || index >= this->NumberOfWidgets || !CheckWidget(widget))
    return false;
  
  this->Modified(min(widget.Range[0][L], this->Widgets[index].Range[0][L]), max(widget.Range[0][R], this->Widgets[index].Range[0][R]));

  this->Widgets[index] = widget;
  this->Widgets[index].Update();
  return true;
  }


//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetVisibility(int index, bool visible) {
  if (visible != this->Widgets[index].Visible) {
    this->Widgets[index].Visible = visible;
    this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
    }
  return true;
  }

//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetOpacity(int index, float opacity) {
  if (opacity != this->Widgets[index].Opacity) {
    this->Widgets[index].Opacity = clip(opacity, 0.f, 1.f);
    this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
    }
  return true;
  }

//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetColor(int index, float color[3]) {
  if (color[0] != this->Widgets[index].Color[0] || color[1] != this->Widgets[index].Color[1] || color[2] != this->Widgets[index].Color[2]) {
    this->Widgets[index].Color[0] = clip(color[0], 0.f, 1.f);
    this->Widgets[index].Color[1] = clip(color[1], 0.f, 1.f);
    this->Widgets[index].Color[2] = clip(color[2], 0.f, 1.f);
    this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
    }
  return true;
  }


//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetDiffuse(int index, float diffuse) {
  if (diffuse != this->Widgets[index].Diffuse) {
    this->Widgets[index].Diffuse = clip(diffuse, 0.f, 1.f);
    this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
    }
  return true;
  }


//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetValueRatio(int index, float ratio) {
  if (ratio != this->Widgets[index].Ratio) {
    this->Widgets[index].Ratio = clip(ratio, 0.0f, 2.f);
    this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
    this->Widgets[index].Update();
    }
  return true;
  }


//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetValueRange(int index, float range[3]) {
  if (range[0] == this->Widgets[index].Range[0][L] && range[1] == this->Widgets[index].Range[0][C] && range[2] == this->Widgets[index].Range[0][C])
    return true; // nothing to do
  
  this->Widgets[index].Range[0][L] = clip(range[L], VTK_FLOAT_MIN, range[C]);
  this->Widgets[index].Range[0][R] = clip(range[R], range[C], VTK_FLOAT_MAX);
  this->Widgets[index].Range[0][C] = clip(range[C], range[L], range[R]);
  this->Widgets[index].Update();
  this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
  return true;
  }


//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetGradientRange(int index, float range[3]) {
  if (range[0] == this->Widgets[index].Range[1][L] && range[1] == this->Widgets[index].Range[1][R] && range[2] == this->Widgets[index].Range[1][R])
    return true; // nothing to do
  
  this->Widgets[index].Range[1][L] = clip(range[L], VTK_FLOAT_MIN, range[C]);
  this->Widgets[index].Range[1][R] = clip(range[R], range[C], VTK_FLOAT_MAX);
  this->Widgets[index].Range[1][C] = clip(range[C], range[L], range[R]);
  this->Widgets[index].Update();
  this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
  return true;
  }

//-----------------------------------------------------------------
bool vtkTransferFunction2D::SetWidgetGradientInterpolation(int index, int order) {
  if (index < 0 || index >= this->NumberOfWidgets || order < 0 || order > 1)
    return false;
  if (order == this->Widgets[index].GradientInterpolationOrder)
    return true;

  this->Widgets[index].GradientInterpolationOrder = order;
  this->Widgets[index].Update();
  this->Modified(this->Widgets[index].Range[0][L], this->Widgets[index].Range[0][R]);
  return true;
  }

//-----------------------------------------------------------------
void vtkTransferFunction2D::UpdateRanges() {
  if (this->RangeValid)
    return;

  this->ValueRange[0] = VTK_FLOAT_MAX;
  this->ValueRange[1] = VTK_FLOAT_MIN;
  this->GradientRange[0] = VTK_FLOAT_MAX;
  this->GradientRange[1] = VTK_FLOAT_MIN;

  for (int i = 0; i < this->NumberOfWidgets; i++) {
    const tfWidget &w = this->Widgets[i];
    if (w.Opacity > 0.00001 && w.Visible) {
      if (w.Range[0][L] < this->ValueRange[0])
        this->ValueRange[0] = w.Range[0][L];
      if (w.Range[0][R] > this->ValueRange[1])
        this->ValueRange[1] = w.Range[0][R];

      if (w.Range[1][L] < this->GradientRange[0])
        this->GradientRange[0] = w.Range[1][L];
      if (w.Range[1][R] > this->GradientRange[1])
        this->GradientRange[1] = w.Range[1][R];
      }
    }
  if (this->GradientRange[0] < 0)
    this->GradientRange[0] = 0;
  if (this->GradientRange[1] < 0)
    this->GradientRange[1] = 0;

  this->RangeValid = true;
  }


//-----------------------------------------------------------------
float vtkTransferFunction2D::GetValue(const float val, const float gradient, float &opacity, float color[3], float &diffuse) {
  opacity = 0;
  color[0] = color[1] = color[2] = 0;
  diffuse = 0;

  // find the widgets
  float attenuations[MAX_NUMBER_OF_WIDGETS];
  float sumOfAttenuations = 0;
  for (int i = 0; i < this->NumberOfWidgets; i++) {
    const tfWidget &w = this->Widgets[i];
    attenuations[i] = 0.f;
    if (w.Opacity > 0.f && w.Visible && w.Inside(val, gradient)) {
      attenuations[i] = w.Attenuation(val, gradient) * this->Widgets[i].Opacity;
      sumOfAttenuations += attenuations[i];
      }
    }
  if (sumOfAttenuations < 0.0001)
    return 0.f;

  sumOfAttenuations = 1.f / sumOfAttenuations;
  for (i = 0; i < this->NumberOfWidgets; i++) {
    const tfWidget &w = this->Widgets[i];
    if (attenuations[i] > 0.f) {
      const float weight = attenuations[i] * sumOfAttenuations;
      opacity  += weight * attenuations[i];
      color[0] += w.Color[0] * weight;
      color[1] += w.Color[1] * weight;
      color[2] += w.Color[2] * weight;
      diffuse  += w.Diffuse  * weight;
      }
    }
  
  opacity = clip(opacity, 0.f, 1.f);

  return opacity;
  }


//-----------------------------------------------------------------
void tfWidget::Update() {
  this->LengthI[0][0] = 1.f / float(this->Range[0][C] - this->Range[0][L]);
  this->LengthI[0][1] = 1.f / float(this->Range[0][C] - this->Range[0][R]);

  this->LengthI[1][0] = 1.f / float(this->Range[1][C] - this->Range[1][L]);
  this->LengthI[1][1] = 1.f / float(this->Range[1][C] - this->Range[1][R]);

  if (this->Range[1][R] > this->Range[1][L])
    this->RatioI = (1.f - this->Ratio) / float(this->Range[1][R] - this->Range[1][L]);
  else
    this->RatioI = 0.f;
  }

//-----------------------------------------------------------------
float tfWidget::Attenuation(float val, float grad) const {
  if (!this->Visible)
    return 0;

  int lr = grad > this->Range[1][C]; // 0 or 1
  float gv = ((grad - this->Range[1][lr]) * this->LengthI[1][lr]);
  if (this->GradientInterpolationOrder == 0 && gv > 0.)
    gv = 1.f;

  lr = val > this->Range[0][C];  // 0 or 1
  float av = (val  - this->Range[0][lr]) * this->LengthI[0][lr];
  if (this->Ratio != 1.f) {
    const float rx = this->Ratio + this->RatioI * (grad - this->Range[1][L]);
    av = (rx >  0.01f) ? clip(1.f - (1.f - av) / rx, 0.f, 1.f) : 0.f;
    }

  return (av < 0 || gv < 0) ? 0 : (min(gv, av));
  }


//-----------------------------------------------------------------
bool vtkTransferFunction2D::GetTable(int vsize, const float *vTable, int gsize, const float *gTable, float *opacityTable, unsigned char *rgbdTable, bool useOpacity) {
  if (vsize < 1 || gsize < 1 || opacityTable == NULL)
    return false;

  // prepare the table
  //this->SampleCache = new float [2 * vsize * gsize];
  memset(opacityTable, 0, sizeof(float) * vsize * gsize);
  memset(rgbdTable,    0, 4 * vsize * gsize);

  // find the range
  this->UpdateRanges();
  for (int vmin = 0; (vmin < (vsize - 1)) && (vTable[vmin] < this->ValueRange[0]); vmin++)
    { ; }
  for (int vmax = vsize - 1; (vmax > vmin) && (vTable[vmax] >= this->ValueRange[1]); vmax--)
    { ; }
  for (int gmin = 0; (gmin < (gsize - 1)) && (gTable[gmin] < this->GradientRange[0]); gmin++)
    { ; }
  for (int gmax = gsize - 1; (gmax > gmin) && (gTable[gmax] >= this->GradientRange[1]); gmax--)
    { ; }

  if (vmin >= vmax || gmin >= gmax)
    return true;

  // additional variables to speed up calculations
  int activeWidgets[MAX_NUMBER_OF_WIDGETS];
  int activeWidgetsNumber = 0;
  int widgetIndexRange[MAX_NUMBER_OF_WIDGETS][2][2];
  float *widgetAttenuation[MAX_NUMBER_OF_WIDGETS][3]; // attenuation along x and y-axes
  
  for (int wi = 0; wi < this->NumberOfWidgets; wi++) {
    const tfWidget &w = this->Widgets[wi];
    widgetAttenuation[wi][0] = widgetAttenuation[wi][1] = NULL;
    if (w.Opacity < 0.00001f || !w.Visible)
      continue;
    int (&indexRange)[2][2] = widgetIndexRange[wi];

    // find indeces
    for (int vi = vmin; (vi < vmax) && (vTable[vi] < w.Range[0][L]); vi++)
      { ; }
    indexRange[0][L] = vi;
    for ( ; (vi < vmax) && (vTable[vi] < w.Range[0][R]); vi++)
      { ; }
    indexRange[0][R] = vi;
    for (int gi = gmin; (gi < gmax) && (gTable[gi] < w.Range[1][L]); gi++)
      { ; }
    indexRange[1][L] = gi;
    for ( ; (gi < gmax) && (gTable[gi] < w.Range[1][R]); gi++)
      { ; }
    indexRange[1][R] = gi;
    const int widgetVSize = indexRange[0][R] - indexRange[0][L] + 1;
    const int widgetGSize = indexRange[1][R] - indexRange[1][L] + 1;
    if (widgetVSize < 1 || widgetGSize < 1)
      continue;

    widgetAttenuation[wi][0] = new float [widgetVSize + 2 * widgetGSize];
    widgetAttenuation[wi][1] = widgetAttenuation[wi][0] + widgetVSize;
    widgetAttenuation[wi][2] = widgetAttenuation[wi][1] + widgetGSize;
    
    float *pVAttenuation = widgetAttenuation[wi][0] - indexRange[0][L];
    for ( vi = indexRange[0][L]; vi <= indexRange[0][R]; vi++) {
      const int lr = vTable[vi] > w.Range[0][C];
      pVAttenuation[vi] = clip((vTable[vi]  - w.Range[0][lr]) * w.LengthI[0][lr], 0.f, 1.f);
      }
    float *pGAttenuation = widgetAttenuation[wi][1] - indexRange[1][L];
    for ( gi = indexRange[1][L]; gi <= indexRange[1][R]; gi++) {
      const int lr = gTable[gi] > w.Range[1][C];
      pGAttenuation[gi] = clip((gTable[gi]  - w.Range[1][lr]) * w.LengthI[1][lr], 0.f, 1.f);
      if (w.GradientInterpolationOrder == 0 && pGAttenuation[gi] > 0)
        pGAttenuation[gi] = 1.f;
      }
    float *pRAttenuation = widgetAttenuation[wi][2] - indexRange[1][L];
    for ( gi = indexRange[1][L]; gi <= indexRange[1][R]; gi++)
      pRAttenuation[gi] = 1.f / (w.Ratio + w.RatioI * (gTable[gi] - w.Range[1][L]));
    activeWidgets[activeWidgetsNumber++] = wi;
    }
  if (activeWidgetsNumber == 0)
    return true;

  // handle intersected widgets
  const int newvsize = vmax - vmin + 1, newgsize = gmax - gmin + 1;;
  float * const attenuationSums = new float[newvsize * newgsize];
  memset(attenuationSums, 0, sizeof(float) * newvsize * newgsize);
  for (int awi = 0; awi < activeWidgetsNumber; awi++) {
    const int wi = activeWidgets[awi];
    int (&indexRange)[2][2] = widgetIndexRange[wi];
    
    const float *pVAttenuation = widgetAttenuation[wi][0] - indexRange[0][L];
    const float *pGAttenuation = widgetAttenuation[wi][1] - indexRange[1][L];
    const float *pRAttenuation = widgetAttenuation[wi][2] - indexRange[1][L];

    for (int gi = indexRange[1][L]; gi <= indexRange[1][R]; gi++) {
      float *asum = attenuationSums + (gi - gmin) * newvsize + (indexRange[0][L] - vmin);
      const float ag = pGAttenuation[gi];
      const float ar = pRAttenuation[gi];

      for (int vi = indexRange[0][L]; vi <= indexRange[0][R]; vi++, asum++) {
        float av = ar > 0.0001f ? (1.f - (1.f - pVAttenuation[vi]) * ar) : 0.f;
        if (av < 0.f)
          av = 0.f;
        *asum += min(ag, av);
        }
      }
    }

  // sample the function
  for (awi = 0; awi < activeWidgetsNumber; awi++) {
    const int wi = activeWidgets[awi];
    int (&indexRange)[2][2] = widgetIndexRange[wi];

    const float opacity = useOpacity ? this->Widgets[wi].Opacity : 1.f;
    const float rgbd[4] = {255.f * this->Widgets[wi].Color[0], 255.f * this->Widgets[wi].Color[1], 255.f * this->Widgets[wi].Color[2], 255.f * this->Widgets[wi].Diffuse };
    const unsigned char rgbdByte[4] = {clip(int(255.f * this->Widgets[wi].Color[0]), 0, 255), clip(int(255.f * this->Widgets[wi].Color[1]), 0, 255), clip(int(255.f * this->Widgets[wi].Color[2]), 0, 255), clip(int(255.f * this->Widgets[wi].Diffuse), 0, 255)};
    const float *pVAttenuation = widgetAttenuation[wi][0] - indexRange[0][L];
    const float *pGAttenuation = widgetAttenuation[wi][1] - indexRange[1][L];
    const float *pRAttenuation = widgetAttenuation[wi][2] - indexRange[1][L];

    for (int gi = indexRange[1][L]; gi <= indexRange[1][R]; gi++) {
      int index = (vsize * gi + indexRange[0][L]);
      float         *opacityTablePtr = opacityTable + index;
      unsigned char *rgbdTablePtr    = rgbdTable + (index << 2);
      
      const float *asum   = attenuationSums + (gi - gmin) * newvsize + (indexRange[0][L] - vmin);
      const float ag      = pGAttenuation[gi];
      const float ar      = pRAttenuation[gi];

      for (int vi = indexRange[0][L]; vi <= indexRange[0][R]; vi++, asum++, opacityTablePtr++, rgbdTablePtr += 4) {
        if (*asum < 0.0001)
          continue;

        float av = ar > 0.0001f ? (1.f - (1.f - pVAttenuation[vi]) * ar) : 0.f;
        if (av < 0.f)
          av = 0.f;

        const float attenuation = min(ag, av);
        const float weight = attenuation == *asum ? 1.f : clip(attenuation / (*asum), 0.f, 1.f);
        opacityTablePtr[0] += (weight * attenuation * opacity);

        // this may results in some round-error. should be corrected if the error results in artefacts
        if (weight == 1.f) {
          for (int ci = 0; ci < 4; ci++)
            rgbdTablePtr[ci] = rgbdByte[ci];
          }
        else {
          for (int ci = 0; ci < 4; ci++) {
            const int val = int(float(rgbdTablePtr[ci]) + weight * rgbd[ci]);
            rgbdTablePtr[ci] = val > 255 ? 255 : unsigned char(val);
            }
          }
        }
      }
    }
  
  // free memory
  for (int ai = 0; ai < activeWidgetsNumber; ai++)
    delete [] widgetAttenuation[ai][0];
  delete [] attenuationSums;
  return true;
  }


//////////////////////////// volume property ////////////////////////////
//-----------------------------------------------------------------
vtkVolumeProperty2::vtkVolumeProperty2() {
  this->TransferFunction2D = NULL;
  }


//-----------------------------------------------------------------
vtkVolumeProperty2::~vtkVolumeProperty2() {
  if (this->TransferFunction2D != NULL)
    this->TransferFunction2D->Delete();
  }

//-----------------------------------------------------------------
void vtkVolumeProperty2::DeepCopy(vtkVolumeProperty *f ) {
  vtkVolumeProperty2 *prop = vtkVolumeProperty2::SafeDownCast(f);
  if (prop && prop != this) {
    if (prop->TransferFunction2D != this->TransferFunction2D) {
      if (this->TransferFunction2D != NULL)
        this->TransferFunction2D->Delete();
      this->TransferFunction2D = vtkTransferFunction2D::New();
      this->TransferFunction2D->DeepCopy(prop->TransferFunction2D);
      }
    this->SetShade(prop->GetShade());
    }
  }

void vtkVolumeProperty2::ShallowCopy(vtkVolumeProperty *f ) {
  vtkVolumeProperty2 *prop = vtkVolumeProperty2::SafeDownCast(f);
  if (prop && prop != this) {
    if (prop->TransferFunction2D != this->TransferFunction2D) {
      if (this->TransferFunction2D != NULL)
        this->TransferFunction2D->Delete();
      this->TransferFunction2D = prop->TransferFunction2D;
      this->TransferFunction2D->Register(this);
      }
    this->SetShade(prop->GetShade());
    }
  }

//-----------------------------------------------------------------
void vtkVolumeProperty2::SetTransferFunction2D(vtkTransferFunction2D *function) { 
  if (this->TransferFunction2D == function)
    return;
  if (this->TransferFunction2D != NULL)
    this->TransferFunction2D->Delete();
  this->TransferFunction2D = function;
  if (function != NULL)
    this->TransferFunction2D->Register(this);

  this->Modified();
  }

unsigned long int vtkVolumeProperty2::GetMTime() {
  unsigned long mTime = vtkVolumeProperty::GetMTime();
  
  if (this->TransferFunction2D) {
    unsigned long  mTimeTF = this->TransferFunction2D->GetMTime();
    mTime = max(mTime, mTimeTF);
    }
  
  return mTime;
  }

static const char *startString = "vtk volume property 2\n";
static const char *endString   = "end of vtk volume property 2\n";

//-----------------------------------------------------------------
char *vtkTransferFunction2D::SaveToString() { 
  strstream stream;
  
  stream << startString;
  // save transfer function
  for (int wi = 0; wi < this->GetNumberOfWidgets(); wi++) {
    const tfWidget &w = this->GetWidget(wi);
    stream << "  widget '" << w.Name << "'\n";
    stream << "    color:   " << w.Color[0] << ", " << w.Color[1] << ", " << w.Color[2] << "\n";
    stream << "    range:   " << w.Range[0][0] << ", " << w.Range[0][1] << ", " << w.Range[0][2] << "   " << w.Range[1][0] << ", " << w.Range[1][1] << ", " << w.Range[1][2] << "\n";
    stream << "    ratio:   " << w.Ratio << "\n";
    stream << "    ginterpolation:   " << w.GradientInterpolationOrder << "\n";
    stream << "    opacity: " << w.Opacity << "\n";
    stream << "    diffuse: " << w.Diffuse << "\n";
    stream << "    visible: " << (w.Visible ? 1 : 0) << "\n";
    stream << "  widget end\n";
    }
  stream << endString;

  char *string = new char[stream.pcount() + 64];
  strncpy(string, stream.str(), stream.pcount());
  string[stream.pcount()] = '\0';
  return string;
  }

//-----------------------------------------------------------------
bool vtkTransferFunction2D::LoadFromString(const char *string) { 
  if (string == NULL || strncmp(string, startString, strlen(startString)) != 0)
    return false;
  string += strlen(startString);

  while (this->RemoveWidget(0))
    { ; }

  tfWidget widget;
  while (strncmp(string, endString, strlen(endString)) != 0) {
    int visible = widget.Visible;

    while (*string == ' ')
      string++;
    if (*string == '\n') {
      string++;
      continue;
      }

    if (strncmp(string, "widget \'", strlen("widget \'")) == 0) {
      string = strchr(string, '\'');
      if (string == NULL || strchr(string, '\n') == NULL)
        return false;
      strncpy(widget.Name, string + 1, min(sizeof(widget.Name), int(strchr(string, '\n') - string) - 1) - 1);
      widget.Name[min(sizeof(widget.Name), int(strchr(string, '\n') - string) - 1) - 1] = '\0';
      }
    else if (strncmp(string, "widget end\n", strlen("widget end\n")) == 0) {
      if (this->AddWidget(widget) < 0)
        return false;
      }
    else if (sscanf(string, "color: %f, %f, %f", widget.Color, widget.Color + 1, widget.Color + 2) != 3 &&
      sscanf(string, "range: %f, %f, %f %f, %f, %f", widget.Range[0], widget.Range[0] + 1, widget.Range[0] + 2, widget.Range[1], widget.Range[1] + 1, widget.Range[1] + 2) != 6 &&
      sscanf(string, "ratio: %f", &widget.Ratio) != 1 &&
      sscanf(string, "ginterpolation: %d", &widget.GradientInterpolationOrder) != 1 &&
      sscanf(string, "opacity: %f", &widget.Opacity) != 1 &&
      sscanf(string, "diffuse: %f", &widget.Diffuse) != 1 &&
      sscanf(string, "visible: %d", &visible) != 1)
      return false;
    widget.Visible = visible != 0;

    string = strchr(string, '\n');
    if (string == NULL)
      return false;
    string++;
    }
  return true;
  }

