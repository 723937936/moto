#include "widget.h"
#include "ui_widget.h"
#include <QtNetwork/QtNetwork>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    timer = new QTimer{this};
    connect(timer, SIGNAL(timeout()), this, SLOT(doQuery()));
    tts = new QTextToSpeech{this};
}

Widget::~Widget()
{
    delete ui;
}

void Widget::doQuery()
{
    QString date = ui->dateEdit->date().toString("yyyy.MM.dd");
    QJsonObject res = post(QString{"date="} + date);
    int n = res.value("remaining_number").toInt();
    ui->remainingLabel->setText(QString::number(n));
    ui->counterLabel->setText(QString::number(++counter));
    if (n > 0)
        say("有号啦");
}

QJsonObject Widget::post(const QString &data)
{
    QNetworkAccessManager mgr;
    QNetworkRequest req;
    req.setUrl(QUrl{QString{"http://appointment.jundie.net/home/get_surplus_number"}});
    req.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    QNetworkReply *reply = mgr.post(req, data.toUtf8());
    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        return QJsonObject{};
    }
    QByteArray res = reply->readAll();
    reply->deleteLater();
    QJsonDocument json = QJsonDocument::fromJson(res);
    QJsonObject obj = json.object();
    qDebug() << obj;
    return obj;
}

void Widget::on_pushButton_clicked()
{
    if (!flag) {
        ui->pushButton->setText("停止查询");
        ui->remainingLabel->setText("0");
        counter = 0;
        doQuery();
        timer->start(5000);
    } else {
        ui->pushButton->setText("开始查询");
        timer->stop();
    }

    flag = !flag;
    ui->dateEdit->setReadOnly(flag);
}

void Widget::say(const char *s)
{
    tts->say(s);
}
