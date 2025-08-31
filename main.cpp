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
#include <windows.h>
#include "xlsxdocument.h"  // Подключаем QXlsx для Excel
#include "eastor_egg.h"
#include "UsageTableWidget.h"

QString tempExcelPath;
QString tempLibrePath;

class AppUsageMonitor : public QMainWindow {
    Q_OBJECT

public:
    AppUsageMonitor(QWidget* parent = nullptr)
        : QMainWindow(parent), last_active_app(""), last_time_point(QDateTime::currentDateTime()), currentFileName("AppUsageData.xlsx"),isFirstRun(true)
    {
        setWindowTitle("App Usage Monitor");
        setWindowIcon(QIcon( "C:/Users/fipok/Downloads/OIP.png"));
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

        QWidget* central_widget = new QWidget(this);
        main_layout = new QVBoxLayout(central_widget);


        appsTable = new UsageTableWidget(UsageTableWidget::Desktop, central_widget);
        appsTable->setHorizontalHeaderLabels({ "Application", "Time Spent", "Current opened", "Remove" });
        main_layout->addWidget(new QLabel("Applications", this));
        main_layout->addWidget(appsTable);


        // Новый виджет и макет для веб-приложений
        QWidget* webAppsWidget = new QWidget(this);
        QVBoxLayout* webAppsLayout = new QVBoxLayout(webAppsWidget);

        webAppsTable = new UsageTableWidget(UsageTableWidget::Web, central_widget);
        webAppsTable->setHorizontalHeaderLabels({ "Web Application", "Time Spent", "Current opened", "Remove" });
        webAppsLayout->addWidget(new QLabel("Web Applications", this));
        webAppsLayout->addWidget(webAppsTable);


        main_layout->addWidget(webAppsWidget);

        setCentralWidget(central_widget);

        // Підключення слоту ShowAboutDialog
        connect(aboutAction, &QAction::triggered, this, &AppUsageMonitor::TextAboutDialog2);
        connect(minimizeAction, &QAction::triggered, this, &AppUsageMonitor::MinimizeWindow);
        connect(maximizeAction, &QAction::triggered, this, &AppUsageMonitor::MaximizeWindow);
        connect(toggleNightModeAction, &QAction::triggered, this, &AppUsageMonitor::applyNightMode);
        connect(toggleLightModeAction, &QAction::triggered, this, &AppUsageMonitor::applyLightMode);

        connect(sortAscAction, &QAction::triggered, appsTable, &UsageTableWidget::sortByTimeAscending);
        connect(sortAscAction, &QAction::triggered, webAppsTable, &UsageTableWidget::sortByTimeAscending);
        connect(sortDescAction, &QAction::triggered, appsTable, &UsageTableWidget::sortByTimeDescending);
        connect(sortDescAction, &QAction::triggered, webAppsTable, &UsageTableWidget::sortByTimeDescending);
        connect(sortAlphaAscAction, &QAction::triggered, appsTable, &UsageTableWidget::sortByNameAscending);
        connect(sortAlphaAscAction, &QAction::triggered, webAppsTable, &UsageTableWidget::sortByNameAscending);
        connect(sortAlphaDescAction, &QAction::triggered, appsTable, &UsageTableWidget::sortByNameDescending);
        connect(sortAlphaDescAction, &QAction::triggered, webAppsTable, &UsageTableWidget::sortByNameDescending);

        connect(openExel, &QAction::triggered, this, &AppUsageMonitor::OpenExcel);
        connect(openLibreAction, &QAction::triggered, this, &AppUsageMonitor::OpenLibreOfficeCalc);
        connect(setExcelPathAction, &QAction::triggered, this, &AppUsageMonitor::SetExcelPath);
        connect(setLibrePathAction, &QAction::triggered, this, &AppUsageMonitor::SetLibrePath);

        connect(appsTable, &UsageTableWidget::removeRequested,
                this, &AppUsageMonitor::handleRemoveRequest);
        connect(webAppsTable, &UsageTableWidget::removeRequested,
                this, &AppUsageMonitor::handleRemoveRequest);

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

    void handleRemoveRequest(const QString &appName, bool permanent)
    {
        if (isWebApplication(appName)) {
            if (permanent) {
                web_data_map.remove(appName);
                // Видалити з файлу, якщо потрібно
                SaveAppData(appName); // Можливо, вам потрібно оновити цей метод
            } else {
                session_web_data_map.insert(appName, true);
            }
        } else {
            if (permanent) {
                data_map.remove(appName);
                SaveAppData(appName);
            } else {
                session_data_map.insert(appName, true);
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

        xlsx.saveAs(currentFileName);
    }

    void MinimizeWindow() {
        resize(400, 400);
    }

    void MaximizeWindow() {
        resize(800, 600);
    }

signals:
    void stopTrackingApp(const QString& appName);

private:

    void clearData()
    {
        data_map.clear();
        web_data_map.clear();
        session_data_map.clear();
        session_web_data_map.clear();

        if (appsTable) {
            appsTable->setRowCount(0);
        }
        if (webAppsTable) {
            webAppsTable->setRowCount(0);
        }
    }

    void LaunchProgram(const QString& programPath, const QString& fileName)
    {
        QStringList arguments;
        arguments << fileName;

        QProcess* process = new QProcess(this);
        process->start(programPath, arguments);
    }


    void UpdateUI()
    {
        if (isFirstRun) {
            clearData();
            isFirstRun = false;
        }

        HWND hwnd = GetForegroundWindow();
        char window_title[256];
        GetWindowTextA(hwnd, window_title, sizeof(window_title));
        QString activeWindow = QString::fromLocal8Bit(window_title);

        // Зберігаємо стан сортування перед оновленням
        int appsSortColumn = appsTable->horizontalHeader()->sortIndicatorSection();
        Qt::SortOrder appsSortOrder = appsTable->horizontalHeader()->sortIndicatorOrder();

        int webAppsSortColumn = webAppsTable->horizontalHeader()->sortIndicatorSection();
        Qt::SortOrder webAppsSortOrder = webAppsTable->horizontalHeader()->sortIndicatorOrder();

        // Оновлюємо дані
        appsTable->updateData(data_map, session_data_map, activeWindow);
        webAppsTable->updateData(web_data_map, session_web_data_map, activeWindow);

        // Відновлюємо сортування
        if (appsSortColumn != -1) {
            appsTable->sortItems(appsSortColumn, appsSortOrder);
        }
        if (webAppsSortColumn != -1) {
            webAppsTable->sortItems(webAppsSortColumn, webAppsSortOrder);
        }
    }

    bool isWebApplication(const QString& appName) {
        return appName.contains("Chrome") || appName.contains("Firefox") || appName.contains("Edge") || appName.contains("Safari");
    }




    QString excelPath;
    QString librePath;

    UsageTableWidget* appsTable;
    UsageTableWidget* webAppsTable;
    QVBoxLayout* main_layout;
    QMap<QString, int> data_map;
    QMap<QString, int> web_data_map;
    QMap<QString, bool> session_data_map;
    QMap<QString, bool> session_web_data_map;
    QString last_active_app;
    QDateTime last_time_point;
    QString currentFileName;
    QMap<QString, TimeTracker*> timeTrackers;
    bool isFirstRun;
    QElapsedTimer elapsedTimer;  // Додаємо таймер для точного вимірювання часу
    QTableWidget* tableWidget;
    bool dataSaved = false;
     MyWindow window;

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
