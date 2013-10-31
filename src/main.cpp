
#include "vector.h"
#include "model.h"
#include "bvh.h"

#include <iostream>
#include <memory>
#include "GL/glut.h"

using namespace std;

shared_ptr<Model> targetModel;
Vector3 centerpos;
shared_ptr<BVH> targetBVH;

void resize(int w, int h)
{
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)w/(double)h, 1.0, 10000.0);

  glMatrixMode(GL_MODELVIEW);
}

double rate = 1.0;
double rotate_y = 0;
double rotate_x = 0;


void display()
{
  //glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  /* éãì_à íuÇ∆éãê¸ï˚å¸ */
  gluLookAt(0.0, 0.001, -100, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  GLfloat light0Pos[] = {0,100,0,1};
  GLfloat light0Color[] = {1,1,1,1};
  glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Color);

  /* ê}å`ÇÃâÒì] */
  glRotated(rotate_y, 0.0, 1.0, 0.0);
  glRotated(rotate_x, 1.0, 0.0, 0.0);

  // ambient
  GLfloat ambient[] = {0.5, 0.5, 0.5, 1.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambient);

  if (targetModel) {
    for (int i=0; i<targetModel->getMaterialCount(); i++) {
      const Material &mat = targetModel->getMaterial(i);
      const Model::PolygonList &objs = targetModel->getPolygonList(mat);
      for (int j=0; j<objs.size(); j++) {
        glBegin(GL_POLYGON);
        glNormal3d(-objs[j]->m_normal.x, -objs[j]->m_normal.y, -objs[j]->m_normal.z);
        glVertex3d((objs[j]->m_pos[0].x-centerpos.x)*rate, (objs[j]->m_pos[0].y-centerpos.y)*rate, (objs[j]->m_pos[0].z-centerpos.z)*rate);
        glVertex3d((objs[j]->m_pos[1].x-centerpos.x)*rate, (objs[j]->m_pos[1].y-centerpos.y)*rate, (objs[j]->m_pos[1].z-centerpos.z)*rate);
        glVertex3d((objs[j]->m_pos[2].x-centerpos.x)*rate, (objs[j]->m_pos[2].y-centerpos.y)*rate, (objs[j]->m_pos[2].z-centerpos.z)*rate);
        glEnd();
      }
    }
  }

  glFlush();
}

void idle(void)
{
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case '+':
    rate += 0.1;
    glutPostRedisplay();
    break;
  case '-':
    rate = std::max(rate-0.1, 0.001);
    glutPostRedisplay();
    break;

  }
}

void keyboardSpecial(int key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_LEFT:
    // left arrow
    rotate_y -= 10;
    break;

  case GLUT_KEY_RIGHT:
    // right arrow
    rotate_y += 10;
    break;

  case GLUT_KEY_UP:
    rotate_x -= 10;
    break;

  case GLUT_KEY_DOWN:
    rotate_x += 10;
    break;
  }
}

void constructBVH()
{
  if (!targetModel) {
    cerr << "no model" << endl;
    return;
  }

  targetBVH.reset(new BVH);

  vector<SceneObject *> targets;
  centerpos = Vector3::Zero();
  int num = 0;
  for (int i=0; i<targetModel->getMaterialCount(); i++) {
    const Material &mat = targetModel->getMaterial(i);
    const Model::PolygonList &objs = targetModel->getPolygonList(mat);
    for (int j=0; j<objs.size(); j++) {
      targets.push_back(objs[j]);
      num++;
      Vector3 polygonpos = (objs[j]->m_pos[0]+objs[j]->m_pos[1]+objs[j]->m_pos[2])/3.0;
      centerpos += polygonpos;
    }
  }
  centerpos /= num;

  targetBVH->Construct(BVH::CONSTRUCTION_OBJECT_SAH, targets);
}

void init(void)
{
  glClearColor(0,0,1,1);

  targetModel.reset(new Model);
  if (!targetModel->readFromObj("torii.obj")) {
    cerr << "Failed to load the model" << endl;
    getchar();
    exit(-1);
  }

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  constructBVH();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(640, 480);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecial);
  glutIdleFunc(idle);
  glDisable(GL_CULL_FACE) ;
  init();
  glutMainLoop();

  return 0;
}
