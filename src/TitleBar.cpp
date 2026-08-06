#include "TitleBar.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QStyle>

namespace {

QString iconThemeFolder(const QString &themeName)
{
    return themeName == QStringLiteral("cyber-light") ? QStringLiteral("light") : QStringLiteral("dark");
}

QString windowIconPath(const QString &themeName, const QString &iconName)
{
    return QStringLiteral(":/icons/window/%1/%2.png").arg(iconThemeFolder(themeName), iconName);
}

QString themeIconPath(const QString &themeName)
{
    return QStringLiteral(":/icons/theme/%1/skin.png").arg(iconThemeFolder(themeName));
}

} // namespace

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    Q_INIT_RESOURCE(resources);
    setupUi();
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void TitleBar::setThemeName(const QString &themeName)
{
    if (!m_themeButton) {
        return;
    }

    m_themeName = themeName;
    m_themeButton->setIcon(QIcon(themeIconPath(m_themeName)));
    if (m_minimizeButton) {
        m_minimizeButton->setIcon(QIcon(windowIconPath(m_themeName, QStringLiteral("minimize"))));
    }
    if (m_closeButton) {
        m_closeButton->setIcon(QIcon(windowIconPath(m_themeName, QStringLiteral("close"))));
    }
    updateMaximizeButtonState(m_maximizeButton ? m_maximizeButton->property("windowMaximized").toBool() : false);
    m_themeButton->update();
    emit themeIconChanged();
}

void TitleBar::setThemeButtonVisible(bool visible)
{
    if (m_themeButton) {
        m_themeButton->setVisible(visible);
    }
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && window()) {
        m_dragPosition = event->globalPos() - window()->frameGeometry().topLeft();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && window() && !window()->isMaximized()) {
        window()->move(event->globalPos() - m_dragPosition);
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        toggleMaximized();
        event->accept();
        return;
    }

    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::setupUi()
{
    setObjectName(QStringLiteral("TitleBar"));
    setFixedHeight(36);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 4, 0);
    layout->setSpacing(4);

    m_titleLabel = new QLabel(QStringLiteral("插件框架"), this);
    m_titleLabel->setObjectName(QStringLiteral("WindowTitle"));

    m_themeButton = new QPushButton(this);
    m_themeButton->setObjectName(QStringLiteral("ThemeButton"));
    m_themeButton->setToolTip(QStringLiteral("切换皮肤"));
    m_themeButton->setFixedSize(32, 28);

    m_minimizeButton = new QPushButton(this);
    m_minimizeButton->setObjectName(QStringLiteral("MinimizeButton"));
    m_minimizeButton->setToolTip(QStringLiteral("最小化"));
    m_minimizeButton->setFixedSize(36, 28);

    m_maximizeButton = new QPushButton(this);
    m_maximizeButton->setObjectName(QStringLiteral("MaximizeButton"));
    m_maximizeButton->setToolTip(QStringLiteral("最大化"));
    m_maximizeButton->setFixedSize(36, 28);
    updateMaximizeButtonState(false);

    m_closeButton = new QPushButton(this);
    m_closeButton->setObjectName(QStringLiteral("CloseButton"));
    m_closeButton->setToolTip(QStringLiteral("关闭"));
    m_closeButton->setFixedSize(36, 28);

    const QSize iconSize(16, 16);
    m_themeButton->setIconSize(iconSize);
    m_minimizeButton->setIconSize(iconSize);
    m_maximizeButton->setIconSize(iconSize);
    m_closeButton->setIconSize(iconSize);
    setThemeName(QStringLiteral("cyber-dark"));

    layout->addWidget(m_titleLabel);
    layout->addStretch();
    layout->addWidget(m_themeButton);
    layout->addWidget(m_minimizeButton);
    layout->addWidget(m_maximizeButton);
    layout->addWidget(m_closeButton);

    connect(m_themeButton, &QPushButton::clicked, this, &TitleBar::themeToggleRequested);
    connect(m_minimizeButton, &QPushButton::clicked, this, [this]() {
        if (window()) {
            window()->showMinimized();
        }
    });
    connect(m_maximizeButton, &QPushButton::clicked, this, &TitleBar::toggleMaximized);
    connect(m_closeButton, &QPushButton::clicked, this, [this]() {
        if (window()) {
            window()->close();
        }
    });
}

void TitleBar::toggleMaximized()
{
    if (!window()) {
        return;
    }

    if (window()->isMaximized()) {
        window()->showNormal();
        updateMaximizeButtonState(false);
    } else {
        window()->showMaximized();
        updateMaximizeButtonState(true);
    }
}

void TitleBar::updateMaximizeButtonState(bool maximized)
{
    if (!m_maximizeButton) {
        return;
    }

    m_maximizeButton->setProperty("windowMaximized", maximized);
    m_maximizeButton->setIcon(QIcon(windowIconPath(m_themeName, maximized
                                                   ? QStringLiteral("restore")
                                                   : QStringLiteral("maximize"))));
    m_maximizeButton->setToolTip(maximized ? QStringLiteral("还原") : QStringLiteral("最大化"));
    m_maximizeButton->style()->unpolish(m_maximizeButton);
    m_maximizeButton->style()->polish(m_maximizeButton);
    m_maximizeButton->update();
}
