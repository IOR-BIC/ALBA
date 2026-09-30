/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATransferFunction2D
 Authors: Alexander Savenko, Mel Krokos
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBATransferFunction2D - 2D transfer function
// .SECTION Description
// This class is based on Kniss paper: "Interactive Volume Rendering Using Multi-Dimensional Transfer Functions and Direct Manipulation Widgets".

// .SECTION See Also
// vtkVolumeProperty vtkAdaptiveVolumeMapper 

#ifndef __vtkALBATransferFunction2D_h
#define __vtkALBATransferFunction2D_h

#include "assert.h"

#include "vtkDataObject.h"
#include "vtkVolumeProperty.h"
#include "vtkImageMapToColors.h"

#include "albaConfigure.h"

#define VTK_TRANSFER_FUNCTION_2D 20

struct tfWidget {
  char   Name[32];

  int    GradientInterpolationOrder; // 0 - no interpolation, 1 linear

  // position
  double  Ratio;
  double  Range[2][3];  // 0 - left, 1 - right, 2 - center
  
  // value
  double  Color[3];
  double  Opacity;
  double  Diffuse;

  bool   Visible;

  // data calculated from the above parameters
  double  LengthI[2][2]; // 1 / (Range[x][i] - Range[x][1])
  double  RatioI;

  // methods
  tfWidget() { this->Name[0] = '\0'; this->GradientInterpolationOrder = 0; this->Ratio = 1.f; this->Opacity = 1.f; this->Diffuse = 0.f; this->Color[0] = this->Color[1] = this->Color[2] = 1.f; this->Visible = true; }

  void   SetValueRange(double from, double to, double center) { Range[0][0] = from; Range[0][1] = to; Range[0][2] = center; }
  void   SetGradientRange(double from, double to, double center) { Range[1][0] = from; Range[1][1] = to; Range[1][2] = center; }

  bool   Inside(double val, double grad) const { return (val >= this->Range[0][0] && val <= this->Range[0][1] && grad >= this->Range[1][0] && grad <= this->Range[1][1]); }
  ALBA_EXPORT double  Attenuation(double val, double grad) const;

  void   Update();
  };


#define MAX_NUMBER_OF_WIDGETS 16

class ALBA_EXPORT vtkALBATransferFunction2D : public vtkDataObject {
  public:
    static vtkALBATransferFunction2D *New();
    vtkTypeMacro(vtkALBATransferFunction2D,vtkDataObject);
    
    void Initialize();
    void DeepCopy( vtkDataObject *f ) { this->ShallowCopy(f); }
    void ShallowCopy( vtkDataObject *f );
    
    /**
    Return what type of dataset this is.*/
    int GetDataObjectType() {return VTK_TRANSFER_FUNCTION_2D;};
    
    /**
    Get number of widgets. Some of them can be not-active*/
    int GetNumberOfWidgets() const { return this->NumberOfWidgets;}

    /**
    Add new widget and return its index. If no more widgets can be added than -1 is returned*/
    int  AddWidget(const tfWidget &widget);

    /**
    Remove widget. all indeces become invalid*/
    bool RemoveWidget(int index);
  
    /**
    Set/get widget parameters*/
    bool            SetWidget(int index, const tfWidget &widget);
    const tfWidget& GetWidget(int n) const { assert(n < this->NumberOfWidgets); return this->Widgets[n]; }
    
    bool  SetWidgetName(int index, const char *name) { strncpy(this->Widgets[index].Name, name, sizeof(this->Widgets[index].Name) - 1); return true; }
    const char *GetWidgetName(int index) const { return this->Widgets[index].Name; }
    
    bool SetWidgetVisibility(int index, bool visible);
    bool GetWidgetVisibility(int index) const { return this->Widgets[index].Visible; }

    bool  SetWidgetOpacity(int index, double opacity);
    double GetWidgetOpacity(int index) const  { return this->Widgets[index].Opacity; }

    bool         SetWidgetColor(int index, double color[3]);
    const double* GetWidgetColor(int index) const {return this->Widgets[index].Color;}

    bool  SetWidgetDiffuse(int index, double diffuse);
    double GetWidgetDiffuse(int index) const { return this->Widgets[index].Diffuse;}
    
    bool  SetWidgetValueRatio(int index, double ratio);
    double GetWidgetValueRatio(int index) const { return this->Widgets[index].Ratio;}

    bool SetWidgetValueRange(int index, double range[3]);
    const double * GetWidgetValueRange(int index) const { return this->Widgets[index].Range[0];}

    bool SetWidgetGradientRange(int index, double range[3]);
    const double *GetWidgetGradientRange(int index) const { return this->Widgets[index].Range[1];}

    bool SetWidgetGradientInterpolation(int index, int order);
    int  GetWidgetGradientInterpolation(int index) const { return this->Widgets[index].GradientInterpolationOrder; }

    // Description:
    // Check widget fields
    static bool CheckWidget(const tfWidget &widget);

    // Description:
    // Returns the segments on which widgets are defined. Diffuse segement is defined where diffuse coefficient is not zero
    const double *GetRange() { this->UpdateRanges(); return this->ValueRange; }
    const double *GetGradientRange() { this->UpdateRanges(); return this->GradientRange; }
    
    // Description:
    // Returns opacity and color for given pair of value and gradient. This is slow method and it should not
    // be use if speed is important. Renderer should only use GetTable methods.
    double GetValue(const double val, const double gradient, double &opacity, double color[3], double &diffuse);
 
    // Description:
    // These methods should be used by a volume mapper to sample transfer function
    bool GetTable(int vsize, const double *vTable, int gsize, const double *gTable, double *opacityTable, unsigned char *rgbdTable, bool useOpacity = true);

    void Modified(double from = VTK_DOUBLE_MIN, double to = VTK_DOUBLE_MAX);

    // Description:
    // Save/Load transfer function to/from a string. The caller is responsible to delete the string.
    char *SaveToString();
    bool LoadFromString(const char* string);

  protected:
    vtkALBATransferFunction2D();
    ~vtkALBATransferFunction2D();
 
    tfWidget  Widgets[MAX_NUMBER_OF_WIDGETS];
    int       NumberOfWidgets;

    double     UpdateRange[2];

    bool      RangeValid;
    double     ValueRange[2];
    double     GradientRange[2];

    void      UpdateRanges();

  private:
    vtkALBATransferFunction2D(const vtkALBATransferFunction2D&);  // Not implemented.
    void operator=(const vtkALBATransferFunction2D&);  // Not implemented.
};


class ALBA_EXPORT vtkVolumeProperty2 : public vtkVolumeProperty {
  public:
    static vtkVolumeProperty2 *New();
    vtkTypeMacro(vtkVolumeProperty2, vtkVolumeProperty);

    void SetTransferFunction2D(vtkALBATransferFunction2D *function);
    vtkALBATransferFunction2D *GetTransferFunction2D() const { return this->TransferFunction2D; }

		vtkMTimeType GetMTime();

    void DeepCopy(vtkVolumeProperty *f );
    void ShallowCopy(vtkVolumeProperty *f );

  protected:
    vtkVolumeProperty2();
    ~vtkVolumeProperty2();

    vtkALBATransferFunction2D *TransferFunction2D;
  
  private:
    vtkVolumeProperty2(const vtkVolumeProperty2&);  // Not implemented.
    void operator=(const vtkVolumeProperty2&);  // Not implemented.
  };



#endif

