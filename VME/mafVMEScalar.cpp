/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2006-10-23 13:21:40 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafVMEScalar.h"
#include "mmgGui.h"

#include "mafStorageElement.h"
#include "mafVME.h"
#include "mafVMEOutputScalar.h"

#include "mafScalarInterpolator.h"
#include "mafDataVector.h"
#include "mafVMEItemScalar.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEScalar)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEScalar::mafVMEScalar()
//-------------------------------------------------------------------------
{
  m_DataVector = mafDataVector::New();
  m_DataVector->SetItemTypeName(mafVMEItemScalar::GetStaticTypeName());  
  m_DataVector->SetListener(this);
  SetDataPipe(mafScalarInterpolator::New()); // interpolator data pipe

  m_ScalarArrayOrientationInMatrix = ROWS;
  m_Xtype = USE_TIME;
  m_Ytype = USE_PROGRESS_NUMBER;
  m_Ztype = USE_SCALAR;
  m_XID = 0;
  m_YID = 0;
  m_ZID = 0;
}

//-------------------------------------------------------------------------
mafVMEScalar::~mafVMEScalar()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int mafVMEScalar::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(a);
    this->SetTypeForXCoordinates(scalar->GetTypeForXCoordinates());
    this->SetTypeForYCoordinates(scalar->GetTypeForYCoordinates());
    this->SetTypeForZCoordinates(scalar->GetTypeForZCoordinates());
    this->SetScalarArrayOrientation(scalar->GetScalarArrayOrientation());
    this->SetScalarIdForXCoordinate(scalar->GetScalarIdForXCoordinate());
    this->SetScalarIdForYCoordinate(scalar->GetScalarIdForYCoordinate());
    this->SetScalarIdForZCoordinate(scalar->GetScalarIdForZCoordinate());

    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEScalar::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(vme);
    ret = this->m_Xtype == scalar->GetTypeForXCoordinates() && \
          this->m_Ytype == scalar->GetTypeForYCoordinates() && \
          this->m_Ztype == scalar->GetTypeForZCoordinates() && \
          this->m_XID == scalar->GetScalarIdForXCoordinate() && \
          this->m_YID == scalar->GetScalarIdForYCoordinate() && \
          this->m_ZID == scalar->GetScalarIdForZCoordinate() && \
          this->m_ScalarArrayOrientationInMatrix == scalar->GetScalarArrayOrientation();
  }
  return ret;
}
//-------------------------------------------------------------------------
mafVMEOutput *mafVMEScalar::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputScalar::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEScalar::SetData(vnl_matrix<double> &data, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemScalar> item;
  item->SetData(data);
  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  return MAF_OK;
}
//-------------------------------------------------------------------------
mmgGui* mafVMEScalar::CreateGui()
//-------------------------------------------------------------------------
{
  wxString type_array[3] = {_("time"), _("progress number"), _("scalar ID")};
  
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Label(_("Parameters used for"));
  m_Gui->Label(_("VTK representation"));
  m_Gui->Divider(2);
  m_Gui->Label(_("x coord"));
  m_Gui->Combo(ID_TYPE_FOR_X,_("type"),&m_Xtype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_X,_("scalar id"),&m_XID,0,MAXINT,_("Scalar ID associated to the X coordinate \nof the VTK representation"));
  m_Gui->Label(_("y coord"));
  m_Gui->Combo(ID_TYPE_FOR_Y,_("type"),&m_Ytype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_Y,_("scalar id"),&m_YID,0,MAXINT,_("Scalar ID associated to the Y coordinate \nof the VTK representation"));
  m_Gui->Label(_("z coord"));
  m_Gui->Combo(ID_TYPE_FOR_Z,_("type"),&m_Ztype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_Z,_("scalar id"),&m_ZID,0,MAXINT,_("Scalar ID associated to the Z coordinate \nof the VTK representation"));
  m_Gui->Divider();

  EnableWidgets();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEScalar::EnableWidgets()
//-------------------------------------------------------------------------
{
  m_Gui->Enable(ID_SCALAR_FOR_X, m_Xtype == USE_SCALAR);
  m_Gui->Enable(ID_SCALAR_FOR_Y, m_Ytype == USE_SCALAR);
  m_Gui->Enable(ID_SCALAR_FOR_Z, m_Ztype == USE_SCALAR);
}
//-------------------------------------------------------------------------
void mafVMEScalar::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_SCALAR_FOR_X:
      case ID_SCALAR_FOR_Y:
      case ID_SCALAR_FOR_Z:
      break;
      case ID_TYPE_FOR_X:
      case ID_TYPE_FOR_Y:
      case ID_TYPE_FOR_Z:
        EnableWidgets();
      break;
      default:
        mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-----------------------------------------------------------------------
int mafVMEScalar::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    if (parent->StoreInteger("XID",m_XID)==MAF_OK &&
        parent->StoreInteger("YID",m_YID)==MAF_OK &&
        parent->StoreInteger("ZID",m_ZID)==MAF_OK &&
        parent->StoreInteger("Xtype",m_Xtype)==MAF_OK &&
        parent->StoreInteger("Ytype",m_Ytype)==MAF_OK &&
        parent->StoreInteger("Ztype",m_Ztype)==MAF_OK &&
        parent->StoreInteger("ScalarArrayOrientationInMatrix",m_ScalarArrayOrientationInMatrix)==MAF_OK)
      return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEScalar::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    if (node->RestoreInteger("XID",m_XID) == MAF_OK &&
        node->RestoreInteger("YID",m_YID) == MAF_OK &&
        node->RestoreInteger("ZID",m_ZID) == MAF_OK &&
        node->RestoreInteger("Xtype",m_Xtype) == MAF_OK &&
        node->RestoreInteger("Ytype",m_Ytype) == MAF_OK &&
        node->RestoreInteger("Ztype",m_Ztype) == MAF_OK &&
        node->RestoreInteger("ScalarArrayOrientationInMatrix",m_ScalarArrayOrientationInMatrix) == MAF_OK)
    {
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetScalarArrayOrientation(int o)
//-------------------------------------------------------------------------
{
  m_ScalarArrayOrientationInMatrix = o;
  m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix < ROWS ? ROWS : m_ScalarArrayOrientationInMatrix;
  m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix > COLUMNS ? COLUMNS : m_ScalarArrayOrientationInMatrix;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetScalarIdForXCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_XID == id)
  {
    return;
  }
  m_XID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalar::GetScalarIdForXCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalar *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_XID = m_XID > data.columns() ? data.columns() : m_XID;
    }
    else
    {
      m_XID = m_XID > data.rows() ? data.rows() : m_XID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_XID;
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetTypeForXCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Xtype == t)
  {
    return;
  }
  m_Xtype = t;
  m_Xtype = m_Xtype < USE_TIME ? USE_TIME : m_Xtype;
  m_Xtype = m_Xtype > USE_SCALAR ? USE_SCALAR : m_Xtype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetScalarIdForYCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_YID == id)
  {
    return;
  }
  m_YID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalar::GetScalarIdForYCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalar *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_YID = m_YID > data.columns() ? data.columns() : m_YID;
    }
    else
    {
      m_YID = m_YID > data.rows() ? data.rows() : m_YID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_YID;
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetTypeForYCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Ytype == t)
  {
    return;
  }
  m_Ytype = t;
  m_Ytype = m_Ytype < USE_TIME ? USE_TIME : m_Ytype;
  m_Ytype = m_Ytype > USE_SCALAR ? USE_SCALAR : m_Ytype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetScalarIdForZCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_ZID == id)
  {
    return;
  }
  m_ZID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalar::GetScalarIdForZCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalar *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_ZID = m_ZID > data.columns() ? data.columns() : m_ZID;
    }
    else
    {
      m_ZID = m_ZID > data.rows() ? data.rows() : m_ZID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_ZID;
}
//-------------------------------------------------------------------------
void mafVMEScalar::SetTypeForZCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Ztype == t)
  {
    return;
  }
  m_Ztype = t;
  m_Ztype = m_Ztype < USE_TIME ? USE_TIME : m_Ztype;
  m_Ztype = m_Ztype > USE_SCALAR ? USE_SCALAR : m_Ztype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEScalar::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << _("Scalar Array orientation in matrix: ");
  os << (m_ScalarArrayOrientationInMatrix == 0) ? _("Rows") : _("Columns");
  os << _("\n");

  mafString type_scalar[3] = {_("Time"), _("Progress numbers"), _("Scalars")};

  os << indent << _("Scalar type for X coordinate: ");
  os << type_scalar[m_Xtype];
  os << _("\n");

  if (m_Xtype == USE_SCALAR)
  {
    os << indent << _("Scalar ID for X coordinate: ");
    os << m_XID;
    os << _("\n");
  }

  os << indent << _("Scalar type for Y coordinate: ");
  os << type_scalar[m_Ytype];
  os << _("\n");

  if (m_Ytype == USE_SCALAR)
  {
    os << indent << _("Scalar ID for Y coordinate: ");
    os << m_YID;
    os << _("\n");
  }

  os << indent << _("Scalar type for Z coordinate: ");
  os << type_scalar[m_Ztype];
  os << _("\n");

  if (m_Ztype == USE_SCALAR)
  {
    os << indent << _("Scalar ID for Z coordinate: ");
    os << m_ZID;
    os << _("\n");
  }
}
//-------------------------------------------------------------------------
char** mafVMEScalar::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEScalar.xpm"
  return mafVMEScalar_xpm;
}
