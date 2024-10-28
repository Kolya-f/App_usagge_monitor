#include "gradient.h"
#include <QPainter>
#include <QTimer>
#include <QLinearGradient>
#include <QPainterPath>
#include <QtMath>  // Для fmod

GradientWidget::GradientWidget(QWidget *parent)
    : QWidget(parent), hue(0) {
    // Установлюємо таймер для оновлення градієнта
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GradientWidget::updateGradient);
    timer->start(16);  // Оновлення кожні 16 мс для плавного переходу
}

void GradientWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect rect = this->rect();

    // Створюємо шлях з заокругленими кутами
    QPainterPath path;
    path.addRoundedRect(rect, 4, 4);  // Заокруглені кути, схожі на ті, що на зображенні
    painter.setClipPath(path);  // Встановлюємо обмеження малювання в межах заокругленого прямокутника

    // Створюємо лінійний градієнт по діагоналі (з верхнього лівого кута до нижнього правого)
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());

    gradient.setColorAt(0, QColor::fromHsv(hue, 255, 255));                     // Початковий колір
    gradient.setColorAt(1, QColor::fromHsv(fmod(hue + 240, 360), 255, 255));    // Кінцевий колір (повинен бути плавним)

    painter.fillRect(rect, gradient);  // Малюємо градієнт у прямокутнику
}

void GradientWidget::updateGradient() {
    hue = fmod(hue + 2.5, 360.0);  // Зменшуємо швидкість зміни на 2 для плавного збільшення кольору
    update();  // Перемальовуємо віджет
}
