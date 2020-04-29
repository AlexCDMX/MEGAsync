#include "AlertItem.h"
#include "ui_AlertItem.h"
#include <QDateTime>
#include "MegaApplication.h"

using namespace mega;

AlertItem::AlertItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlertItem)
{
    ui->setupUi(this);
    megaApi = ((MegaApplication *)qApp)->getMegaApi();

    ui->sIconWidget->hide();
    ui->wNotificationIcon->hide();
}

AlertItem::~AlertItem()
{
    delete ui;
}

void AlertItem::setAlertData(MegaUserAlert *alert)
{
    setAlertType(alert->getType());
    setAlertHeading(alert);
    setAlertContent(alert);
    setAlertTimeStamp(alert->getTimestamp(0));
    alert->getSeen() ? ui->lNew->hide() : ui->lNew->show();
}

void AlertItem::setAlertType(int type)
{
    ui->wNotificationIcon->hide();

    QString notificationTitle;
    QString notificationColor;
    switch (type)
    {
            case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_REQUEST:
            case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_CANCELLED:
            case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_REMINDER:
            case MegaUserAlert::TYPE_CONTACTCHANGE_DELETEDYOU:
            case MegaUserAlert::TYPE_CONTACTCHANGE_CONTACTESTABLISHED:
            case MegaUserAlert::TYPE_CONTACTCHANGE_ACCOUNTDELETED:
            case MegaUserAlert::TYPE_CONTACTCHANGE_BLOCKEDYOU:
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_IGNORED:
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_ACCEPTED:
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_DENIED:
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTOUTGOING_ACCEPTED:
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTOUTGOING_DENIED:
            {
                notificationTitle = tr("Contacts").toUpper();
                notificationColor = QString::fromUtf8("#1CB5A0");
                break;
            }
            case MegaUserAlert::TYPE_NEWSHARE:
            case MegaUserAlert::TYPE_DELETEDSHARE:
            case MegaUserAlert::TYPE_NEWSHAREDNODES:
            case MegaUserAlert::TYPE_REMOVEDSHAREDNODES:
            {
                if (type == MegaUserAlert::TYPE_DELETEDSHARE)
                {
                    ui->bSharedFolder->setIcon(QIcon(QString::fromUtf8(":/images/grey_folder.png")).pixmap(24.0, 24.0));
                }
                else
                {
                    ui->bSharedFolder->setIcon(QIcon(QString::fromUtf8(":/images/color_folder.png")).pixmap(24.0, 24.0));
                }

                ui->bNotificationIcon->setMinimumSize(QSize(10, 8));
                ui->bNotificationIcon->setMaximumSize(QSize(10, 8));
                ui->bNotificationIcon->setIconSize(QSize(10, 8));
                ui->bNotificationIcon->setIcon(QIcon(QString::fromAscii("://images/share_arrow.png")));
                ui->wNotificationIcon->show();
                notificationTitle = tr("Incoming Shares").toUpper();
                notificationColor = QString::fromUtf8("#F2C249");
                break;
            }
            case MegaUserAlert::TYPE_PAYMENT_SUCCEEDED:
            case MegaUserAlert::TYPE_PAYMENT_FAILED:
            case MegaUserAlert::TYPE_PAYMENTREMINDER:
            {
                notificationTitle = tr("Payment").toUpper();
                notificationColor = QString::fromUtf8("#FFA502");
                break;
            }
            case MegaUserAlert::TYPE_TAKEDOWN:
            case MegaUserAlert::TYPE_TAKEDOWN_REINSTATED:
            {
                notificationTitle = tr("Takedown notice").toUpper();
                notificationColor = QString::fromUtf8("#D64446");
                break;
            }
            default:
            {
                notificationTitle = QString::fromUtf8("");
                notificationColor = QString::fromUtf8("#FFFFFF");
                ui->bNotificationIcon->setMinimumSize(QSize(16, 16));
                ui->bNotificationIcon->setMaximumSize(QSize(16, 16));
                ui->bNotificationIcon->setIconSize(QSize(16, 16));
                ui->bNotificationIcon->setIcon(QIcon(QString::fromAscii("://images/mega_notifications.png")));
                ui->wNotificationIcon->show();
            }
                break;
    }

    ui->lTitle->setStyleSheet(QString::fromAscii("#lTitle { font-family: Lato; font-weight: 900; font-size: 10px; color: %1; } ").arg(notificationColor));
    ui->lTitle->setText(notificationTitle);
}

