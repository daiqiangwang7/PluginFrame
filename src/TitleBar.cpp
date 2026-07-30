#include "TitleBar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyle>

namespace {

enum class WindowControlIcon
{
    Minimize,
    Maximize,
    Close
};

class WindowControlButton : public QPushButton
{
public:
    explicit WindowControlButton(WindowControlIcon icon, QWidget *parent = nullptr)
        : QPushButton(parent)
        , m_icon(icon)
    {
        setCursor(Qt::ArrowCursor);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPushButton::paintEvent(event);

        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QPen pen(palette().color(QPalette::ButtonText));
        pen.setWidth(2);
        pen.setCapStyle(Qt::SquareCap);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        const QRect iconRect = QRect(0, 0, 16, 16).translated(rect().center() - QPoint(8, 8));
        if (m_icon == WindowControlIcon::Minimize) {
            painter.drawLine(iconRect.left() + 2, iconRect.center().y() + 3,
                             iconRect.right() - 2, iconRect.center().y() + 3);
            return;
        }

        if (m_icon == WindowControlIcon::Close) {
            painter.drawLine(iconRect.left() + 3, iconRect.top() + 3,
                             iconRect.right() - 3, iconRect.bottom() - 3);
            painter.drawLine(iconRect.right() - 3, iconRect.top() + 3,
                             iconRect.left() + 3, iconRect.bottom() - 3);
            return;
        }

        const bool maximized = property("maximized").toBool();
        if (maximized) {
            painter.drawRect(iconRect.adjusted(3, 5, -5, -3));
            painter.drawRect(iconRect.adjusted(6, 2, -2, -6));
        } else {
            painter.drawRect(iconRect.adjusted(3, 3, -3, -3));
        }
    }

private:
    WindowControlIcon m_icon;
};

} // namespace

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
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
    setFixedHeight(42);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 0, 6, 0);
    layout->setSpacing(6);

    m_titleLabel = new QLabel(QStringLiteral("插件框架"), this);
    m_titleLabel->setObjectName(QStringLiteral("WindowTitle"));

    m_themeButton = new QPushButton(QStringLiteral("皮肤"), this);
    m_themeButton->setObjectName(QStringLiteral("ThemeButton"));

    m_minimizeButton = new WindowControlButton(WindowControlIcon::Minimize, this);
    m_minimizeButton->setObjectName(QStringLiteral("MinimizeButton"));
    m_minimizeButton->setToolTip(QStringLiteral("最小化"));

    m_maximizeButton = new WindowControlButton(WindowControlIcon::Maximize, this);
    m_maximizeButton->setObjectName(QStringLiteral("MaximizeButton"));
    m_maximizeButton->setToolTip(QStringLiteral("最大化"));
    updateMaximizeButtonState(false);

    m_closeButton = new WindowControlButton(WindowControlIcon::Close, this);
    m_closeButton->setObjectName(QStringLiteral("CloseButton"));
    m_closeButton->setToolTip(QStringLiteral("关闭"));

    const QSize iconSize(16, 16);
    m_themeButton->setIconSize(iconSize);
    m_minimizeButton->setIconSize(iconSize);
    m_maximizeButton->setIconSize(iconSize);
    m_closeButton->setIconSize(iconSize);

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

    m_maximizeButton->setProperty("maximized", maximized);
    m_maximizeButton->setToolTip(maximized ? QStringLiteral("还原") : QStringLiteral("最大化"));
    m_maximizeButton->style()->unpolish(m_maximizeButton);
    m_maximizeButton->style()->polish(m_maximizeButton);
    m_maximizeButton->update();
}
