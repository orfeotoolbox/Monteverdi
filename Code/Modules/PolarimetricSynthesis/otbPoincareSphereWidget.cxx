/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#include "otbPoincareSphereWidget.h"
#include "otbMath.h"
#include "FL/fl_draw.H"

namespace otb
{
/**
 * Constructor.
 */
PoincareSphereWidget::PoincareSphereWidget()
    : Fl_Gl_Window(0,0,0,0,0)
{
  m_Resolution = 30;
  m_GridSize = 15;
  m_RotateAngle1 = -10;
  m_RotateAngle2 = -45;
  m_PsiI = 45;
  m_KhiI = 45;
  m_PsiR = -128;
  m_KhiR = 128;
}
/**
 * Destructor.
 */
PoincareSphereWidget::~PoincareSphereWidget()
{
}
/**
 * Show The widget.
 */
void PoincareSphereWidget::Init(int x, int y, int w, int h, const char * l)
{
  this->label(l);
  this->resize(x,y,w,h);
}

int PoincareSphereWidget::handle(int event)
{
  switch (event)
  {
  case FL_FOCUS:
    return 1;
  case FL_UNFOCUS:
    return 1;
  case FL_KEYBOARD:
    switch (Fl::event_key())
    {
    case FL_Left:
      m_RotateAngle1+=10;
      this->redraw();
      break;
    case FL_Right:
      m_RotateAngle1-=10;
      this->redraw();
      break;
    case FL_Up:
      m_RotateAngle2-=10;
      this->redraw();
      break;
    case FL_Down:
      m_RotateAngle2+=10;
      this->redraw();
      break;
    }
    return 1;
  case FL_PUSH:
    m_ClickedX = Fl::event_x();
    m_ClickedY = Fl::event_y();
    return 1;
  case FL_DRAG:
    m_RotateAngle1-=(m_ClickedX-Fl::event_x());
    m_RotateAngle2-=(m_ClickedY-Fl::event_y());
    m_ClickedX = Fl::event_x();
    m_ClickedY = Fl::event_y();
    this->redraw();
    return 1;
  case FL_RELEASE:
    m_ClickedX = 0;
    m_ClickedY = 0;
    return 1;
  }
  return Fl_Gl_Window::handle(event);
}

/**
 * Draw the widget
 */
void PoincareSphereWidget::draw(void)
{
  if (!this->valid())
  {
    valid(1);
    glLoadIdentity();
    unsigned int viewPortSize = std::min(w(),h());
    glViewport(0,0,viewPortSize,viewPortSize);
  }

  InitGL();



  glRotatef(m_RotateAngle2,1,0,0);
  glRotatef(m_RotateAngle1,0,0,1);

  DrawSphere();

  DrawTorus();

  glDisable(GL_LIGHTING);

  //  DrawPlans();
  DrawAxis();
  DrawVectors();
}

void PoincareSphereWidget::InitGL()
{
  glClearDepth(1.0);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glShadeModel(GL_SMOOTH);

  glClearColor((float)1.0, (float)1.0, (float)1.0, (float)1.0);
  glClear(GL_DEPTH_BUFFER_BIT);    //this clears and paints to black
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_BLEND);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.3,1.3,-1.3,1.3,-1.3,1.3);
}

void PoincareSphereWidget::ToggleLight()
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  const GLfloat paramLight[4] = {1.0,1.0,1.0,1.0};
  const GLfloat paramAmbient[4] = {0.5,0.5,0.5,1.0};

  glLightfv(GL_LIGHT0,GL_DIFFUSE,paramLight);
  glLightfv(GL_LIGHT1,GL_AMBIENT,paramAmbient);
}

void PoincareSphereWidget::DrawSphere()
{
  glPushMatrix();
  glColor4d(0.75,0.75,0.75,0.5);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  this->RenderSphere(1.,m_GridSize,m_Resolution);
  glPopMatrix();
}

void PoincareSphereWidget::DrawAxis()
{
  // axis
  glPushMatrix();
  glLineWidth(2);
  glBegin(GL_LINES);
  glColor3d(1,0,0);
  glVertex2f(-1,0);
  glVertex2f(1,0);
  glColor3d(0,1,0);
  glVertex2f(0,-1);
  glVertex2f(0,1);
  glColor3d(0,0,1);
  glVertex3f(0,0,-1);
  glVertex3f(0,0,1);
  glEnd();
  glLineWidth(1);

  gl_font(fl_font(),12);

  glColor3d(1,0,0);
  glRasterPos3f(-1.2,0,0);
  gl_draw("VL",2);
  glRasterPos3f(1.1,0,0);
  gl_draw("HL",2);
  glColor3d(0,0,1);
  glRasterPos3f(0,0,-1.2);
  gl_draw("LC",2);
  glRasterPos3f(0,0,1.1);
  gl_draw("RC",2);

  glPopMatrix();
}

