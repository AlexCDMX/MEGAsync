#include "MenuItemAction.h"
#include <QKeyEvent>
#include <QStyle>

const QString MenuItemAction::Colors::Normal = QLatin1String("#777777");
const QString MenuItemAction::Colors::Highlight = QLatin1String("#000000");
const QString MenuItemAction::Colors::Accent = QLatin1String("#F46265");

static constexpr int ENTRY_MAX_WIDTH_PX = 240;

MenuItemAction::MenuItemAction(const QString& title, const QString& value,
                               const QIcon& icon, const QIcon& hoverIcon, bool manageHoverStates,
                               int treeDepth, const QSize& iconSize, QObject* parent)
    : QWidgetAction (parent),
      mAccent(false),
      mContainer (new QWidget()),
      mIcon (icon),
      mHoverIcon (hoverIcon),
      mTitle (new QLabel(mContainer)),
      mValue (value.isNull() ? nullptr : new QLabel(value, mContainer)),
      mTreeDepth (treeDepth),
      mIconButton (new QPushButton(mContainer))
{
    setLabelText(title);
    mContainer->setObjectName(QLatin1String("wContainer"));
    mContainer->installEventFilter(this);

    if (manageHoverStates)
    {
        mContainer->setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    setupActionWidget(iconSize);
    setDefaultWidget(mContainer);
}

MenuItemAction::MenuItemAction(const QString& title, const QIcon& icon,
                               bool manageHoverStates, int treeDepth, const QSize& iconSize, QObject* parent)
    : MenuItemAction (title, QString(), icon, QIcon(), manageHoverStates, treeDepth, iconSize, parent)
{
}

MenuItemAction::MenuItemAction(const QString& title, const QString& value, const QIcon& icon,
                               bool manageHoverStates, int treeDepth, const QSize& iconSize, QObject* parent)
    : MenuItemAction (title, value, icon,  QIcon(), manageHoverStates, treeDepth, iconSize, parent)
{
}

MenuItemAction::MenuItemAction(const QString& title, const QIcon& icon, const QIcon& hoverIcon,
                               bool manageHoverStates, int treeDepth, const QSize& iconSize, QObject* parent)
    : MenuItemAction (title, QString(), icon, hoverIcon, manageHoverStates, treeDepth, iconSize, parent)
{
}

void MenuItemAction::setLabelText(const QString& title)
{
    // Force polish to update font Info with .ui StyleSheet
    mTitle->ensurePolished();
    auto f (mTitle->fontMetrics());
    QString elidedTitle (f.elidedText(title, Qt::ElideMiddle, ENTRY_MAX_WIDTH_PX));
    mTitle->setText(elidedTitle);
    if (title != elidedTitle)
    {
        setToolTip(title);
    }
}

void MenuItemAction::setIcon(const QIcon& icon)
{
    mIcon = icon;
    mIconButton->setIcon(mIcon);
}

void MenuItemAction::setHoverIcon(const QIcon& icon)
{
    mHoverIcon = icon;
}

void MenuItemAction::setHighlight(bool highlight)
{   
    if (highlight)
    {
        mTitle->setStyleSheet(QString::fromLatin1("color: %1;").arg(Colors::Highlight));
    }
    else
    {
        mTitle->setStyleSheet(QString::fromLatin1("color: %1;").arg(getColor()));
    }
}

MenuItemAction::~MenuItemAction()
{
    QLayout* layout (mContainer->layout());
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }
    mContainer->deleteLater();
}

void MenuItemAction::setupActionWidget(const QSize& iconSize)
{
    mContainer->setMinimumHeight(32);
    mContainer->setMaximumHeight(32);
    mContainer->setStyleSheet(QLatin1String("#wContainer { margin-left: 20px; padding: 0px; }"
                                            "QLabel {font-family: Lato; font-size: 14px;}"));

    mIconButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    mIconButton->setText(QString());
    mIconButton->setMinimumSize(iconSize);
    mIconButton->setMaximumSize(iconSize);
    mIconButton->setIconSize(iconSize);
    mIconButton->setIcon(mIcon);
    mIconButton->setFlat(true);

    mTitle->setStyleSheet(QString::fromLatin1("color: %1;").arg(getColor()));

    auto layout = new QHBoxLayout();
    layout->setContentsMargins(QMargins(16 + mTreeDepth * 33, 0, 8, 0));
    layout->setSpacing(12);
    layout->addWidget(mIconButton);
    layout->addWidget(mTitle);
    layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));

    if (mValue)
    {
        mValue->setStyleSheet(QString::fromLatin1("padding-right: 6px; color: %1;").arg(getColor()));
        layout->addWidget(mValue);
    }
    mContainer->setLayout(layout);
}

bool MenuItemAction::eventFilter(QObject *obj, QEvent *event)
{
    if (!mValue)
    {
        if (event->type() == QEvent::Enter)
        {
            mTitle->setStyleSheet(QString::fromLatin1("color: %1;").arg(Colors::Highlight));
        }

        if (event->type() == QEvent::Leave)
        {
            mTitle->setStyleSheet(QString::fromLatin1("color: %1;").arg(getColor()));
        }
    }

    return QWidgetAction::eventFilter(obj,event);
}

bool MenuItemAction::getAccent() const
{
    return mAccent;
}

void MenuItemAction::setAccent(bool enabled)
{
    mAccent = enabled;
    if(mTitle)
    {
        mTitle->setStyleSheet(QString::fromLatin1("color: %1;").arg(getColor()));
    }
}

const QString& MenuItemAction::getColor() const
{
    if(mAccent)
        return Colors::Accent;
    else
        return Colors::Normal;
}