void AlertItem::setAlertHeading(MegaUserAlert *alert)
{
    ui->sIconWidget->hide();
    notificationHeading.clear();

    switch (alert->getType())
    {
        // Contact notifications
        case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_REQUEST:
        case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_CANCELLED:
        case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_REMINDER:
        {
            setAvatar(alert);
            notificationHeading = tr("New Contact Request");
            ui->sIconWidget->setCurrentWidget(ui->pContact);
            ui->sIconWidget->show();
            break;
        }
        case MegaUserAlert::TYPE_CONTACTCHANGE_DELETEDYOU:
        case MegaUserAlert::TYPE_CONTACTCHANGE_ACCOUNTDELETED:
        {
            setAvatar(alert);
            notificationHeading = tr("Contact Deleted");
            ui->sIconWidget->setCurrentWidget(ui->pContact);
            ui->sIconWidget->show();
            break;
        }
        case MegaUserAlert::TYPE_CONTACTCHANGE_CONTACTESTABLISHED:
        {
            setAvatar(alert);
            notificationHeading = tr("Contact Established");
            ui->sIconWidget->setCurrentWidget(ui->pContact);
            ui->sIconWidget->show();
            break;
        }
        case MegaUserAlert::TYPE_CONTACTCHANGE_BLOCKEDYOU:
        {
            setAvatar(alert);
            notificationHeading = tr("Contact Blocked");
            ui->sIconWidget->setCurrentWidget(ui->pContact);
            ui->sIconWidget->show();
            break;
        }
        case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_IGNORED:
        case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_ACCEPTED:
        case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_DENIED:
        {
            setAvatar(alert);
            notificationHeading = tr("Contact Updated");
            ui->sIconWidget->setCurrentWidget(ui->pContact);
            ui->sIconWidget->show();
            break;
        }
        case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTOUTGOING_ACCEPTED:
        {
            setAvatar(alert);
            notificationHeading = tr("Contact Accepted");
            ui->sIconWidget->setCurrentWidget(ui->pContact);
            ui->sIconWidget->show();
            break;
        }
        case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTOUTGOING_DENIED:
        {
            setAvatar(alert);
            notificationHeading = tr("Contact Denied");
            ui->sIconWidget->setCurrentWidget(ui->pContact);
            ui->sIconWidget->show();
            break;
        }
        // Share notifications
        case MegaUserAlert::TYPE_NEWSHARE:
        case MegaUserAlert::TYPE_DELETEDSHARE:
        case MegaUserAlert::TYPE_NEWSHAREDNODES:
        case MegaUserAlert::TYPE_REMOVEDSHAREDNODES:
        {
            ui->sIconWidget->setCurrentWidget(ui->pSharedFolder);
            ui->sIconWidget->show();
            MegaNode *node = megaApi->getNodeByHandle(alert->getNodeHandle());
            notificationHeading = QString::fromUtf8(node ? node->getName() : alert->getName());

            if (notificationHeading.isEmpty())
            {
                notificationHeading = tr("Shared Folder Activity");
            }

            if (node)
            {
                delete node;
            }
            break;
        }
        // Payment notifications
        case MegaUserAlert::TYPE_PAYMENT_SUCCEEDED:
        case MegaUserAlert::TYPE_PAYMENT_FAILED:
        case MegaUserAlert::TYPE_PAYMENTREMINDER:
            notificationHeading = tr("Payment Info");
            break;
        // Takedown notifications
        case MegaUserAlert::TYPE_TAKEDOWN:
        case MegaUserAlert::TYPE_TAKEDOWN_REINSTATED:
            notificationHeading = tr("Takedown Notice");
            break;

        default:
            notificationHeading = tr("Notification");
            break;
    }

    ui->lHeading->ensurePolished();
    ui->lHeading->setText(ui->lHeading->fontMetrics().elidedText(notificationHeading, Qt::ElideMiddle,ui->lHeading->minimumWidth()));
}

