#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QElapsedTimer>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QDateTime>
#include <QMap>
#include <QHeaderView>
#include <QPainter>
#include <QProxyStyle>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QPushButton>
#include <QFileInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QClipboard>
#include <QFileDialog>
#include <QInputDialog>
#include <QPropertyAnimation>
#include <QPalette>
#include <QTableWidgetItem>
#include <QColor>
#include <QMediaPlayer>
#include <QThread>
#include<QSettings>

#include"threads.h"
#include "gradient.h" // Подключаем заголовочный файл для GradientWidget
#include <windows.h>
#include "xlsxdocument.h"  // Подключаем QXlsx для Excel
#include "eastor_egg.h"

QString tempExcelPath;
QString tempLibrePath;

class AppUsageMonitor : public QMainWindow {
    Q_OBJECT

public:
    AppUsageMonitor(QWidget* parent = nullptr)
        : QMainWindow(parent), last_active_app(""), last_time_point(QDateTime::currentDateTime()), currentFileName("AppUsageData.xlsx"),isFirstRun(true)
    {
        setWindowTitle("App Usage Monitor");
        setWindowIcon(QIcon("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Постійність_пам'яті.jpg"));
        resize(800, 600);

        applyLightMode();

        // Создание главного меню
        QMenuBar* menuBar = new QMenuBar(this);
        setMenuBar(menuBar);

        // Файл
        QMenu* fileMenu = new QMenu("Файл", this);
        menuBar->addMenu(fileMenu);
        QAction* openExel = new QAction("Відкрити у Excel", this);
        QAction* openLibreAction = new QAction("Відкрити в LibreOffice Calc", this);
        QAction* setExcelPathAction = new QAction("Встановити шлях до Excel", this);
        QAction* setLibrePathAction = new QAction("Встановити шлях до LibreOffice Calc", this);
        fileMenu->addAction(openExel);
        fileMenu->addAction(openLibreAction);
        fileMenu->addSeparator();
        fileMenu->addAction(setExcelPathAction);
        fileMenu->addAction(setLibrePathAction);

        // Правка
        QMenu* editMenu = new QMenu("Правка", this);
        menuBar->addMenu(editMenu);
        QAction* sortAscAction = new QAction("Сортувати за зростанням", this);
        QAction* sortDescAction = new QAction("Сортувати за спаданням", this);
        QAction* sortAlphaAscAction = new QAction("Сортувати за алфавітом", this);
        QAction* sortAlphaDescAction = new QAction("Сортувати проти алфавіта", this);

        editMenu->addAction(sortAlphaAscAction);
        editMenu->addAction(sortAlphaDescAction);
        editMenu->addAction(sortAscAction);
        editMenu->addAction(sortDescAction);

        // Вид
        QMenu* viewMenu = new QMenu("Вид", this);
        menuBar->addMenu(viewMenu);
        QAction* toggleNightModeAction = new QAction("Нічний режим", this);
        QAction* toggleLightModeAction = new QAction("Світлий режим", this);
        QAction* minimizeAction = new QAction("Свернуть", this);
        QAction* maximizeAction = new QAction("Розвернуть", this);

        viewMenu->addAction(toggleNightModeAction);
        viewMenu->addAction(toggleLightModeAction);
        viewMenu->addAction(minimizeAction);
        viewMenu->addAction(maximizeAction);

        // Справка
        QMenu* helpMenu = new QMenu("Допомога", this);
        menuBar->addMenu(helpMenu);
        QAction* aboutAction = new QAction("Про програму", this);
        helpMenu->addAction(aboutAction);

        // Підключення слоту ShowAboutDialog
      connect(aboutAction, &QAction::triggered, this, &AppUsageMonitor::TextAboutDialog2);



        // Підключення активації перевірки клавіатури
        // connect(aboutAction, &QAction::triggered, this, &AppUsageMonitor::OpenNewWindow);

        connect(minimizeAction, &QAction::triggered, this, &AppUsageMonitor::MinimizeWindow);
        connect(maximizeAction, &QAction::triggered, this, &AppUsageMonitor::MaximizeWindow);
        connect(toggleNightModeAction, &QAction::triggered, this, &AppUsageMonitor::applyNightMode);
        connect(toggleLightModeAction, &QAction::triggered, this, &AppUsageMonitor::applyLightMode);

        connect(sortAscAction, &QAction::triggered, this, &AppUsageMonitor::SortByTimeAscending);
        connect(sortDescAction, &QAction::triggered, this, &AppUsageMonitor::SortByTimeDescending);
        connect(sortAlphaAscAction, &QAction::triggered, this, &AppUsageMonitor::SortByNameAscending);
        connect(sortAlphaDescAction, &QAction::triggered, this, &AppUsageMonitor::SortByNameDescending);

        connect(openExel, &QAction::triggered, this, &AppUsageMonitor::OpenExcel);
        connect(openLibreAction, &QAction::triggered, this, &AppUsageMonitor::OpenLibreOfficeCalc);
        connect(setExcelPathAction, &QAction::triggered, this, &AppUsageMonitor::SetExcelPath);
        connect(setLibrePathAction, &QAction::triggered, this, &AppUsageMonitor::SetLibrePath);




        QWidget* central_widget = new QWidget(this);
        main_layout = new QVBoxLayout(central_widget);

        appsTable = new QTableWidget(0, 4, this);
        appsTable->setHorizontalHeaderLabels({ "Application", "Time Spent", "Current opened", "Remove" });
        ConfigureTable_UI(appsTable);
        main_layout->addWidget(new QLabel("Applications", this));
        main_layout->addWidget(appsTable);


        // Новый виджет и макет для веб-приложений
        QWidget* webAppsWidget = new QWidget(this);
        QVBoxLayout* webAppsLayout = new QVBoxLayout(webAppsWidget);

        webAppsTable = new QTableWidget(0, 4, this);
        webAppsTable->setHorizontalHeaderLabels({ "Web Application", "Time Spent", "Current opened", "Remove" });
        ConfigureTable_UI(webAppsTable);
        webAppsLayout->addWidget(new QLabel("Web Applications", this));
        webAppsLayout->addWidget(webAppsTable);


        main_layout->addWidget(webAppsWidget);

        setCentralWidget(central_widget);

        elapsedTimer.start();

        // Таймер для регулярного оновлення
        QTimer* updateTimer = new QTimer(this);
        connect(updateTimer, &QTimer::timeout, this, &AppUsageMonitor::UpdateBackend);
        updateTimer->start(2000);  // Оновлення кожні 2 секунд

        UpdateBackend();
    }

