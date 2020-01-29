#include <GL/glut.h>

GLdouble vertex[][3] = {
  { 0.0, 0.0, 0.0 },
  { 1.0, 0.0, 0.0 },
  { 1.0, 1.0, 0.0 },
  { 0.0, 1.0, 0.0 },
  { 0.0, 0.0, 1.0 },
  { 1.0, 0.0, 1.0 },
  { 1.0, 1.0, 1.0 },
  { 0.0, 1.0, 1.0 }
};

int edge[][2] = {
  { 0, 1 },
  { 1, 2 },
  { 2, 3 },
  { 3, 0 },
  { 4, 5 },
  { 5, 6 },
  { 6, 7 },
  { 7, 4 },
  { 0, 4 },
  { 1, 5 },
  { 2, 6 },
  { 3, 7 }
};

int face[][4] = {
  { 0, 1, 2, 3 },
  { 1, 5, 6, 2 },
  { 5, 4, 7, 6 },
  { 4, 0, 3, 7 },
  { 4, 5, 1, 0 },
  { 3, 2, 6, 7 }
};

GLdouble color[][3] = {
  { 1.0, 0.0, 0.0 }, /* 赤 */
  { 0.0, 1.0, 0.0 }, /* 緑 */
  { 0.0, 0.0, 1.0 }, /* 青 */
  { 1.0, 1.0, 0.0 }, /* 黄 */
  { 1.0, 0.0, 1.0 }, /* マゼンタ */
  { 0.0, 1.0, 1.0 }  /* シアン 　*/
};

GLdouble normal[][3] = {
  { 0.0, 0.0,-1.0 },
  { 1.0, 0.0, 0.0 },
  { 0.0, 0.0, 1.0 },
  {-1.0, 0.0, 0.0 },
  { 0.0,-1.0, 0.0 },
  { 0.0, 1.0, 0.0 }
};

GLfloat light0pos[] = { 0.0, 3.0, 5.0, 1.0 };
GLfloat light1pos[] = { 5.0, 3.0, 0.0, 1.0 };

GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };

void cube(void)
{
  int i;
  int j;

  glBegin(GL_QUADS);
  for (j = 0; j < 6; ++j) {
    glNormal3dv(normal[j]);
    for (i = 0; i < 4; ++i) {
      glVertex3dv(vertex[face[j][i]]);
    }
  }
  glEnd();
}

void display(void)
{
  static int theta = 0;
  theta += 10;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 

  glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
  glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
  /* 図形の描画 */
  glColor3d(0.0, 0.0, 0.0);
  // glBegin(GL_LINES);
 /* 図形の色 (赤)  */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
  GLfloat white[] = {1, 1, 1, 1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30);

  glPushMatrix();
  glTranslated(0.0, 1.0, 0.0);
  glRotated(theta, 0.0, 1.0, 0.0);
  cube();
  glPopMatrix();
 
/* 二つ目の図形の描画 */
  glPushMatrix();
  glTranslated(0, 0, 0);
  glRotated(2*theta, 0.0, 1.0, 0.0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
  cube();
  glPopMatrix();

  if (theta >= 360) theta = 0;
  glFlush();
}

void resize(int w, int h)
{
  glViewport(0, 0, w, h);

  glLoadIdentity();
 gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
  glTranslated(0.0, 0.0, -5.0);
  gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  // glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
  /* 光源の位置設定 */

}

void init(void)
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, green);
  glLightfv(GL_LIGHT1, GL_SPECULAR, green);
}

void timer(int value) {
  glutPostRedisplay();  
  glutTimerFunc(1000, timer , 5);
}


int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutTimerFunc(5 , timer , 1);
  glutReshapeFunc(resize);
  init();
  glutMainLoop();
  return 0;
}