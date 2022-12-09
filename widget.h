#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtTextToSpeech/QTextToSpeech>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void query();

private:
    QJsonObject post(const QString &data);
    int doQuery(int post_meridiem);
    void say(const char *s);

private:
    Ui::Widget *ui;
    QTimer *timer;
    QTextToSpeech *tts;
    int counter{0};
    bool flag{false};
};
#endif // WIDGET_H
