// ===================================================
// Esempi OSG : Esempio 2
//
// carica un terreno
//  --- lo trovi in pc-imboden/sharedarea/fanini/grandCanyon
//
// istanzia un manipolatore di esempio
//
// il manipolatore mostra anche un esempio d'uso dell'intersect visitor
//
//===================================================
#include "common.h"
//#include "sampleManipulator.h"
#include <string>
#include <stdlib.h> // per getenv
#include <osgDB/ReadFile>

#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Drawable>
#include <osg/StateSet>

#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>
#include "shaders.h"

//----------------------------------------------------
// costruisce una palla
osg::Geode* GetGeode( )
//----------------------------------------------------
{
  // drawable
  double radius = 0.7;
  osg::Sphere* sphereShape = new osg::Sphere(osg::Vec3d(0,0,0),radius);
  osg::ShapeDrawable* sphereDrawable = new osg::ShapeDrawable(sphereShape);

  // stateset
  osg::StateSet* stateSet = new osg::StateSet();
  stateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

  // geode 
  osg::Geode* geode = new osg::Geode();
  geode->setStateSet(stateSet);
  geode->addDrawable(sphereDrawable);
  return geode;
}
//----------------------------------------------------
// aggancia uno shader ad un geode
void AttachPlasticShader( osg::Geode *geode )
//----------------------------------------------------
{
  // shaders
  osg::Shader* vertObj = new osg::Shader( osg::Shader::VERTEX );
  osg::Shader* fragObj = new osg::Shader( osg::Shader::FRAGMENT );
  vertObj->setShaderSource(plastic_vert);
  fragObj->setShaderSource(plastic_frag);

  // program
  osg::Program *program = new osg::Program;
  program->addShader( vertObj );
  program->addShader( fragObj );

  // stateset
  osg::StateSet* stateSet = geode->getOrCreateStateSet();
  stateSet->setAttributeAndModes(program, osg::StateAttribute::ON);

  // uniforms
  stateSet->addUniform( new osg::Uniform("view_position", osg::Vec3f(0.0f, 0.0f, 1.0f)) );
  stateSet->addUniform( new osg::Uniform("color",         osg::Vec4f(0.2f, 0.5f, 1.0f, 1.0f)) );
}
//----------------------------------------------------
// aggancia uno shader ad un geode
void AttachMiniShader( osg::Geode *geode )
//----------------------------------------------------
{
  // shaders
  osg::Shader* vertObj = new osg::Shader( osg::Shader::VERTEX );
  osg::Shader* fragObj = new osg::Shader( osg::Shader::FRAGMENT );
  vertObj->setShaderSource(mini_vert);
  fragObj->setShaderSource(mini_frag);

  // program
  osg::Program *program = new osg::Program;
  program->addShader( vertObj );
  program->addShader( fragObj );

  // stateset
  osg::StateSet* stateSet = geode->getOrCreateStateSet();
  stateSet->setAttributeAndModes(program, osg::StateAttribute::ON);
}
//----------------------------------------------------
int main( int argc, char **argv )
//----------------------------------------------------
{
   InitViewer();

   osg::Geode *geode = GetGeode();

   // scommenta uno dei seguenti
   //AttachPlasticShader( geode );
   AttachMiniShader( geode );

   SetSceneGraph( geode );

   Loop();
}