void PoincareSphereWidget::DrawPlans()
{

  glPushMatrix();
  // Bleu
  glColor4d(0.9,0.9,1.0,1);
  glTranslatef(0.0,0.0,-0.8);
  glNormal3i(0,0,1);
  glRectf(-0.8,-0.8,0.8,0.8);

  // Vert
  glColor4d(.9,1.0,0.9,1);
  glRotatef(90,1,0,0);
  glTranslatef(-0.8,0.0,-0.8);
  glNormal3i(0,0,1);
  glRectf(0,0,1.6,1.6);

  // Rouge
  glColor4d(1.0,0.9,0.9,1);
  glRotatef(-90,0,1,0);
  glNormal3i(0,0,1);
  glRectf(0,0,1.6,1.6);
  glPopMatrix();

}

void PoincareSphereWidget::DrawTorus()
{
  glLineWidth(2);
  glPushMatrix();
  glColor4d(0.25,0.25,0.25,1.);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  this->RenderCircle(1., m_Resolution);

  glPushMatrix();
  glRotatef(90,1,0,0);
  this->RenderCircle(1., m_Resolution);
  glPopMatrix();

  glPushMatrix();
  glRotatef(90,0,1,0);
  this->RenderCircle(1., m_Resolution);
  glPopMatrix();

  glPopMatrix();
}

void PoincareSphereWidget::DrawVectors()
{
  double radius =1.2;
  double DTOR = CONST_PI_180;

  double xi = radius * vcl_sin(DTOR * (90-m_KhiI))*vcl_cos(DTOR * m_PsiI);
  double yi = radius * vcl_sin(DTOR * (90-m_KhiI))*vcl_sin(DTOR * m_PsiI);
  double zi = radius * vcl_cos(DTOR * (90-m_KhiI));


  double xr = radius * vcl_sin(DTOR * (90-m_KhiR))*vcl_cos(DTOR * m_PsiR);
  double yr = radius * vcl_sin(DTOR * (90-m_KhiR))*vcl_sin(DTOR * m_PsiR);
  double zr = radius * vcl_cos(DTOR * (90-m_KhiR));

  glPushMatrix();

  glLineWidth(3);
  glBegin(GL_LINES);

  glColor3d(1,0.647,0);
  glVertex3d(0,0,0);
  glVertex3d(xi,yi,zi);
  glColor3d(0.627,0.125,0.94);
  glVertex3d(0,0,0);
  glVertex3d(xr,yr,zr);
  glEnd();
  glLineWidth(1);

  glPopMatrix();
}

void PoincareSphereWidget::RenderSphere(double radius, unsigned int thetaResolution, unsigned int phiResolution)
{
  double thetaStep = CONST_PI/thetaResolution;
  double phiStep = CONST_2PI/phiResolution;

  for (unsigned int j = 0; j <= phiResolution; ++j)
  {
    glBegin(GL_LINE_LOOP);
    for (unsigned int i = 0; i <= thetaResolution; ++i)
    {
      double x = radius * vcl_cos(-CONST_PI/2+static_cast<double>(i)*thetaStep)*vcl_cos(-CONST_PI+static_cast<double>(j)*phiStep);
      double y = radius * vcl_cos(-CONST_PI/2+static_cast<double>(i)*thetaStep)*vcl_sin(-CONST_PI+static_cast<double>(j)*phiStep);
      double z = radius * vcl_sin(-CONST_PI/2+static_cast<double>(i)*thetaStep);
      glVertex3d(x,y,z);
    }
    glEnd();
  }

  for (unsigned int i = 0; i <= thetaResolution; ++i)
  {
    glBegin(GL_LINE_LOOP);

    for (unsigned int j = 0; j <= phiResolution; ++j)
    {
      double x = radius * vcl_cos(-CONST_PI/2+static_cast<double>(i)*thetaStep)*vcl_cos(-CONST_PI+static_cast<double>(j)*phiStep);
      double y = radius * vcl_cos(-CONST_PI/2+static_cast<double>(i)*thetaStep)*vcl_sin(-CONST_PI+static_cast<double>(j)*phiStep);
      double z = radius * vcl_sin(-CONST_PI/2+static_cast<double>(i)*thetaStep);
      glVertex3d(x,y,z);
    }
    glEnd();
  }
}

void PoincareSphereWidget::RenderCircle(double radius, unsigned int resolution)
{
  double step = CONST_2PI/resolution;

  glBegin(GL_LINE_LOOP);
  for (unsigned int i = 0; i <= resolution; ++i)
  {
    double x = radius * vcl_cos(static_cast<double>(i)*step);
    double y = radius * vcl_sin(static_cast<double>(i)*step);
    glVertex2d(x,y);
  }
  glEnd();
}

}// end namespace otb
