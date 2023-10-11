#pragma once

#include <Base/GLWindow.hpp>

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>
#include <QLabel>
#include <QBasicTimer>
#include <QSlider>

#include <memory>

class TriangleWindow final : public fgl::GLWindow
{

public:
	void init() override;
	void render() override;

protected:
	void mousePressEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent * e) override;
	void wheelEvent(QWheelEvent *ev) override;
	void timerEvent(QTimerEvent *) override;

private:
	GLint resolutionUniform_ = -1;
	GLint settingsUniform_ = -1;

	QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
	QOpenGLVertexArrayObject vao_;

	std::unique_ptr<QOpenGLShaderProgram> program_ = nullptr;

	size_t frame_ = 0;
	QVector2D mousePressPosition_{0., 0.};

    double zoom_ = 1.2;
	QVector2D mousePosition_{0., 0.};
	QVector3D settings_{100., 0., 0.};

	size_t last_counted_frame = 0;
	double last_time = 0;
	double countFPS();
	QBasicTimer timer;

	QSlider iterations;

	// settings
};
