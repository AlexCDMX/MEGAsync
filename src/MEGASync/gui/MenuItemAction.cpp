#include "MenuItemAction.h"
#include <QKeyEvent>

MenuItemAction::MenuItemAction(const QString title, const QIcon icon, bool manageHoverStates, QSize iconSize)
    : QWidgetAction(NULL)
{
    this->title = new QLabel(title);
    this->icon = new QIcon(icon);
    this->hoverIcon = NULL;
    this->value = NULL;
    container = new QWidget(NULL);
    container->setObjectName(QString::fromUtf8("wContainer"));
    container->installEventFilter(this);

    if (manageHoverStates)
    {
        container->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    setupActionWidget(iconSize);
    setDefaultWidget(container);
}

MenuItemAction::MenuItemAction(const QString title, const QString value, const QIcon icon, bool manageHoverStates, QSize iconSize)
    : QWidgetAction(NULL)
{
    this->title = new QLabel(title);
    this->value = new QLabel(value);
    this->icon = new QIcon(icon);
    this->hoverIcon = NULL;
    container = new QWidget(NULL);
    container->setObjectName(QString::fromUtf8("wContainer"));
    container->installEventFilter(this);

    if (manageHoverStates)
    {
        container->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    setupActionWidget(iconSize);
    setDefaultWidget(container);
}

MenuItemAction::MenuItemAction(const QString title, const QIcon icon, const QIcon hoverIcon, bool manageHoverStates, QSize iconSize)
    : QWidgetAction(NULL)
{
    this->title = new QLabel(title);
    this->icon = new QIcon(icon);
    this->hoverIcon = new QIcon(hoverIcon);
    this->value = NULL;
    container = new QWidget (NULL);
    container->setObjectName(QString::fromUtf8("wContainer"));
    container->installEventFilter(this);

    if (manageHoverStates)
    {
        container->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    setupActionWidget(iconSize);
    setDefaultWidget(container);
}

void MenuItemAction::setLabelText(QString title)
{
    this->title->setText(title);
}

void MenuItemAction::setIcon(const QIcon icon)
{
    delete this->icon;
    this->icon = new QIcon(icon);
    iconButton->setIcon(*(this->icon));
}

void MenuItemAction::setHoverIcon(const QIcon icon)
{
    delete this->hoverIcon;
    this->hoverIcon = new QIcon(icon);
}

void MenuItemAction::setHighlight(bool highlight)
{
    if (highlight)
    {
        title->setStyleSheet(QString::fromAscii("font-family: Lato; font-size: 14px; color: #000000;"));
    }
    else
    {
        title->setStyleSheet(QString::fromAscii("font-family: Lato; font-size: 14px; color: #777777;"));
    }
}

MenuItemAction::~MenuItemAction()
{
    delete title;
    delete value;
    delete iconButton;
    delete icon;
    delete hoverIcon;
    delete layout;
    delete container;
}

void MenuItemAction::setupActionWidget(QSize iconSize)
{
    container->setMinimumHeight(32);
    container->setMaximumHeight(32);
    container->setStyleSheet(QString::fromAscii("#wContainer { margin-left: 20px; padding: 0px; }"));

    iconButton = new QPushButton();
    iconButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    iconButton->setText(QString::fromUtf8(""));
    iconButton->setMinimumSize(iconSize);
    iconButton->setMaximumSize(iconSize);
    const QList<QSize> sizes = icon->availableSizes();
    if (!sizes.empty())
    {
        iconButton->setIconSize(sizes.at(0));
    }
    iconButton->setIcon(*icon);
    iconButton->setFlat(true);

    title->setStyleSheet(QString::fromAscii("font-family: Lato; font-size: 14px; color: #777777;"));

    layout = new QHBoxLayout();
    layout->setContentsMargins(QMargins(16, 0, 8, 0));
    layout->setSpacing(12);
    layout->addWidget(iconButton);
    layout->addWidget(title);
    layout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Expanding));

    if (value)
    {
        value->setStyleSheet(QString::fromAscii("font-family: Lato; font-size: 14px; color: #777777; padding-right: 6px;"));
        layout->addWidget(value);
    }
    container->setLayout(layout);
}

bool MenuItemAction::eventFilter(QObject *obj, QEvent *event)
{
    if (!value)
    {
        if (event->type() == QEvent::Enter)
        {
            title->setStyleSheet(QString::fromAscii("font-family: Lato; font-size: 14px; color: #000000;"));
        }

        if (event->type() == QEvent::Leave)
        {
            title->setStyleSheet(QString::fromAscii("font-family: Lato; font-size: 14px; color: #777777;"));
        }
    }

    return QWidgetAction::eventFilter(obj,event);
}
