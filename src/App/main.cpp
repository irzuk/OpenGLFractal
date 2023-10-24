#include <QApplication>
#include <QSurfaceFormat>
#include <QWidget>
#include <QHBoxLayout>
#include <QDialog>
#include <QColorDialog>
#include <QPushButton>

#include "TriangleWindow.h"
#include <iostream>

namespace
{
constexpr auto g_sampels = 16;
constexpr auto g_gl_major_version = 3;
constexpr auto g_gl_minor_version = 3;


}// namespace

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setSamples(g_sampels);
	format.setVersion(g_gl_major_version, g_gl_minor_version);
	format.setProfile(QSurfaceFormat::CoreProfile);

	TriangleWindow window;
	window.setFormat(format);
	window.resize(640, 640);
	
    auto m = new QDialog();
	m->resize(150,70);
	auto sliderIteration = new QSlider(Qt::Horizontal);
    sliderIteration->setRange(0, 500);
	sliderIteration->setValue(200);
	QObject::connect(sliderIteration, SIGNAL(valueChanged(int)), sliderIteration, SLOT(setValue(int)));
	QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(sliderIteration);
    m->setLayout(layout);
    m->setUpdatesEnabled(true);

	QColor frac(255, 0, 0);
    QColor background(0, 0, 0);

	QPushButton backgr(QString("background color"), m);
	backgr.setGeometry(0, 0, 100, 30);
	QPushButton fra(QString("fractal color"), m);
	fra.setGeometry(0, 40, 100, 30);

	QObject::connect(&backgr, &QPushButton::released, m, 
	      [&m, &background] {
    	auto color_ = QColorDialog::getColor(Qt::red, m, QString("Choose color"));
		background.setRed(color_.red());
		background.setGreen(color_.green());
		background.setBlue(color_.blue());
});
	QObject::connect(&fra, &QPushButton::released, m,
	      [&m, &frac] {
    	auto color_ = QColorDialog::getColor(Qt::red, m, QString("Choose color"));
		frac.setRed(color_.red());
		frac.setGreen(color_.green());
		frac.setBlue(color_.blue());
});
	
	window.setSliders(sliderIteration);
	window.setColors(&frac, &background);
	window.show();
	window.setAnimated(true);
	m->setVisible(true);
	return app.exec();
}