    ~AppUsageMonitor() {
        for (auto& tracker : timeTrackers) {
            StopTrackingApp(tracker->getAppName());
        }
    }

private slots:

    void UpdateBackend() {
        HWND hwnd = GetForegroundWindow();
        if (hwnd) {
            char window_title[256];
            GetWindowTextA(hwnd, window_title, sizeof(window_title));
            QString cur_win = QString::fromLocal8Bit(window_title);

            // Отримуємо поточний час
            QDateTime currentTime = QDateTime::currentDateTime();

            // Обчислюємо кількість секунд, що минули з останнього оновлення
            int secondsSpent = last_time_point.secsTo(currentTime);

            if (!last_active_app.isEmpty() && secondsSpent > 0) {
                // Додаємо час до активного додатку
                if (isWebApplication(last_active_app)) {
                    web_data_map[last_active_app] += secondsSpent;
                } else {
                    data_map[last_active_app] += secondsSpent;
                }
            }

            // Перезапускаємо таймер після обчислення часу
            last_time_point = currentTime;

            if (cur_win != last_active_app) {
                if (!last_active_app.isEmpty()) {
                    StopTrackingApp(last_active_app);
                }
                StartTrackingApp(cur_win);
                last_active_app = cur_win;
            }

            UpdateUI();  // Оновлюємо інтерфейс при кожному виклику
        }
    }



    void HandleRemoveButtonClicked() {
        QPushButton* button = qobject_cast<QPushButton*>(sender());
        if (!button) return;

        // Получаем значение свойства "tableType", чтобы определить, какая таблица вызвала событие
        QString tableType = button->property("tableType").toString();

        // Определяем таблицу на основе свойства "tableType"
        QTableWidget* table = nullptr;
        if (tableType == "apps") {
            table = appsTable;
        } else if (tableType == "web") {
            table = webAppsTable;
        }

        if (!table) return;

        // Получаем индекс строки и имя приложения
        int row = table->indexAt(button->pos()).row();
        if (row < 0) return;

        QString appName = table->item(row, 0)->text();

        QMessageBox msgBox;
        msgBox.setWindowTitle("Видалення");
        msgBox.setWindowIcon(QIcon("D:/Codes/Qt_projects/App_usage_monitor/build/Desktop_Qt_6_7_2_MinGW_64_bit-Release/Icons/Постійність_пам'яті.jpg"));
        msgBox.setText("Ви хочете видалити цей елемент?");
        msgBox.setInformativeText("Виберіть 'Yes' для видалення до наступного відкриття,'No' для видалення назавжди.");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);

