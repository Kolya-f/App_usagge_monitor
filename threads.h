#ifndef THREADS_H
#define THREADS_H

#include <QObject>
#include <QDateTime>
#include <QTimer>

class TimeTracker : public QObject {
    Q_OBJECT

public:
    TimeTracker(const QString& appName, QObject* parent = nullptr)
        : QObject(parent), appName(appName), timeSpent(0), running(false) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TimeTracker::updateTime);
    }

    int getTimeSpent() const { return timeSpent; }

public slots:
    void startTracking() {
        if (!running) {
            lastTimePoint = QDateTime::currentDateTime();
            running = true;
            timer->start(1000);  // Обновляем каждую секунду
        }
    }

    void stopTracking() {
        if (running) {
            timeSpent += lastTimePoint.secsTo(QDateTime::currentDateTime());
            running = false;
            timer->stop();
            lastTimePoint = QDateTime();  // Сбросим lastTimePoint
            emit finished();  // Сигнал о завершении работы
        }
    }

signals:
    void timeUpdated(int timeSpent);
    void finished();  // Добавьте этот сигнал для завершения работы

private slots:
    void updateTime() {
        if (running) {
            timeSpent += lastTimePoint.secsTo(QDateTime::currentDateTime());
            lastTimePoint = QDateTime::currentDateTime();
            emit timeUpdated(timeSpent);
        }
    }
public:
    QString getAppName() const {
        return appName;
    }


private:
    QString appName;
    QDateTime lastTimePoint;
    int timeSpent;
    bool running;
    QTimer *timer;  // Таймер для регулярного обновления времени
};

#endif // THREADS_H
