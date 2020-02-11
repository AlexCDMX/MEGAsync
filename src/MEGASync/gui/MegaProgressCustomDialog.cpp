#include "MegaProgressCustomDialog.h"
#include "ui_MegaProgressCustomDialog.h"
#include <QCloseEvent>
#include <qdebug.h>

MegaProgressCustomDialog::MegaProgressCustomDialog(QWidget *parent, int minimum, int maximum):
    QDialog(parent),
    ui(new Ui::MegaProgressCustomDialog)
{
    ui->setupUi(this);

    ui->progressBar->setMinimum(minimum);
    ui->progressBar->setMaximum(maximum);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    highDpiResize.init(this);
}

MegaProgressCustomDialog::~MegaProgressCustomDialog() {
    delete ui;
}

void MegaProgressCustomDialog::reject()
{

}

void MegaProgressCustomDialog::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

void MegaProgressCustomDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}
