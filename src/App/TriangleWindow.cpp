#include "TriangleWindow.h"

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QScreen>
#include <QString>
#include <QTime>

#include <array>
#include <iostream>


namespace
{

constexpr std::array<GLfloat, 8u> vertices = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f,
    -1.0f,  1.0f
};
constexpr std::array<GLuint, 6u> indices = { 0, 1, 2, 0, 2, 3 };



}// namespace

void TriangleWindow::init()
{
	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment,
									  ":/Shaders/diffuse.fs");
	program_->link();

	// Create VAO object
	vao_.create();
	vao_.bind();

	// Create VBO
	vbo_.create();
	vbo_.bind();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(vertices[0])));

	// Create IBO
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(indices[0])));

	// Bind attributes
	program_->bind();

	program_->enableAttributeArray(0);
	program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

	resolutionUniform_ = program_->uniformLocation("resolution");
	settingsUniform_ = program_->uniformLocation("settings");
	colorUniform_ = program_->uniformLocation("color");

	// Release all
	program_->release();

	vao_.release();

	ibo_.release();
	vbo_.release();

	// Uncomment to enable depth test and face culling
	// glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    timer.start(12, this);
	last_time = QTime::currentTime().second();

    // setMouseGrabEnabled?
	// iterations
}

void TriangleWindow::render()
{
	// Configure viewport
	const auto retinaScale = devicePixelRatio();
	glViewport(0, 0, static_cast<GLint>(width() * retinaScale),
			   static_cast<GLint>(height() * retinaScale));

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind VAO and shader program
	program_->bind();
	vao_.bind();

	// Update uniform value
    QVector3D v = QVector3D(diff.x(), diff.y(), zoom_);
	program_->setUniformValue(resolutionUniform_, v);
	settings_.setX(background->red() / (float)255);
	settings_.setY(background->green() / (float)255);
	settings_.setZ(background->blue() / (float)255);
	settings_.setW(iterations->value());
	program_->setUniformValue(settingsUniform_, settings_);
	program_->setUniformValue(colorUniform_, QVector3D(frac->red(), frac->green(), frac->blue()) / (float)255);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// Release VAO and shader program
	vao_.release();
	program_->release();

	// Increment frame counter
	++frame_;

	// update vizualization
	if(QTime::currentTime().second() - last_time > 0) {
		this->setTitle(QString("FPS: ").append(QString::number(countFPS())));
	}
}

void TriangleWindow::mousePressEvent(QMouseEvent * e)
{
	mousePrevPosition_ = QVector2D(e->windowPos());
	isMoving_ = true;
}

void TriangleWindow::mouseReleaseEvent(QMouseEvent * e)
{
    (void)e;
	isMoving_ = false;
}

void TriangleWindow::mouseMoveEvent(QMouseEvent * e)
{
	if (isMoving_) {
		mousePosition_ = QVector2D(e->windowPos());
		auto localDiff = mousePosition_ - mousePrevPosition_;
		diff += QVector2D(-localDiff.x() / (this->width() / 2), localDiff.y() / (this->height() / 2));
		mousePrevPosition_ = mousePosition_;
	}
}

void TriangleWindow::wheelEvent(QWheelEvent *event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
		float y = numPixels.y();
		zoom_ += y / 1000;
		// std::cout << zoom_ << std::endl;
		auto pos = QVector2D(event->pos()) - QVector2D(this->width() / 2, this->height() / 2);
		auto realPos = QVector2D(pos.x() / (this->width() / 2), pos.y() / (this->height() / 2));
		const float c = 2.2;
		auto d = QVector2D ((-realPos.x()) / c * (y / 1000) + (1/2 * (y / 1000)), (realPos.y()) / c * (y / 1000) - (1/2 * (y / 1000)));
		// std::cout << "x: " << d.x() << ", localDiff: " << d.y() << std::endl;
		diff += d;
    } else if (!numDegrees.isNull()) {
		std::cout << "degrees:  x: " << numDegrees.x() << ", y: " << numDegrees.y() << std::endl;
		// throw std::exception("wheel event by degrees is not implemented");
    }
    event->accept();
}
// mouseMoveEvent(QMouseEvent *ev)

double TriangleWindow::countFPS()
{
    double current_time = QTime::currentTime().second();
	double fps = (frame_ - last_counted_frame) / (current_time - last_time);
	last_counted_frame = frame_;
    last_time = current_time;
	return fps;
}

void TriangleWindow::timerEvent(QTimerEvent *)
{
    renderNow();
}