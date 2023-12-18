
#ifndef FOURIERSERIESANIMATION_H
#define FOURIERSERIESANIMATION_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <opencv2/opencv.hpp>

#include <QMouseEvent>
#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QDebug>
#include <complex>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

class FourierSeriesAnimation : public QMainWindow {
    Q_OBJECT

public:
    struct ComplexResult {
        std::complex<double> value; // Complex value
        double freq;           // Frequency
        double amp;            // Amplitude
        double phase;          // Phase
    };
    const double PI = 3.14159265358979323846;
    explicit FourierSeriesAnimation(QWidget *parent = nullptr);


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    enum Mode { Drawing, Visualizing };
    Mode currentMode;

    QVector<QPoint> points; // Drawing points
    vector<ComplexResult> dftResult;// Fourier coefficients
    QVector<QPointF> path; // Container for the path
    double time;
    double dt;
    bool drawing;

    void convertAndCompute();
    std::vector<ComplexResult> computeDFT(const std::vector<std::complex<double>>& input);
    void updateAnimation();
};

#endif 
