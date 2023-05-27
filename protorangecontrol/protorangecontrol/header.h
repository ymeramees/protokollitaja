#ifndef HEADER_H
#define HEADER_H

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSet>

class Header : public QWidget
{
    Q_OBJECT
public:
    explicit Header(QWidget *parent = nullptr);

signals:

};

#endif // HEADER_H
