#ifndef LOCALANDREMOTEDIFFERENTWIDGET_H
#define LOCALANDREMOTEDIFFERENTWIDGET_H

#include "StalledIssueBaseDelegateWidget.h"
#include "StalledIssueChooseWidget.h"


#include <QWidget>
#include <memory>

namespace Ui {
class LocalAndRemoteDifferentWidget;
}

class LocalAndRemoteDifferentWidget : public StalledIssueBaseDelegateWidget
{
    Q_OBJECT

public:
    explicit LocalAndRemoteDifferentWidget(std::shared_ptr<mega::MegaSyncStall> originalStall, QWidget *parent = nullptr);
    ~LocalAndRemoteDifferentWidget();

    void refreshUi() override;

    std::shared_ptr<mega::MegaSyncStall> originalStall;

private slots:
    void onLocalButtonClicked(int);
    void onRemoteButtonClicked(int);
    void onKeepBothButtonClicked(int);
    void onKeepLastModifiedTimeButtonClicked(int);

private:
    Ui::LocalAndRemoteDifferentWidget *ui;
};

#endif // LOCALANDREMOTEDIFFERENTWIDGET_H
