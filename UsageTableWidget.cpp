#include "UsageTableWidget.h"
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QColor>

namespace {
constexpr int COL_NAME_WIDTH = 200;
constexpr int COL_TIME_WIDTH = 150;
constexpr int COL_STATUS_WIDTH = 100;
constexpr int COL_REMOVE_WIDTH = 50;
}

UsageTableWidget::UsageTableWidget(TableType type, QWidget *parent)
    : QTableWidget(0, 4, parent), m_tableType(type)
{
    QString title = (type == Desktop) ? "Application" : "Web Application";
    setHorizontalHeaderLabels({ title, "Time Spent", "Current opened", "Remove" });
    setupTable();
}

void UsageTableWidget::setupTable()
{
    setColumnWidth(COL_NAME, COL_NAME_WIDTH);
    setColumnWidth(COL_TIME, COL_TIME_WIDTH);
    setColumnWidth(COL_STATUS, COL_STATUS_WIDTH);
    setColumnWidth(COL_REMOVE, COL_REMOVE_WIDTH);

    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    setAlternatingRowColors(true);

    // Стилізація заголовків
    horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "    background-color: #D1C7B7;"
        "    color: #4A3F35;"
        "    border: 1px solid #D1C7B7;"
        "    padding: 4px;"
        "}"
        );
}

void UsageTableWidget::updateData(const QMap<QString, int> &dataMap,
                                  const QMap<QString, bool> &sessionMap,
                                  const QString &activeWindow)
{
    m_activeWindow = activeWindow;
    setRowCount(0);

    QColor backgroundColor(209, 199, 183);
    QColor foregroundColor(74, 63, 53);

    for (auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        if (sessionMap.contains(it.key())) continue;

        int row = rowCount();
        insertRow(row);

        // Назва додатку
        QTableWidgetItem *appItem = new QTableWidgetItem(it.key());
        appItem->setFlags(appItem->flags() & ~Qt::ItemIsEditable);
        appItem->setBackground(backgroundColor);
        appItem->setForeground(foregroundColor);
        setItem(row, COL_NAME, appItem);

        // Час
        QTableWidgetItem *timeItem = new QTableWidgetItem(formatTime(it.value()));
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        timeItem->setBackground(backgroundColor);
        timeItem->setForeground(foregroundColor);
        setItem(row, COL_TIME, timeItem);

        // Статус - градієнт або текст
        if (it.key() == activeWindow) {
            GradientWidget* gradientWidget = new GradientWidget();
            setCellWidget(row, COL_STATUS, gradientWidget);
        } else {
            QTableWidgetItem *statusItem = new QTableWidgetItem("Inactive");
            statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
            statusItem->setBackground(backgroundColor);
            statusItem->setForeground(foregroundColor);
            setItem(row, COL_STATUS, statusItem);
        }

        // Кнопка видалення
        QPushButton *removeButton = new QPushButton("×");
        removeButton->setStyleSheet(
            "QPushButton {"
            "    background-color: #e36486;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 3px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #d35472;"
            "}"
            );
        removeButton->setProperty("appName", it.key());
        connect(removeButton, &QPushButton::clicked,
                this, &UsageTableWidget::handleRemoveButtonClicked);
        setCellWidget(row, COL_REMOVE, removeButton);
    }
}

void UsageTableWidget::handleRemoveButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString appName = button->property("appName").toString();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Видалення");
    msgBox.setText("Ви хочете видалити цей елемент?");
    msgBox.setInformativeText("'Так' - видалити до наступного відкриття\n'Ні' - видалити назавжди");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int reply = msgBox.exec();

    if (reply == QMessageBox::Yes) {
        emit removeRequested(appName, false); // Тимчасово
    } else if (reply == QMessageBox::No) {
        emit removeRequested(appName, true);  // Назавжди
    }
}

QString UsageTableWidget::formatTime(int seconds)
{
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
