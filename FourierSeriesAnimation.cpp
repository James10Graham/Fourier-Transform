#include "FourierSeriesAnimation.h"

FourierSeriesAnimation::FourierSeriesAnimation(QWidget *parent)
    : QMainWindow(parent), currentMode(Drawing), time(0.0), drawing(false){

    resize(1200, 800);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FourierSeriesAnimation::updateAnimation); 
    timer->start(33); // Around 30fps

}


void FourierSeriesAnimation::mousePressEvent(QMouseEvent *event) {
    if (currentMode == Visualizing) {
        // Clear previous data
        points.clear();
        dftResult.clear();
        path.clear();
        currentMode = Drawing;
    }

    if (event->button() == Qt::LeftButton && event->pos().x()) {
        points.clear();
        QPoint centeredPos = event->pos() - QPoint(width() / 2, height() / 2); // Convert the position to be centered relative to the window
        points.append(centeredPos);
        drawing = true;
    }
}

void FourierSeriesAnimation::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && drawing && event->pos().x()) {
        QPoint centeredPos = event->pos() - QPoint(width() / 2, height() / 2); // Convert the position to be centered relative to the window
        points.append(centeredPos);
        update();
    }
}


void FourierSeriesAnimation::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && drawing) {
        drawing = false;
        points.append(points.first()); // Close the loop
        update();

        convertAndCompute(); // Convert points to complex numbers and compute DFT
        currentMode = Visualizing;
    }
}

void FourierSeriesAnimation::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.translate(width() / 2, height() / 2); // Center the drawing area

    if (!points.isEmpty()) {
        painter.setPen(QPen(QColor("#FFFFFF"), 1));
        painter.drawPolyline(points.data(), points.size());
    }
    
    if (currentMode == Visualizing) {
        int N = dftResult.size();
        QPointF center(0, 0);
        QPointF prevCenter = center;

        // Draw each epicycle
        for (int i = 0; i < N; ++i) {
            const auto& coef = dftResult[i];
 
            // Compute the position of the current epicycle
            int freq = coef.freq;
            double amp = coef.amp;
            double phase = coef.phase;

            double x = prevCenter.x() + amp * cos(freq * time + phase);
            double y = prevCenter.y() + amp * sin(freq * time + phase);

            // Draw epicycle
            
            painter.setPen(QPen(QColor("#707070"), 1));
            painter.drawEllipse(QPointF(prevCenter.x(), prevCenter.y()), amp, amp);
            painter.setPen(QPen(QColor("#bbb"), 1));
            painter.drawLine(prevCenter, QPointF(x, y));

            prevCenter = QPointF(x, y);
        }

        // Draw the path of the Fourier Series
        path.append(prevCenter);
        painter.setPen(QPen(Qt::red, 2));
        painter.drawPolyline(path.data(), path.size());
    }
}

void FourierSeriesAnimation::convertAndCompute() {
    // Convert points to complex numbers and compute DFT
    std::vector<std::complex<double>> complexPoints;
    for (const QPoint& point : points) {
        complexPoints.emplace_back(point.x(), point.y());
    }

    dftResult = computeDFT(complexPoints);
    
    // Sort the DFT results by amplitude (largest epicircle first)
    std::sort(dftResult.begin(), dftResult.end(), [](const FourierSeriesAnimation::ComplexResult& a, const FourierSeriesAnimation::ComplexResult& b) {
        return a.amp > b.amp;
    });

    time = 0;
    if (!dftResult.empty()) {
        dt = 2 * PI / dftResult.size();
    }
}

std::vector<FourierSeriesAnimation::ComplexResult> FourierSeriesAnimation::computeDFT(const std::vector<std::complex<double>>& input) {
    int N = input.size();
    std::vector<FourierSeriesAnimation::ComplexResult> output(N);

    for (int k = 0; k < N; ++k) {
        std::complex<double> sum(0.0, 0.0);

        for (int n = 0; n < N; ++n) {
            double angle = 2 * PI * k * n / N;
            std::complex<double> w(cos(angle), -sin(angle)); // e^{-i2πkn/N}
            sum += input[n] * w;
        }

        sum /= static_cast<double>(N);

        output[k].value = sum;
        output[k].freq = k;
        output[k].amp = std::abs(sum); // Absolute value of the complex number i.e sqrt(re*re + im*im) =
        output[k].phase = std::arg(sum); // Argument of the complex number i.e atan2(re, im)
    }

    return output;
}

void FourierSeriesAnimation::updateAnimation() {
    if (currentMode == Visualizing) {
        time += dt * 5; // dt * s, where s is any sclar value for the speed increase
        update();
    }
}


