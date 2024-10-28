#include "eastor_egg.h"
#include <QDebug>
#include <QPushButton>  // Якщо потрібно використовувати кнопки
#include <QLabel>
#include <QMessageBox>
#include<QScrollArea>

bool aboutDialogOpened = false;

// Реалізація InfoWindow
InfoWindow::InfoWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Історія створення та інше");
    setWindowIcon(QIcon("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Знімок екрана 2024-03-25 222406.png"));
    resize(800, 600);

    // Налаштування загального стилю для вікна та віджетів
    QString styleSheet = R"(
    /* Загальний стиль для вікна */
    QMainWindow {
        background-color: #F8F4E3; /* Світлий відтінок для фону вікна */
        border: none;
    }

    /* Вкладки */
    QTabWidget::pane {
        border: none; /* Без рамки */
    }

    QTabBar::tab {
        background-color: #EDE3CF; /* Світлий бежевий для вкладок */
        padding: 10px;
        color: #4A3F35; /* Темно-коричневий текст вкладок */
        border-radius: 5px;
    }

    QTabBar::tab:selected {
        background-color: #D1C7B7; /* Трохи темніший бежевий для вибраної вкладки */
        color: #FFFFFF; /* Білий текст для вибраної вкладки */
    }

    QTabBar::tab:hover {
        background-color: #D1C7B7; /* Трохи темніший бежевий при наведенні */
        color: #FFFFFF; /* Білий текст при наведенні */
    }

    /* Стиль для заголовків */
    QLabel#commentLabel1, QLabel#commentLabel2, QLabel#commentLabel3, QLabel#commentLabel4, QLabel#commentLabel5 {
        font-size: 16px; /* Розмір шрифту для заголовків */
        font-weight: bold;
        color: #4A3F35; /* Темно-коричневий колір заголовків */
    }

    /* Стиль для всіх текстових віджетів */
    QLabel {
        font-size: 14px; /* Розмір шрифту для звичайного тексту */
        color: #4A3F35; /* Темно-коричневий для звичайного тексту */
    }

    /* Кнопки */
    QPushButton {
        background-color: #EDE3CF; /* Світлий бежевий фон кнопок */
        border: none; /* Без рамки */
        border-radius: 5px;
        padding: 5px 10px;
        color: #4A3F35; /* Темно-коричневий текст */
    }

    QPushButton:hover {
        background-color: #D1C7B7; /* Трохи темніший бежевий при наведенні */
        color: #FFFFFF; /* Білий текст при наведенні */
    }

    QPushButton:pressed {
        background-color: #4A3F35; /* Темний коричневий при натисканні */
        color: #FFFFFF; /* Білий текст при натисканні */
    }
)";


    // Встановлюємо стилі для вікна та віджетів
    this->setStyleSheet(styleSheet);

    // Створюємо QTabWidget для вкладок
    QTabWidget *tabWidget = new QTabWidget(this);

    // --- Вкладка 1: Історія створення ---
    QWidget *historyTab = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyTab);

    // Створюємо головний віджет і компоновку для контенту
    QWidget *contentWidget = new QWidget(); // Контент, який буде в середині QScrollArea
    QVBoxLayout *layout = new QVBoxLayout(contentWidget); // Компоновка для контенту

    // Додаємо коментар до першого зображення
    QLabel *commentLabel1 = new QLabel("<p><b>Перший успішний запуск програми.</b></p>", this);
    layout->addWidget(commentLabel1);

    // Додаємо перше зображення
    QLabel *imageLabel1 = new QLabel(this);
    QPixmap pixmap1("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Знімок екрана 2024-09-08 200002.png");
    if (!pixmap1.isNull()) {
        pixmap1 = pixmap1.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel1->setPixmap(pixmap1);
    } else {
        imageLabel1->setText("Зображення не знайдено");
    }
    layout->addWidget(imageLabel1);

    // Додаємо коментар до другого зображення
    QLabel *commentLabel2 = new QLabel("<p><b>Автоматизація збору статистики: Наступним кроком було налаштування автоматичного збору статистики про використання програм.</b></p>"
                                       "<p><b>Для цього було вирішено використовувати системні таймери та фонові процеси.</b></p>", this);
    layout->addWidget(commentLabel2);

    // Додаємо друге зображення
    QLabel *imageLabel2 = new QLabel(this);
    QPixmap pixmap2("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Знімок екрана 2024-09-08 203719.png");
    if (!pixmap2.isNull()) {
        pixmap2 = pixmap2.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel2->setPixmap(pixmap2);
    } else {
        imageLabel2->setText("Зображення не знайдено");
    }
    layout->addWidget(imageLabel2);

    // Додаємо коментар до третього зображення
    QLabel *commentLabel3 = new QLabel("<p><b>Наступний етап розвитку:</b></p>"
                                       "<p><b>Після успішного запуску та налаштування базового функціоналу з таймером було вирішено розширити можливості програми для зберігання та експорту даних у різні формати.</b></p>", this);
    layout->addWidget(commentLabel3);

    // Додаємо третє зображення
    QLabel *imageLabel3 = new QLabel(this);
    QPixmap pixmap3("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Знімок екрана 2024-09-09 083745.png");
    if (!pixmap3.isNull()) {
        pixmap3 = pixmap3.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel3->setPixmap(pixmap3);
    } else {
        imageLabel3->setText("Зображення не знайдено");
    }
    layout->addWidget(imageLabel3);

    // Додаємо коментар до четвертого зображення
    QLabel *commentLabel4 = new QLabel("<p><b>Наступний етап розвитку:</b></p>"
                                       "<p><b>Оптимізація продуктивності та створення декількох потоків для точнішого вимірювання проведеного часу у застосунках.</b></p>", this);
    layout->addWidget(commentLabel4);

    // Додаємо четверте зображення
    QLabel *imageLabel4 = new QLabel(this);
    QPixmap pixmap4("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Знімок екрана 2024-09-09 083745.png");
    if (!pixmap4.isNull()) {
        pixmap4 = pixmap4.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel4->setPixmap(pixmap4);
    } else {
        imageLabel4->setText("Зображення не знайдено");
    }
    layout->addWidget(imageLabel4);

    // Додаємо коментар до п'ятого зображення
    QLabel *commentLabel5 = new QLabel("<p><b>Наступний етап розвитку:</b></p>"
                                       "<p><b>Додання градієнта для виділення програми, яка відкрита.</b></p>", this);
    layout->addWidget(commentLabel5);

    // Додаємо п'яте зображення
    QLabel *imageLabel5 = new QLabel(this);
    QPixmap pixmap5("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Знімок екрана 2024-09-09 083745.png");
    if (!pixmap5.isNull()) {
        pixmap5 = pixmap5.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel5->setPixmap(pixmap5);
    } else {
        imageLabel5->setText("Зображення не знайдено");
    }
    layout->addWidget(imageLabel5);

    // Створюємо QScrollArea для прокрутки контенту
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true); // Робимо область прокручування динамічною
    scrollArea->setWidget(contentWidget); // Встановлюємо контент всередині QScrollArea

    // Додаємо область прокручування до вкладки
    historyLayout->addWidget(scrollArea);

    // --- Вкладка 2: Інший розділ ---
    QWidget *otherTab = new QWidget();
    QVBoxLayout *otherLayout = new QVBoxLayout(otherTab);

    // Додаємо контент для другої вкладки
    QLabel *otherContent = new QLabel(
        "<p><b>Після створення такої програми, будуть опановані нові технології.</b></p>"
        "<p>У цій програмі використовуються такі бібліотеки та технології:</p>"
        "<ul>"
        "<li><b>QApplication:</b> Основний клас для керування життєвим циклом Qt-додатку.</li>"
        "<li><b>QMainWindow:</b> Базовий клас для створення головного вікна з меню, панелями інструментів та іншими елементами інтерфейсу.</li>"
        "<li><b>QMenuBar, QMenu, QAction:</b> Створюють меню та елементи дій у вікні програми.</li>"
        "<li><b>QElapsedTimer:</b> Використовується для вимірювання часу, який минув.</li>"
        "<li><b>QTableWidget:</b> Виджет для відображення таблиць з даними.</li>"
        "<li><b>QVBoxLayout:</b> Компоновка для вертикального розташування віджетів.</li>"
        "<li><b>QDateTime:</b> Клас для роботи з датою та часом.</li>"
        "<li><b>QMap:</b> Контейнер для збереження пар ключ-значення.</li>"
        "<li><b>QHeaderView:</b> Управління заголовками колонок у таблицях.</li>"
        "<li><b>QPainter:</b> Клас для малювання графіки.</li>"
        "<li><b>QProxyStyle:</b> Використовується для налаштування стилю інтерфейсу користувача.</li>"
        "<li><b>QLabel:</b> Виджет для відображення тексту або зображень.</li>"
        "<li><b>QFile, QTextStream:</b> Класи для роботи з файлами та потоками тексту.</li>"
        "<li><b>QProcess:</b> Використовується для запуску зовнішніх процесів.</li>"
        "<li><b>QPushButton:</b> Виджет кнопки для виконання дій за кліком.</li>"
        "<li><b>QFileInfo:</b> Інформація про файли та їхні властивості.</li>"
        "<li><b>QMessageBox:</b> Віджет для відображення повідомлень і діалогових вікон.</li>"
        "<li><b>QCloseEvent:</b> Обробляє події закриття вікна.</li>"
        "<li><b>QClipboard:</b> Доступ до буфера обміну.</li>"
        "<li><b>QFileDialog:</b> Діалогове вікно для вибору файлів або каталогів.</li>"
        "<li><b>QInputDialog:</b> Діалогове вікно для введення користувацьких даних.</li>"
        "<li><b>QPropertyAnimation:</b> Анімація для зміни властивостей об'єктів.</li>"
        "<li><b>QPalette:</b> Клас для налаштування кольорів інтерфейсу користувача.</li>"
        "<li><b>QTableWidgetItem:</b> Елементи для таблиці.</li>"
        "<li><b>QColor:</b> Клас для роботи з кольорами.</li>"
        "<li><b>QMediaPlayer:</b> Клас для відтворення мультимедійних файлів.</li>"
        "<li><b>QThread:</b> Клас для створення багатопоточних програм.</li>"
        "<li><b>QSettings:</b> Клас для збереження та завантаження налаштувань програми.</li>"
        "<li><b>GradientWidget:</b> Клас для створення віджету з градієнтами.</li>"
        "<li><b>QXlsx:</b> Бібліотека для роботи з Excel-документами.</li>"
        "<li><b>eastor_egg:</b> Додатковий модуль для прихованих можливостей програми.</li>"
        "</ul>", this);

    otherLayout->addWidget(otherContent);

    // Додаємо вкладки до QTabWidget
    tabWidget->addTab(historyTab, "Історія створення");
    tabWidget->addTab(otherTab, "Інший розділ");

    // Встановлюємо QTabWidget як центральний віджет
    setCentralWidget(tabWidget);
}


