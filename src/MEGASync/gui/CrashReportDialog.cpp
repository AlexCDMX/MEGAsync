#include "CrashReportDialog.h"
#include "ui_CrashReportDialog.h"

CrashReportDialog::CrashReportDialog(QString crash, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashReportDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->tCrash->setText(crash);
    ui->bOK->setDefault(true);
    highDpiResize.init(this);
}

QString CrashReportDialog::getUserMessage()
{
    return ui->tUserMessage->toPlainText();
}

CrashReportDialog::~CrashReportDialog()
{
    delete ui;
}
