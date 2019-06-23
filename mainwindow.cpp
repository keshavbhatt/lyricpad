#include "mainwindow.h"
#include "elidedlabel.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QPixmapCache>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QtNetwork>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  // init
  this->setWindowTitle(QApplication::applicationName());
  ui->resultCount->hide();
  ui->noResult->hide();
  ui->progressBar->hide();

  // setTheme
  setStyle(":/light.qss");
  bool checked = ui->themeSwitch->isChecked();
  ui->search_button->setIcon(
      QIcon(checked ? ":/dark/search.png" : ":/light/search.png"));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_search_edit_returnPressed() {
  if (ui->search_edit->text().trimmed().isEmpty())
    return;

  process = new QProcess(nullptr);
  QString script_path = QApplication::applicationDirPath() + "/lyrics.py";

  QFile file(script_path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  connect(process, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
  connect(process, SIGNAL(finished(int)), this, SLOT(processFinished(int)));

  process->start("python", QStringList() << script_path
                                         << ui->search_edit->text());
  ui->progressBar->show();
}

void MainWindow::processReadyRead() {
  lastResultList.clear();
  // parse result to list
  QString strReply = static_cast<QString>(process->readAll());
 // strReply.replace("'", "\"");
  QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
  QJsonArray jsonArray = jsonResponse.array();

  if (jsonArray.count() > 0) {
    ui->noResult->hide();
    ui->resultCount->show();
    ui->resultCount->setText("Returned " + QString::number(jsonArray.count()) +
                             " results for your query.");

    foreach (const QJsonValue &value, jsonArray) {
      QJsonObject obj = value.toObject();
      QString id = obj.value("song_id").toString();
      QString url = obj.value("song_url").toString();
      QString artist = obj.value("song_artist").toString();
      QString title = obj.value("song_name").toString();
      QStringList item;
      item << id << url << artist << title;
      lastResultList.append(item);

      ui->resultList->addItem(title + " - " + artist);
    }

  } else {
    ui->noResult->show();
    ui->resultCount->hide();
  }
}

void MainWindow::processFinished(int exitCode) {
  ui->progressBar->hide();
  Q_UNUSED(exitCode);
}

void MainWindow::on_search_button_clicked() { on_search_edit_returnPressed(); }

void MainWindow::on_search_edit_textChanged(const QString &arg1) {
  ui->search_button->setEnabled(!arg1.trimmed().isEmpty());
  ui->resultList->clear();
  ui->resultCount->hide();
  ui->noResult->hide();
}

void MainWindow::on_resultList_itemDoubleClicked(QListWidgetItem *item) {
  int itemNum = ui->resultList->row(item);
  getLyrics(lastResultList.at(itemNum).at(1));
}

void MainWindow::getLyrics(const QString url) {
  ui->progressBar->show();
  QNetworkAccessManager *m_netwManager = new QNetworkAccessManager(this);
  connect(m_netwManager, SIGNAL(finished(QNetworkReply *)), this,
          SLOT(slot_netwManagerFinished(QNetworkReply *)));
  QNetworkRequest request((QUrl(url)));
  request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) "
                                     "AppleWebKit/537.36 (KHTML, like Gecko) "
                                     "Chrome/73.0.3683.103 Safari/537.36");
  m_netwManager->get(request);
}

void MainWindow::slot_netwManagerFinished(QNetworkReply *reply) {

  if (reply->error() == QNetworkReply::NoError) {
    int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (v >= 200 && v < 300) // Success
    {
      QString html = reply->readAll();
      ui->result->setHtml("<center>" +
                          html.split("<div>").last().split("</div>").first() +
                          "</center>");
    } else if (v >= 300 && v < 400) // Redirection
    {
      QUrl newUrl =
          reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
      newUrl = reply->url().resolved(newUrl);
      qDebug() << "request redirected to new url" << newUrl;

      QNetworkAccessManager *manager = new QNetworkAccessManager();
      connect(manager, SIGNAL(finished(QNetworkReply *)), this,
              SLOT(slot_netwManagerFinished(QNetworkReply *)));
      QNetworkRequest request((QUrl(newUrl)));
      request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) "
                                         "AppleWebKit/537.36 (KHTML, like "
                                         "Gecko) Chrome/73.0.3683.103 "
                                         "Safari/537.36");
      manager->get(request);
      ui->progressBar->show();
    }
  } else {
    qDebug() << reply->errorString();
  }
  ui->progressBar->hide();
}

void MainWindow::setStyle(QString fname) {
  QFile styleSheet(fname);
  if (fname.contains("dark"))
    ui->themeSwitch->setChecked(true);
  else
    ui->themeSwitch->setChecked(false);
  if (!styleSheet.open(QIODevice::ReadOnly)) {
    qWarning("Unable to open file");
    return;
  }
  qApp->setStyleSheet(styleSheet.readAll());

  styleSheet.close();
}

void MainWindow::on_themeSwitch_toggled(bool checked) {
  setStyle(checked ? ":/dark.qss" : ":/light.qss");
  ui->search_button->setIcon(
      QIcon(checked ? ":/dark/search.png" : ":/light/search.png"));
}