// Реалізація MyWindow
MyWindow::MyWindow(QWidget *parent)
    : QMainWindow(parent), fKeyPressed(false), iKeyPressed(false), pKeyPressed(false) {

    setWindowTitle("Головне вікно");
    resize(600, 400);
    setFocusPolicy(Qt::StrongFocus);

    installEventFilter(this);
    connect(this, &MyWindow::aboutDialogRequested, this, &MyWindow::ShowAboutDialog);
}


// Функція для показу діалогу "Про програму" (тільки одна)
void MyWindow::ShowAboutDialog() {
    if (!aboutDialogOpened) {  // Використовуємо глобальну змінну
        QDialog *aboutDialog = new QDialog(this);
        aboutDialog->setWindowTitle("Про програму");
        aboutDialog->resize(400, 400);

        // Встановлюємо іконку для вікна "Про програму"
        aboutDialog->setWindowIcon(QIcon("Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Постійність_пам'яті.jpg"));

        QLabel *imageLabel = new QLabel(aboutDialog);
        QPixmap pixmap("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Знімок екрана 2024-03-25 222406.png");
        if (!pixmap.isNull()) {
            pixmap = pixmap.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            imageLabel->setPixmap(pixmap);
            imageLabel->setAlignment(Qt::AlignCenter);
        } else {
            imageLabel->setText("Зображення не знайдено");
        }

        QLabel *textLabel = new QLabel(R"(
        <p><b>Це програма для моніторингу використання додатків, створена Fipokzec з команди "Fipokzec and PSGP developer team" .</b></p>
        <p><b>Версія:</b> 1.0.0</p>
        <h4>Як встановити шлях до Excel або LibreOffice:</h4>
        <ol>
            <li>Відкрийте провідник Windows (Файловий менеджер).</li>
            <li>Знайдіть файл <b>scalc.exe</b> для LibreOffice або <b>excel.exe</b> для Microsoft Excel.</li>
            <li>Натисніть правою кнопкою миші на файл і виберіть "Розташування файлу".</li>
            <li>Натисніть правою кнопкою миші на файл і виберіть "Властивості".</li>
            <li>У вікні властивостей скопіюйте шлях до файлу.</li>
            <li>Дефолтний шлях для LibreOffice: <code>C:\Program Files (x86)\LibreOffice\program\scalc.exe</code>.</li>
        </ol>
    )", aboutDialog);

        textLabel->setWordWrap(true);

        QVBoxLayout *layout = new QVBoxLayout(aboutDialog);
        layout->addWidget(imageLabel);
        layout->addWidget(textLabel);

        aboutDialog->setLayout(layout);
        aboutDialogOpened = true;  // Використовуємо глобальну змінну
        aboutDialog->show();

        connect(aboutDialog, &QDialog::finished, this, []() {
            aboutDialogOpened = false;  // Скидаємо глобальну змінну
            qDebug() << "Вікно 'Про програму' закрите";
        });
    } else {
        qDebug() << "Вікно 'Про програму' вже відкрите! ff";
       // aboutDialogOpened = false;
    }
}
// Функція для відкриття нового вікна
// Реалізація MyWindow
void MyWindow::OpenNewWindow() {
    qDebug() << "Комбінація клавіш натиснута, відкриваємо нове вікно";
    InfoWindow *infoWindow = new InfoWindow(this);  // Створюємо нове вікно
    infoWindow->setAttribute(Qt::WA_DeleteOnClose);  // Вказуємо, що вікно повинно видалятись при закритті
    infoWindow->show();  // Показуємо його
}


