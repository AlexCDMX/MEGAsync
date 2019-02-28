#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QFrame>
#include <QtCore/QRect>
#include <QtGui/QResizeEvent>
#include <QtCore/QString>
#include <QWidget>

class ElidedLabel : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool isElided READ isElided)

public:
    explicit ElidedLabel(QWidget *parent = 0);

    void setText(const QString &text);
    const QString & text() const { return content; }
    bool isElided() const { return elided; }

protected:

    void paintEvent(QPaintEvent *event);

signals:
    void elisionChanged(bool elided);

private:
    bool elided;
    QString content;
};

#endif // ELIDEDLABEL_H