        int reply = msgBox.exec();

        if (reply == QMessageBox::No) {
            // Удалить только на этот сеанс
            if (table == appsTable) {
                session_data_map.insert(appName, true); // Временное удаление для десктопных приложений
            } else if (table == webAppsTable) {
                session_web_data_map.insert(appName, true); // Временное удаление для веб-приложений
            }
        } else if (reply == QMessageBox::Yes) {
            // Удалить навсегда
            if (table == appsTable) {
                data_map.remove(appName);
            } else if (table == webAppsTable) {
                web_data_map.remove(appName);
            }
        }

        UpdateUI();
    }

    void StartTrackingApp(const QString& appName) {
        LoadAppData(appName);

        if (!timeTrackers.contains(appName)) {
            TimeTracker* tracker = new TimeTracker(appName);
            QThread* thread = new QThread(this);

            tracker->moveToThread(thread);
            connect(thread, &QThread::started, tracker, &TimeTracker::startTracking);
            connect(this, &AppUsageMonitor::stopTrackingApp, tracker, &TimeTracker::stopTracking);
            connect(tracker, &TimeTracker::finished, thread, &QThread::quit);
            connect(thread, &QThread::finished, tracker, &QObject::deleteLater);
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);

            last_time_point = QDateTime::currentDateTime();  // Оновлюємо час останнього виклику тут

            timeTrackers[appName] = tracker;
            thread->start();
        } else {
            StopTrackingApp(appName);
        }

        UpdateUI();  // Оновлюємо UI для нових додатків
    }


    void StopTrackingApp(const QString& appName) {
        if (timeTrackers.contains(appName)) {
            TimeTracker* tracker = timeTrackers[appName];
            QThread* thread = tracker->thread();

            emit stopTrackingApp(appName);  // Останавливаем трекер

            if (thread) {
                thread->quit();   // Завершаем поток
                thread->wait();   // Ждем, пока поток завершится
            }

            timeTrackers.remove(appName);  // Удаляем из словаря
            tracker->deleteLater();  // Удаляем объект трекера
            thread->deleteLater();   // Удаляем объект потока

            // Сохраняем данные приложения
            SaveAppData(appName);
        }
    }

    void SaveAppData(const QString& appName) {
        QFile file(currentFileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return;
        }

        QTextStream out(&file);
         out.setEncoding(QStringConverter::Utf8);  // Використання UTF-8 кодування
        out << "Application,TimeSpent\n";

        for (auto it = data_map.begin(); it != data_map.end(); ++it) {
            out << it.key() << "," << it.value() << "\n";
        }

        for (auto it = web_data_map.begin(); it != web_data_map.end(); ++it) {
            out << it.key() << "," << it.value() << "\n";
        }

        file.close();
    }

    // void SaveData() {
    //     // Ваша логіка збереження даних, наприклад, в CSV, ODS або XLSX
    //     SaveToLibreOfficeCSV();  // Приклад виклику функції збереження

    //     dataSaved = true;  // Дані успішно збережені
    // }

    void LoadAppData(const QString& appName ) {
        QFile file(currentFileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString line = in.readLine();  // Пропускаем заголовок

        while (!in.atEnd()) {
            line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() == 2) {
                QString app = fields[0];
                int timeSpent = fields[1].toInt();

                if (isWebApplication(app)) {
                    web_data_map[app] = timeSpent;
                } else {
                    data_map[app] = timeSpent;
                }
            }
        }

        file.close();
     }

    void OpenExcel() {
        // Спочатку завантажуємо налаштування, якщо вони ще не завантажені
        if (excelPath.isEmpty()) {
            LoadSettings();  // Завантажуємо збережені налаштування при старті програми
        }

        QString path = GetExcelPath();  // Використовуємо функцію, яка перевіряє як тимчасовий, так і постійний шлях
        if (path.isEmpty()) {
            QMessageBox::warning(this, "Шлях до Excel", "Шлях до Excel не встановлений.");
            return;
        }

        // Якщо шлях тимчасовий, пропонуємо зберегти його назавжди
        if (tempExcelPath == path) {
            int reply = QMessageBox::question(this, "Зберегти шлях",
                                              "Ви використовуєте тимчасовий шлях до Excel. Бажаєте зберегти його назавжди?",
                                              QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                excelPath = tempExcelPath;  // Зберігаємо шлях назавжди
                SaveSettings();             // Зберігаємо шлях у налаштування
                QMessageBox::information(this, "Шлях збережено", "Шлях до Excel збережено назавжди.");
            }
        }

        SaveToExcel();  // Зберігаємо дані в Excel
        LaunchProgram(path, currentFileName);  // Запускаємо Excel з файлом
    }

    void OpenLibreOfficeCalc() {
        // Спочатку завантажуємо налаштування, якщо вони ще не завантажені
        if (librePath.isEmpty()) {
            LoadSettings();  // Завантажуємо збережені налаштування при старті програми
        }

        QString path = GetLibrePath();  // Використовуємо функцію, яка перевіряє як тимчасовий, так і постійний шлях
        if (path.isEmpty()) {
            QMessageBox::warning(this, "Шлях до LibreOffice", "Шлях до LibreOffice не встановлений.");
            return;
        }

        // Якщо шлях тимчасовий, пропонуємо зберегти його назавжди
        if (tempLibrePath == path) {
            int reply = QMessageBox::question(this, "Зберегти шлях",
                                              "Ви використовуєте тимчасовий шлях до LibreOffice. Бажаєте зберегти його назавжди?",
                                              QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                librePath = tempLibrePath;  // Зберігаємо шлях назавжди
                SaveSettings();             // Зберігаємо шлях у налаштування
                QMessageBox::information(this, "Шлях збережено", "Шлях до LibreOffice збережено назавжди.");
            }
        }

        SaveToExcel();  // Зберігаємо дані в Excel
        LaunchProgram(path, currentFileName);  // Запускаємо LibreOffice з файлом
    }



    void ShowHelperWindow(const QString& title, const QString& message) {
        QMessageBox::information(this, title, message, QMessageBox::Ok);
    }

    // Вибір шляху до Excel з можливістю тимчасового збереження
    void SetExcelPath() {
        QString path = QFileDialog::getOpenFileName(this, "Виберіть Excel", QString(), "Executables (*.exe)");
        if (!path.isEmpty()) {
            // Показуємо діалогове вікно з питанням про збереження шляху
            int reply = QMessageBox::question(this, "Зберегти шлях",
                                              "Бажаєте зберегти цей шлях до Excel назавжди? Натисніть Yes для збереження назавжди або No для збереження до закриття програми.",
                                              QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                excelPath = path;  // Зберігаємо шлях назавжди
                SaveSettings();    // Зберігаємо шлях у налаштування
                QMessageBox::information(this, "Шлях збережено", "Шлях до Excel збережено назавжди.");
            } else if (reply == QMessageBox::No) {
                tempExcelPath = path;  // Зберігаємо тимчасово до закриття програми
                QMessageBox::information(this, "Тимчасовий шлях", "Шлях до Excel збережено до закриття програми.");
            }
        }
    }

    // Вибір шляху до LibreOffice з можливістю тимчасового збереження
    void SetLibrePath() {
        QString path = QFileDialog::getOpenFileName(this, "Виберіть LibreOffice", QString(), "Executables (*.exe)");
        if (!path.isEmpty()) {
            // Показуємо діалогове вікно з питанням про збереження шляху
            int reply = QMessageBox::question(this, "Зберегти шлях",
                                              "Бажаєте зберегти цей шлях до LibreOffice назавжди? Натисніть Yes для збереження назавжди або No для збереження до закриття програми.",
                                              QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                librePath = path;  // Зберігаємо шлях назавжди
                SaveSettings();    // Зберігаємо шлях у налаштування
                QMessageBox::information(this, "Шлях збережено", "Шлях до LibreOffice збережено назавжди.");
            } else if (reply == QMessageBox::No) {
                tempLibrePath = path;  // Зберігаємо тимчасово до закриття програми
                QMessageBox::information(this, "Тимчасовий шлях", "Шлях до LibreOffice збережено до закриття програми.");
            }
        }
    }

    // Функція для отримання шляху до Excel
    QString GetExcelPath() {
        if (!tempExcelPath.isEmpty()) {
            return tempExcelPath;  // Якщо тимчасовий шлях встановлено, використовуємо його
        } else if (!excelPath.isEmpty()) {
            return excelPath;  // Якщо постійний шлях встановлено, використовуємо його
        } else {
            return QString();  // Якщо шляхи не встановлені, повертаємо порожній рядок
        }
    }

    // Функція для отримання шляху до LibreOffice
    QString GetLibrePath() {
        if (!tempLibrePath.isEmpty()) {
            return tempLibrePath;  // Якщо тимчасовий шлях встановлено, використовуємо його
        } else if (!librePath.isEmpty()) {
            return librePath;  // Якщо постійний шлях встановлено, використовуємо його
        } else {
            return QString();  // Якщо шляхи не встановлені, повертаємо порожній рядок
        }
    }

    // "C:\Program Files (x86)\LibreOffice\program\scalc.exe"

    // Функція для збереження шляхів назавжди
    void SaveSettings() {
        QSettings settings("MyCompany", "MyApp");

        // Зберігаємо постійно шляхи до Excel і LibreOffice
        if (!excelPath.isEmpty()) {
            settings.setValue("paths/excel", excelPath);
        }

        if (!librePath.isEmpty()) {
            settings.setValue("paths/libre", librePath);
        }
    }

    // Функція для завантаження шляхів із збережених налаштувань
    void LoadSettings() {
        QSettings settings("MyCompany", "MyApp");

        // Завантажуємо постійно збережені шляхи до Excel і LibreOffice
        excelPath = settings.value("paths/excel", "").toString();
        librePath = settings.value("paths/libre", "").toString();
    }

    void closeEvent(QCloseEvent *event) {
        SaveSettings();  // Зберігаємо всі налаштування перед закриттям програми
        QMainWindow::closeEvent(event);
    }

    void applyLightMode() {
        setStyleSheet(
            "QMainWindow { background-color: #F8F4E3; }"
            "QMenuBar { background-color: #EDE3CF; color: #4A3F35; }"
            "QMenuBar::item { background-color: #EDE3CF; color: #4A3F35; }"
            "QMenuBar::item:selected { background-color: #D1C7B7; }"
            "QMenu { background-color: #EDE3CF; color: #4A3F35; }"
            "QMenu::item:selected { background-color: #D1C7B7; }"
            "QTableWidget { background-color: #F3F0E7; alternate-background-color: #E8E2D1; }"
            "QHeaderView::section { background-color: #D1C7B7; color: #4A3F35; border: 1px solid #D1C7B7; }"
            "QHeaderView::section:vertical { background-color: #D1C7B7; color: #4A3F35; }"
            "QTableView::verticalHeader { background-color: #D1C7B7; color: #4A3F35; }"
            "QTableCornerButton::section { background-color: #D1C7B7; border: 1px solid #D1C7B7; }"
            "QLabel { color: #4A3F35; }"
            );
    }

    void applyNightMode() {
        setStyleSheet(
            "QMainWindow { background-color: #2E2E2E; }"
            "QMenuBar { background-color: #3C3C3C; color: #FFFFFF; }"
            "QMenuBar::item { background-color: #3C3C3C; color: #FFFFFF; }"
            "QMenuBar::item:selected { background-color: #505050; }"
            "QMenu { background-color: #3C3C3C; color: #FFFFFF; }"
            "QMenu::item:selected { background-color: #505050; }"
            "QTableWidget { background-color: #3A3A3A; alternate-background-color: #4B4B4B; color: #000000; }"  // Чорний текст у таблиці
            "QTableWidget::item { color: #000000; }"  // Чорний текст для елементів
            "QHeaderView::section { background-color: #505050; color: #FFFFFF; border: 1px solid #505050; }"
            "QHeaderView::section:vertical { background-color: #505050; color: #FFFFFF; }"
            "QTableView::verticalHeader { background-color: #505050; color: #FFFFFF; }"
            "QTableCornerButton::section { background-color: #505050; border: 1px solid #505050; }"
            "QLabel { color: #FFFFFF; }"
            );
    }


    // Функція TextAboutDialog2 викликає метод ShowAboutDialog об'єкта mywindow
    void TextAboutDialog2() {
        // Виклик методу ShowAboutDialog через об'єкт 'window' з дужками
        this->window.ShowAboutDialog();
    }


    void LaunchApplication(const QString& applicationPath, const QString& filePath) {
        QProcess::startDetached(applicationPath, QStringList() << filePath);
    }

    void SaveToExcel() {
        QXlsx::Document xlsx;
        int row = 1;

        // Перша копія (незмінна) в колонки A та B
        for (auto it = data_map.begin(); it != data_map.end(); ++it) {
            xlsx.write(row, 1, it.key());   // Колонка A
            xlsx.write(row, 2, it.value()); // Колонка B
            row++;
        }

        for (auto it = web_data_map.begin(); it != web_data_map.end(); ++it) {
            xlsx.write(row, 1, it.key());   // Колонка A
            xlsx.write(row, 2, it.value()); // Колонка B
            row++;
        }

        // Друга копія (редагована) в колонки D та E
        row = 1; // Починаємо знову з першого рядка
        for (auto it = data_map.begin(); it != data_map.end(); ++it) {
            xlsx.write(row, 4, it.key());   // Колонка D
            xlsx.write(row, 5, it.value()); // Колонка E
            row++;
        }

        for (auto it = web_data_map.begin(); it != web_data_map.end(); ++it) {
            xlsx.write(row, 4, it.key());   // Колонка D
            xlsx.write(row, 5, it.value()); // Колонка E
            row++;
        }

        // Збереження файлу
        xlsx.saveAs(currentFileName);
    }

    //ShowAboutDialog() у easter_egg.cpp

    void MinimizeWindow() {
        resize(400, 400);
    }

    void MaximizeWindow() {
        resize(800, 600);
    }

    void SortByTimeAscending() {
        appsTable->sortItems(1, Qt::AscendingOrder);
        webAppsTable->sortItems(1, Qt::AscendingOrder);
    }

    void SortByTimeDescending() {
        appsTable->sortItems(1, Qt::DescendingOrder);
        webAppsTable->sortItems(1, Qt::DescendingOrder);
    }

    void SortByNameAscending() {
        appsTable->sortItems(0, Qt::AscendingOrder);
        webAppsTable->sortItems(0, Qt::AscendingOrder);
    }

    void SortByNameDescending() {
        appsTable->sortItems(0, Qt::DescendingOrder);
        webAppsTable->sortItems(0, Qt::DescendingOrder);
    }