void MyWindow::keyPressEvent(QKeyEvent *event) {
    int keyCode = event->key();
    char keyChar = static_cast<char>(keyCode);

    qDebug() << "Натиснуто клавішу (код): " << keyCode;
    qDebug() << "Символ клавіші: " << keyChar;

    // Перевіряємо натискання клавіші 'F' або кириличної 'А'
    if (keyCode == Qt::Key_F || keyCode == 70 || keyCode == 1040) {
        fKeyPressed = true;
        qDebug() << "Натиснуто F або кирилична А";
    }

    // Перевіряємо натискання клавіші 'I' або кириличної 'Ш'
    if (keyCode == Qt::Key_I || keyCode == 73 || keyCode == 1064) {
        iKeyPressed = true;
        qDebug() << "Натиснуто I або кирилична Ш";
    }

    // Перевіряємо натискання клавіші 'P' або кириличної 'З'
    if (keyCode == Qt::Key_P || keyCode == 80 || keyCode == 1047) {
        pKeyPressed = true;
        qDebug() << "Натиснуто P або кирилична З";
    }

    // Якщо натиснуто правильну комбінацію клавіш
    if (fKeyPressed && iKeyPressed && pKeyPressed) {
        qDebug() << "Комбінація клавіш натиснута, відкриваємо вікно 'Про програму'";
        emit aboutDialogRequested();  // Викликаємо сигнал
        fKeyPressed = false;
        iKeyPressed = false;
        pKeyPressed = false;
    }
}

