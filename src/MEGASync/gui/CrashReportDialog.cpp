#include "CrashReportDialog.h"
#include "ui_CrashReportDialog.h"

CrashReportDialog::CrashReportDialog(QString crash, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashReportDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->cLogs->setText(ui->cLogs->text().replace(QString::fromUtf8("[A]"), QString::fromUtf8("\n")));
    ui->tCrash->setText(crash);
    ui->bOK->setDefault(true);
    highDpiResize.init(this);
}

QString CrashReportDialog::getUserMessage()
{
    return ui->tUserMessage->toPlainText();
}

bool CrashReportDialog::sendLogs()
{
    return ui->cLogs->isChecked();
}

CrashReportDialog::~CrashReportDialog()
{
    delete ui;
}
