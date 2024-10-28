#ifndef EASTOR_EGG_H
#define EASTOR_EGG_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QMessageBox>
#include <QLabel>
#include<QVBoxLayout>
//bool aboutDialogOpened =false;     // Прапор, чи відкрите вікно "Про програму"

// Вікно, яке буде відкриватися
class InfoWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit InfoWindow(QWidget *parent = nullptr);  // Конструктор
};

// Головний клас вікна
class MyWindow : public QMainWindow {
    Q_OBJECT

private:
    bool fKeyPressed;           // Прапор для клавіші F
    bool iKeyPressed;           // Прапор для клавіші I
    bool pKeyPressed;           // Прапор для клавіші P
    //bool aboutDialogOpened =false;     // Прапор, чи відкрите вікно "Про програму"
    QMessageBox *aboutDialog;  // Діалогове вікно "Про програму"

public:
    explicit MyWindow(QWidget *parent = nullptr);  // Конструктор

protected:
    void keyPressEvent(QKeyEvent *event) override;    // Обробка натискання клавіш
    void keyReleaseEvent(QKeyEvent *event) override;  // Обробка відпускання клавіш
    bool eventFilter(QObject *obj, QEvent *event) override;  // Фільтр подій

public slots:
    void ShowAboutDialog();  // Функція для показу діалогу "Про програму"
    void OpenNewWindow();    // Функція для відкриття нового вікна\

signals:
    void aboutDialogRequested();  // Сигнал для відкриття вікна "Про програму"

};

#endif // EASTOR_EGG_H