// Фільтр подій для обробки клавіш (оновлена логіка)
bool MyWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        int keyCode = keyEvent->key();
        char keyChar = static_cast<char>(keyCode);

        qDebug() << "Натиснуто клавішу (код): " << keyCode;
        qDebug() << "Символ клавіші: " << keyChar;

        // Перевіряємо натискання клавіші F, I або P в будь-якій розкладці
        if (keyCode == Qt::Key_F || keyCode == 70 || keyCode == 1040) fKeyPressed = true;
        if (keyCode == Qt::Key_I || keyCode == 73 || keyCode == 1064) iKeyPressed = true;
        if (keyCode == Qt::Key_P || keyCode == 80 || keyCode == 1047) pKeyPressed = true;

        // Якщо натиснуто комбінацію F, I та P або їх кириличних відповідників
        if (fKeyPressed && iKeyPressed && pKeyPressed) {
            qDebug() << "Комбінація клавіш натиснута!";

            // Перевіряємо, чи відкрите вікно "Про програму"
            qDebug() << "xtr до  "<<aboutDialogOpened;
            if (aboutDialogOpened) {
                OpenNewWindow();  // Відкриття нового вікна, якщо вікно "Про програму" відкрите
            } else {
                qDebug() << "уже "<<aboutDialogOpened;

            }

            // Скидаємо стан клавіш
            fKeyPressed = false;
            iKeyPressed = false;
            pKeyPressed = false;
        }
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}



// Обробка відпускання клавіш
void MyWindow::keyReleaseEvent(QKeyEvent *event) {
    int keyCode = event->key();

    // Скидаємо стан після відпускання клавіші
    if (keyCode == Qt::Key_F || keyCode == 70) fKeyPressed = false;
    if (keyCode == Qt::Key_I || keyCode == 73) iKeyPressed = false;
    if (keyCode == Qt::Key_P || keyCode == 80) pKeyPressed = false;
}
