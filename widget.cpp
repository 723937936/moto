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
    connect(timer, SIGNAL(timeout()), this, SLOT(query()));
    tts = new QTextToSpeech{this};
}

Widget::~Widget()
{
    delete ui;
}

void Widget::query()
{
    int index = ui->comboBox->currentIndex();
    int result = 0;
    switch (index) {
    case 0:
        result = doQuery(0);
        break;
    case 1:
        result = doQuery(1);
        break;
    case 2:
        result = doQuery(0)+doQuery(1);
        break;
    default:
        break;
    }

    ui->remainingLabel->setText(QString::number(result));
    ui->counterLabel->setText(QString::number(++counter));
    if (result > 0)
        say("有号啦");
}

int Widget::doQuery(int post_meridiem)
{
    QString date = ui->dateEdit->date().toString("yyyy.MM.dd");
    char data[100];
    sprintf(data, "date=%s&post_meridiem=%d", date.toStdString().c_str(), post_meridiem);
    QJsonObject res = post(data);
    return res.value("remaining_number").toInt();
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
    ui->dateEdit->setEnabled(flag);
    ui->comboBox->setEnabled(flag);

    if (!flag) {
        ui->pushButton->setText("停止查询");
        ui->remainingLabel->setText("0");
        counter = 0;
        query();
        timer->start(10000);
    } else {
        ui->pushButton->setText("开始查询");
        timer->stop();
    }

    flag = !flag;
}

void Widget::say(const char *s)
{
    tts->say(s);
}
