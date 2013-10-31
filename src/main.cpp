
#include "vector.h"
#include "model.h"
#include "bvh.h"

#include <iostream>
#include <memory>
#include "GL/glut.h"

using namespace std;

namespace {
  shared_ptr<Model> targetModel;
  Vector3 centerpos;
  shared_ptr<BVH> targetBVH;
  int currentDepth = 0;
  vector<BoundingBox> boxes;
  double rate = 1.0;
  double rotate_y = 0;
  double rotate_x = 0;
}

void updateBoundingBoxes()
{
  if (!targetBVH) return;

  targetBVH->CollectBoundingBoxes(currentDepth, boxes);
  cerr << "current depth of bounding boxes = " << currentDepth << endl;
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

  updateBoundingBoxes();
}

void resize(int w, int h)
{
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)w/(double)h, 1.0, 10000.0);

  glMatrixMode(GL_MODELVIEW);
}


void display()
{
  //glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  /* éãì_à íuÇ∆éãê¸ï˚å¸ */
  gluLookAt(0.0, 0.001, -100, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

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

  // draw model
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

  glDisable(GL_LIGHTING);

  // draw bounding boxes
  const int colorlistnum = 8;
  GLfloat colors[colorlistnum][4] = {
    {0.1,0.1,0.1},
    {0.1,0.1,0.9},
    {0.1,0.9,0.1},
    {0.9,0.1,0.1},
    {0.1,0.9,0.9},
    {0.9,0.9,0.1},
    {0.9,0.1,0.9},
    {0.9,0.9,0.9}
  };
  for (size_t i=0; i<boxes.size(); i++) {
    const BoundingBox &b = boxes[i];
    Vector3 min = (b.min() - centerpos)*rate;
    Vector3 max = (b.max() - centerpos)*rate;
    GLfloat pos[][4] = {{min.x, min.y, min.z, 0.0},
      {max.x, min.y, min.z, 0.0},
      {min.x, max.y, min.z, 0.0},
      {min.x, min.y, max.z, 0.0},
      {max.x, max.y, min.z, 0.0},
      {max.x, min.y, max.z, 0.0},
      {min.x, max.y, max.z, 0.0},
      {max.x, max.y, max.z, 0.0}};

    const int indices[12][2] = {
      {0,1},{0,3},{5,1},{5,3},{2,4},{2,6},{7,4},{7,6},{0,2},{1,4},{3,6},{5,7}
    };

    for (int j=0; j<12; j++) {
      glBegin(GL_LINES);
      glColor3fv(colors[i]);
      glVertex3fv(pos[indices[j][0]]);
      glColor3fv(colors[i]);
      glVertex3fv(pos[indices[j][1]]);
      glEnd();
    }
  }

  glutSwapBuffers();
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
    cerr << "current scaling = " << rate << endl;
    glutPostRedisplay();
    break;
  case '-':
    rate = std::max(rate-0.1, 0.001);
    cerr << "current scaling = " << rate << endl;
    glutPostRedisplay();
    break;

  case 'n':
    currentDepth++;
    updateBoundingBoxes();
    break;

  case 'p':
    currentDepth = std::max(currentDepth-1, 0);
    updateBoundingBoxes();
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

void init(void)
{
  glClearColor(1,1,1,1);

  targetModel.reset(new Model);
  if (!targetModel->readFromObj("torii.obj")) {
    cerr << "Failed to load the model" << endl;
    getchar();
    exit(-1);
  }

  glEnable(GL_DEPTH_TEST);


  constructBVH();
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
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
