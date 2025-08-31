#ifndef USAGETABLEWIDGET_H
#define USAGETABLEWIDGET_H

#include <QTableWidget>
#include <QMap>
#include "gradient.h"

class UsageTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    enum TableType { Desktop, Web };

    explicit UsageTableWidget(TableType type, QWidget *parent = nullptr);

    void updateData(const QMap<QString, int> &dataMap,
                    const QMap<QString, bool> &sessionMap,
                    const QString &activeWindow);

    static QString formatTime(int seconds);

signals:
    void removeRequested(const QString &appName, bool permanent);

private slots:
    void handleRemoveButtonClicked();

private:
    TableType m_tableType;
    QString m_activeWindow;

    void setupTable();
    void updateRowAppearance(int row, const QString &appName);

    // Константи для колонок
    enum Columns {
        COL_NAME = 0,
        COL_TIME = 1,
        COL_STATUS = 2,
        COL_REMOVE = 3
    };
};

#endif