signals:
    void stopTrackingApp(const QString& appName);

private:

 void  clearData() {
        // Очищення даних
        data_map.clear();
        web_data_map.clear();
        session_data_map.clear();
        session_web_data_map.clear();

        // Очищення таблиць перед додаванням даних
        if (appsTable != nullptr && appsTable->rowCount() > 0) {
            appsTable->blockSignals(true);  // Блокуємо сигнали для запобігання зайвим подіям
            appsTable->clearContents();     // Очищаємо вміст таблиці
            appsTable->setRowCount(0);      // Встановлюємо кількість рядків у 0
            appsTable->blockSignals(false); // Розблоковуємо сигнали
        }

        if (webAppsTable != nullptr && webAppsTable->rowCount() > 0) {
            webAppsTable->blockSignals(true);  // Блокуємо сигнали для запобігання зайвим подіям
            webAppsTable->clearContents();     // Очищаємо вміст таблиці
            webAppsTable->setRowCount(0);      // Встановлюємо кількість рядків у 0
            webAppsTable->blockSignals(false); // Розблоковуємо сигнали
        }
    }

    void LaunchProgram(const QString& programPath, const QString& fileName)
    {
        QStringList arguments;
        arguments << fileName;

        QProcess* process = new QProcess(this);
        process->start(programPath, arguments);
    }

    void ConfigureTable_UI(QTableWidget* table)
    {

        // Встановлюємо фіксовану ширину для кожного стовпця
        table->setColumnWidth(0, 200);  // "Application" або "Web Application"
        table->setColumnWidth(1, 150);  // "Time Spent"
        table->setColumnWidth(2, 100);   // "Current opened"
        table->setColumnWidth(3, 50);   // "Remove"

        // Налаштування для розтягування останнього стовпця
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

        // Чергування кольорів рядків
        table->setAlternatingRowColors(true);

        // Оновлення зовнішнього вигляду таблиці
        UpdateTableAppearance_UI(table);
    }

    void UpdateUI() {
        if (isFirstRun) {
            clearData();
            isFirstRun = false;
        }

        appsTable->setRowCount(0);
        webAppsTable->setRowCount(0);

        QColor foregroundColor(74, 63, 53);

        HWND hwnd = GetForegroundWindow();
        char window_title[256];
        GetWindowTextA(hwnd, window_title, sizeof(window_title));
        QString activeWindow = QString::fromLocal8Bit(window_title);

        QMap<QString, int>::iterator i;
        for (i = data_map.begin(); i != data_map.end(); ++i) {
            if (!session_data_map.contains(i.key())) {
                int row = appsTable->rowCount();
                appsTable->insertRow(row);

                QTableWidgetItem* appItem = new QTableWidgetItem(i.key());
                QTableWidgetItem* timeItem = new QTableWidgetItem(formatTime(i.value()));

                QTableWidgetItem* statusItem = new QTableWidgetItem("Inactive");
                if (i.key() == activeWindow) {
                    statusItem->setText("Active");
                }

                appItem->setFlags(appItem->flags() & ~Qt::ItemIsEditable);
                timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
                statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);

                appItem->setBackground(QColor(209, 199, 183));
                timeItem->setBackground(QColor(209, 199, 183));
                statusItem->setBackground(QColor(209, 199, 183));

                appItem->setForeground(foregroundColor);
                timeItem->setForeground(foregroundColor);
                statusItem->setForeground(foregroundColor);

                appsTable->setItem(row, 0, appItem);
                appsTable->setItem(row, 1, timeItem);

                if (i.key() == activeWindow) {
                    GradientWidget* gradientWidget = new GradientWidget(this);
                   // set_gradient_design(this);
                    appsTable->setCellWidget(row, 2, gradientWidget);
                } else {
                    appsTable->setItem(row, 2, statusItem);
                }

                QPushButton* removeButton = new QPushButton("Remove");
                removeButton->setStyleSheet("background-color: #e36486; color: white;");
                removeButton->setProperty("tableType", "apps");
                connect(removeButton, &QPushButton::clicked, this, &AppUsageMonitor::HandleRemoveButtonClicked);
                appsTable->setCellWidget(row, 3, removeButton);
            }
        }

        for (i = web_data_map.begin(); i != web_data_map.end(); ++i) {
            if (!session_web_data_map.contains(i.key())) {
                int row = webAppsTable->rowCount();
                webAppsTable->insertRow(row);

                QTableWidgetItem* webAppItem = new QTableWidgetItem(i.key());
                QTableWidgetItem* webTimeItem = new QTableWidgetItem(formatTime(i.value()));

                QTableWidgetItem* webStatusItem = new QTableWidgetItem("Inactive");
                if (i.key() == activeWindow) {
                    webStatusItem->setText("Active");
                }

                webAppItem->setFlags(webAppItem->flags() & ~Qt::ItemIsEditable);
                webTimeItem->setFlags(webTimeItem->flags() & ~Qt::ItemIsEditable);
                webStatusItem->setFlags(webStatusItem->flags() & ~Qt::ItemIsEditable);

                webAppItem->setBackground(QColor(209, 199, 183));
                webTimeItem->setBackground(QColor(209, 199, 183));
                webStatusItem->setBackground(QColor(209, 199, 183));

                webAppItem->setForeground(foregroundColor);
                webTimeItem->setForeground(foregroundColor);
                webStatusItem->setForeground(foregroundColor);

                webAppsTable->setItem(row, 0, webAppItem);
                webAppsTable->setItem(row, 1, webTimeItem);

                if (i.key() == activeWindow) {
                    GradientWidget* gradientWidget = new GradientWidget(this);
                    webAppsTable->setCellWidget(row, 2, gradientWidget);
                } else {
                    webAppsTable->setItem(row, 2, webStatusItem);
                }

                QPushButton* removeButton = new QPushButton("Remove");
                removeButton->setStyleSheet("background-color: #e36486; color: white;");
                removeButton->setProperty("tableType", "web");
                connect(removeButton, &QPushButton::clicked, this, &AppUsageMonitor::HandleRemoveButtonClicked);
                webAppsTable->setCellWidget(row, 3, removeButton);
            }
        }

        appsTable->repaint();
        webAppsTable->repaint();
    }

    bool isActiveApplication(const QString& appName) {
        HWND hwnd = GetForegroundWindow();
        if (!hwnd) {
            return false;  // Якщо немає активного вікна, додаток не активний
        }

        // Отримуємо назву активного вікна
        char windowTitle[256];
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
        QString activeWindowTitle = QString::fromLocal8Bit(windowTitle);

        // Перевіряємо, чи назва активного вікна відповідає назві додатку
        if (activeWindowTitle.contains(appName, Qt::CaseInsensitive)) {
            return true;
        }

        // Додатково можна перевірити, чи процес додатку активний (наприклад, у Windows)
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);

        QProcess process;
        process.start("tasklist", QStringList() << "/FI" << QString("PID eq %1").arg(pid));
        process.waitForFinished();

        QString output = process.readAllStandardOutput();
        if (output.contains(appName, Qt::CaseInsensitive)) {
            return true;
        }

        // Якщо жодна з умов не виконана, повертаємо false
        return false;
    }

    QString formatTime(int seconds) {
        int days = seconds / 86400;
        seconds %= 86400;
        int hours = seconds / 3600;
        seconds %= 3600;
        int minutes = seconds / 60;
        seconds %= 60;

        QStringList timeParts;

        if (days > 0) {
            timeParts << QString::number(days) + " дн";
        }
        if (hours > 0) {
            timeParts << QString::number(hours) + " год";
        }
        if (minutes > 0) {
            timeParts << QString::number(minutes) + " мин";
        }
        if (seconds > 0 || timeParts.isEmpty()) {
            timeParts << QString::number(seconds) + " сек";
        }

        return timeParts.join(" ");
    }

    void UpdateTableAppearance_UI(QTableWidget* table) {
        int rowCount = table->rowCount();
        int columnCount = table->columnCount();

        QColor backgroundColor(209, 199, 183);  // #D1C7B7
        QColor foregroundColor(74, 63, 53);  // #4A3F35

        for (int row = 0; row < rowCount; ++row) {
            for (int col = 0; col < columnCount; ++col) {
                QTableWidgetItem* item = table->item(row, col);
                if (item) {
                    item->setBackground(backgroundColor);
                    item->setForeground(foregroundColor);
                } else {
                    item = new QTableWidgetItem();
                    item->setBackground(backgroundColor);
                    item->setForeground(foregroundColor);
                    table->setItem(row, col, item);
                }
            }
        }

        // Налаштування заголовків стовпців
        QHeaderView* header = table->horizontalHeader();
        header->setStyleSheet("QHeaderView::section { background-color: #D1C7B7; color: #4A3F35; }");

        // Оновлення після застосування всіх налаштувань
        table->repaint();
    }

    bool isWebApplication(const QString& appName) {
        // Проверка, является ли приложение веб-приложением по его названию
        return appName.contains("Chrome") || appName.contains("Firefox") || appName.contains("Edge") || appName.contains("Safari");
    }




    QString excelPath;
    QString librePath;

    QVBoxLayout* main_layout;
    QTableWidget* appsTable;
    QTableWidget* webAppsTable;
    QMap<QString, int> data_map; // Десктопные приложения
    QMap<QString, int> web_data_map; // Веб-приложения
    QMap<QString, bool> session_data_map; // Временные удаления для десктопных приложений
    QMap<QString, bool> session_web_data_map; // Временные удаления для веб-приложений
    QString last_active_app;
    QDateTime last_time_point;
    QString currentFileName;
    QMap<QString, TimeTracker*> timeTrackers;
    bool isFirstRun;  // Прапорець для відстеження першого запуску
    QElapsedTimer elapsedTimer;  // Додаємо таймер для точного вимірювання часу
    QTableWidget* tableWidget;
    // Флаг для перевірки, чи були дані збережені
    bool dataSaved = false;
     MyWindow window;  // Додаємо екземпляр MyWindow

};

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);  // Ініціалізація програми

    AppUsageMonitor w;
    w.show();  // Показ основного вікна додатку

    MyWindow mywindow;
    mywindow.show();  // Спочатку показуємо вікно

    mywindow.hide();  // Тепер ховаємо його після показу

    a.installEventFilter(&mywindow);  // Встановлюємо глобальний фільтр подій для обробки клавіш

    return a.exec();  // Запуск основного циклу програми
}




#include "main.moc"
