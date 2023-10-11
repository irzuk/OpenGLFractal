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

constexpr std::array<GLfloat, 15u> vertices = {
	0.0f, 0.707f, 1.f, 0.f, 0.f,
	-0.5f, -0.5f, 0.f, 1.f, 0.f,
	0.5f, -0.5f, 0.f, 0.f, 1.f,
};
constexpr std::array<GLuint, 3u> indices = {0, 1, 2};



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
	vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

	// Create IBO
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

	// Bind attributes
	program_->bind();

	program_->enableAttributeArray(0);
	program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(5 * sizeof(GLfloat)));

	program_->enableAttributeArray(1);
	program_->setAttributeBuffer(1, GL_FLOAT, static_cast<int>(2 * sizeof(GLfloat)), 3,
								 static_cast<int>(5 * sizeof(GLfloat)));

	resolutionUniform_ = program_->uniformLocation("resolution");
	settingsUniform_ = program_->uniformLocation("settings");

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

	// const auto angle = 100.0 * static_cast<double>(frame_) / screen()->refreshRate();
	// matrix.rotate(static_cast<float>(angle), rotationAxis_);

	// Bind VAO and shader program
	program_->bind();
	vao_.bind();

	// Update uniform value
	// std::cout << mousePosition_.x() << ", " << mousePosition_.y() << std::endl;
    // std::cout << "zoom: " << zoom_ << std::endl;
    QVector3D v = QVector3D(mousePosition_, zoom_ * 0.5);
	// program_->use();
	program_->setUniformValue(resolutionUniform_, v);
	program_->setUniformValue(settingsUniform_, settings_);
	settings_.setX(settings_.x() - 1);

	// Draw
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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
	mousePressPosition_ = QVector2D(e->localPos());
	std::cout << "Mouse pressed" << "\n";
}

void TriangleWindow::mouseReleaseEvent(QMouseEvent * e)
{
	// const auto diff = QVector2D(e->localPos()) - mousePressPosition_;
	mousePosition_ = QVector2D(e->localPos());
	std::cout << "Mouse current pos, y: " << mousePosition_.y() << ", x: " << mousePosition_.x() << "\n";
	// rotationAxis_ = QVector3D(diff.y(), diff.x(), 0.0).normalized();
}

void TriangleWindow::wheelEvent(QWheelEvent *event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        //scrollWithPixels(numPixels);
		float y = numPixels.y();
		//std::cout << "pixels: x: " << numPixels.x() << ", y: " << y << std::endl;
		zoom_ += y / 10;
    } else if (!numDegrees.isNull()) {
        // QPoint numSteps = numDegrees / 15;
		std::cout << "degrees:  x: " << numDegrees.x() << ", y: " << numDegrees.y() << std::endl;
        // scrollWithDegrees(numSteps);
    }
    event->accept();
}
// mouseMoveEvent(QMouseEvent *ev)

double TriangleWindow::countFPS()
{
    double current_time = QTime::currentTime().second();
	double fps = (frame_ - last_counted_frame) / (current_time - last_time);
	// std::cout << "updated frame counter: " << frame_ << " , old_frame_counter: " << last_counted_frame << std::endl;
	last_counted_frame = frame_;
    last_time = current_time;
	return fps;
}

void TriangleWindow::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    // angularSpeed *= 0.99;

    // // Stop rotation when speed goes below threshold
    // if (angularSpeed < 0.01) {
    //     angularSpeed = 0.0;
    // } else {
    //     // Update rotation
    //     rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        render();
    //}
}