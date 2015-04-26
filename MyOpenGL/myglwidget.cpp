// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>

#include "myglwidget.h"

#include <GL/glu.h>

MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
}

MyGLWidget::~MyGLWidget()
{
}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MyGLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    angle = angle % (360 * 16);
    if (angle < 0)
    {
        angle = angle + (360 * 16);
    }
}

void MyGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    static GLfloat lightPosition[4] = { 10, 20, 30, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    loadTexture();

    GLUquadricObj *sphere=NULL;
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    //Making a display list
    mysphereID = glGenLists(1);
    glNewList(mysphereID, GL_COMPILE);
    gluSphere(sphere, 1.5, 360, 180);
    glEndList();
    gluDeleteQuadric(sphere);
    //glDisable(GL_TEXTURE_2D);
}

int MyGLWidget::loadTexture()
{
    unsigned char header[54];
    unsigned int dataPos, width, height, imageSize;
    unsigned char *pData;

    FILE *file = fopen("D:\\backups\\Dropbox\\Projects\\MyOpenGL\\earth.bmp", "rb");
    if (!file)
    {
        printf("Open bmp failed\n");
        return -1;
    }

    if (fread(header, 1, 54, file) != 54)
    {
        printf("Read bmp header failed\n");
        return -2;
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
        printf("Invalid bmp header\n");
        return -3;
    }

    dataPos   = *(int*)(header+0x0A);
    imageSize = *(int*)(header+0x22);
    width     = *(int*)(header+0x12);
    height    = *(int*)(header+0x16);

    if (imageSize == 0)
    {
        imageSize = width * height * 3;
    }
    if (dataPos == 0)
    {
        dataPos = 54;
    }

    pData = new unsigned char[imageSize];
    fread(pData, 1, imageSize, file);
    fclose(file);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, (const GLvoid*)pData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}

int MyGLWidget::updateTexture(int width, int height, unsigned char *pData)
{
    //glBindTexture(GL_TEXTURE_2D, textureID);
    return width + height + *pData;
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
    draw();
}

void MyGLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }

    lastPos = event->pos();
}

void MyGLWidget::draw()
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glCallList(mysphereID);
}