void AlertItem::setAlertContent(MegaUserAlert *alert)
{
    QString notificationContent;
    switch (alert->getType())
    {
            // Contact notifications
            case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_REQUEST:
                notificationContent = tr("[A] sent you a contact request")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_CANCELLED:
                notificationContent = tr("[A] cancelled their contact request")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_INCOMINGPENDINGCONTACT_REMINDER:
                notificationContent = tr("Reminder") + QString::fromUtf8(": ") + tr("You have a contact request");
                break;
            case MegaUserAlert::TYPE_CONTACTCHANGE_DELETEDYOU:
                notificationContent = tr("[A] deleted you as a contact")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_CONTACTCHANGE_ACCOUNTDELETED:
                notificationContent = tr("[A] has been deleted/deactivated")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_CONTACTCHANGE_CONTACTESTABLISHED:
                notificationContent = tr("[A] established you as a contact")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_CONTACTCHANGE_BLOCKEDYOU:
                notificationContent = tr("[A] blocked you as contact")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_IGNORED:
                notificationContent = tr("You ignored a contact request");
                break;
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_ACCEPTED:
                notificationContent = tr("You accepted a contact request");
                break;
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTINCOMING_DENIED:
                notificationContent = tr("You denied a contact request");
                break;
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTOUTGOING_ACCEPTED:
                notificationContent = tr("[A] accepted your contact request")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_UPDATEDPENDINGCONTACTOUTGOING_DENIED:
                notificationContent = tr("[A] denied your contact request")
                        .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            // Share notifications
            case MegaUserAlert::TYPE_NEWSHARE:
                notificationContent = tr("New Shared folder from [X]")
                        .replace(QString::fromUtf8("[X]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                break;
            case MegaUserAlert::TYPE_DELETEDSHARE:
            {
                if (alert->getNumber(0) == 0) //Someone left the folder
                {
                    notificationContent = tr("[A] has left the shared folder")
                            .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                }
                else //Access for the user was removed by share owner
                {
                    notificationContent = alert->getEmail() ? tr("Access to shared folder was removed by [A]").replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())))
                                                            : tr("Access to shared folder was removed");
                }
                break;
            }

            case MegaUserAlert::TYPE_NEWSHAREDNODES:
            {
                int64_t updatedItems = alert->getNumber(1) + alert->getNumber(0);
                if (updatedItems == 1)
                {
                    notificationContent = tr("[A] added 1 item")
                            .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                }
                else
                {
                    notificationContent = tr("[A] added [B] items")
                            .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())))
                            .replace(QString::fromUtf8("[B]"), QString::number(updatedItems));
                }
                break;
            }
            case MegaUserAlert::TYPE_REMOVEDSHAREDNODES:
            {
                int64_t updatedItems = alert->getNumber(0);
                if (updatedItems == 1)
                {
                    notificationContent = tr("[A] removed 1 item")
                            .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())));
                }
                else
                {
                    notificationContent = tr("[A] removed [B] items")
                            .replace(QString::fromUtf8("[A]"), formatRichString(QString::fromUtf8(alert->getEmail())))
                            .replace(QString::fromUtf8("[B]"), QString::number(updatedItems));
                }
                break;
            }
            // Payment notifications
            case MegaUserAlert::TYPE_PAYMENT_SUCCEEDED:
                notificationContent = tr("Your payment for the [A] plan was received")
                        .replace(QString::fromUtf8("[A]"), alert->getString(0) ? QString::fromUtf8(alert->getString(0)) : QString::fromUtf8(""));
                break;
            case MegaUserAlert::TYPE_PAYMENT_FAILED:
                notificationContent = tr("Your payment for the [A] plan was unsuccessful")
                        .replace(QString::fromUtf8("[A]"), alert->getString(0) ? QString::fromUtf8(alert->getString(0)) : QString::fromUtf8(""));
                break;
            case MegaUserAlert::TYPE_PAYMENTREMINDER:
            {
                QDateTime expiredDate;
                expiredDate.setMSecsSinceEpoch(alert->getTimestamp(1) * 1000);
                QDateTime currentDate(QDateTime::currentDateTime());

                int daysExpired = currentDate.daysTo(expiredDate);
                if (daysExpired == 1)
                {
                    notificationContent = tr("Your PRO membership plan will expire in 1 day");
                }
                else if (daysExpired > 0)
                {
                    notificationContent = tr("Your PRO membership plan will expire in [A] days")
                            .replace(QString::fromUtf8("[A]"), formatRichString(QString::number(daysExpired)));
                }
                else if (daysExpired == 0)
                {
                    notificationContent = tr("PRO membership plan expiring soon");
                }
                else if (daysExpired == -1)
                {
                    notificationContent = tr("Your PRO membership plan expired 1 day ago");
                }
                else
                {
                    notificationContent = tr("Your PRO membership plan expired [A] days ago")
                            .replace(QString::fromUtf8("[A]"), formatRichString(QString::number(-daysExpired)));
                }
                break;
            }
            // Takedown notifications
            case MegaUserAlert::TYPE_TAKEDOWN:
            {
                MegaNode *node = megaApi->getNodeByHandle(alert->getNodeHandle());
                if (node)
                {
                    notificationContent = tr("Your publicly shared [%1] ([%2]) has been taken down")
                            .replace(QString::fromUtf8("[%1]"), node->getType() == MegaNode::TYPE_FILE ? tr("file")
                                       : node->getType() == MegaNode::TYPE_FOLDER ? tr("folder")
                                       : QString::fromUtf8(""))
                            .replace(QString::fromUtf8("[%2]"), formatRichString(QString::fromUtf8(node->getName())));
                      delete node;
                }
                else
                {
                    notificationContent = tr("Your publicly shared has been taken down");
                }
                break;
            }
            case MegaUserAlert::TYPE_TAKEDOWN_REINSTATED:
            {
                MegaNode *node = megaApi->getNodeByHandle(alert->getNodeHandle());
                if (node)
                {
                    notificationContent = tr("Your publicly shared [A] ([B]) has been reinstated")
                            .replace(QString::fromUtf8("[A]"), node->getType() == MegaNode::TYPE_FILE ? tr("file")
                                       : node->getType() == MegaNode::TYPE_FOLDER ? tr("folder")
                                       : QString::fromUtf8(""))
                            .replace(QString::fromUtf8("[B]"), formatRichString(QString::fromUtf8(node->getName())));
                    delete node;
                }
                else
                {
                    notificationContent = tr("Your taken down has been reinstated");
                }
                break;
            }
            default:
                notificationContent = QString::fromUtf8(alert->getTitle());
                break;
    }

    ui->lDesc->setText(notificationContent);
}

