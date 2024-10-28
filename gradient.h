#ifndef GRADIENT_H
#define GRADIENT_H

#include <QWidget>
#include <QTimer>
#include <QColor>

class GradientWidget : public QWidget {
    Q_OBJECT

public:
    explicit GradientWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateGradient();

private:
    int hue;
};

#endif // GRADIENT_H
