#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QProcess>
#include <QListWidgetItem>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_search_edit_returnPressed();

    void processReadyRead();
    void processFinished(int exitCode);
    void on_search_button_clicked();
    void on_search_edit_textChanged(const QString &arg1);
    void on_resultList_itemDoubleClicked(QListWidgetItem *item);


    void getLyrics(const QString url);
    void slot_netwManagerFinished(QNetworkReply *reply);
    void on_themeSwitch_toggled(bool checked);

    void setStyle(QString fname);
private:
    Ui::MainWindow *ui;
    QProcess *process = nullptr;
    QList<QStringList> lastResultList;
    QStyle *dafaultStyle;
};

#endif // MAINWINDOW_H