void AlertItem::setAlertTimeStamp(int64_t ts)
{
    if (ts != -1)
    {
        QDateTime date;
        date.setMSecsSinceEpoch(ts * 1000);
        ui->lTimeStamp->setText(date.toString(QString::fromUtf8("h:mm ap, d MMMM yyyy")));

    }
    else
    {
        ui->lTimeStamp->setText(QString::fromUtf8(""));
    }

}

QString AlertItem::getHeadingString()
{
    return notificationHeading;
}

QSize AlertItem::minimumSizeHint() const
{
    return QSize(400, 122);
}
QSize AlertItem::sizeHint() const
{
    return QSize(400, 122);
}

void AlertItem::setAvatar(MegaUserAlert *alert)
{
    QString color;
    const char* avatarColor = megaApi->getUserAvatarColor(megaApi->handleToBase64(qHash(QString::fromUtf8(alert->getEmail()))));

    if (avatarColor)
    {
        color = QString::fromUtf8(avatarColor);
        delete [] avatarColor;
    }

    QString fullname = QString::fromUtf8(alert->getEmail());
    if (fullname.isEmpty())
    {
        fullname = QString::fromUtf8("C");
    }

    ui->wAvatarContact->setAvatarLetter(fullname.at(0).toUpper(), color);
}

QString AlertItem::formatRichString(QString str)
{
    return QString::fromUtf8("<span style='color:#333333; font-family: Lato; font-size: 14px; font-weight: bold; text-decoration:none;'><bold>%1</bold></span>")
            .arg(str);
}
