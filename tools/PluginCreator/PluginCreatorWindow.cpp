#include "PluginCreatorWindow.h"
#include "PluginProjectGenerator.h"
#include "PluginProjectValidator.h"
#include "ui_PluginCreatorWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>

namespace {

constexpr int TypePage = 0;
constexpr int InfoPage = 1;
constexpr int ViewOptionsPage = 2;
constexpr int CommonOptionsPage = 3;
constexpr int SummaryPage = 4;
constexpr int ResultPage = 5;

QString defaultPluginId(const QString &className)
{
    QString result = className;
    result.replace(QRegExp(QStringLiteral("([a-z0-9])([A-Z])")), QStringLiteral("\\1-\\2"));
    result = result.toLower();
    result.replace(QLatin1Char('_'), QLatin1Char('-'));
    return QStringLiteral("com.pluginframe.%1").arg(result);
}

QString areaDisplayName(CreatorWindowArea area)
{
    switch (area) {
    case CreatorWindowArea::LeftDock:
        return QStringLiteral("左侧区");
    case CreatorWindowArea::RightDock:
        return QStringLiteral("右侧区");
    case CreatorWindowArea::BottomDock:
        return QStringLiteral("底部区");
    case CreatorWindowArea::Floating:
        return QStringLiteral("浮动窗口");
    case CreatorWindowArea::Central:
    default:
        return QStringLiteral("中心区");
    }
}

} // namespace

PluginCreatorWindow::PluginCreatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PluginCreatorWindow)
{
    ui->setupUi(this);
    setupDefaults();
}

PluginCreatorWindow::~PluginCreatorWindow()
{
    delete ui;
}

void PluginCreatorWindow::previousPage()
{
    const int currentIndex = ui->pageStack->currentIndex();
    if (currentIndex <= TypePage) {
        return;
    }

    if (!isViewPluginSelected() && currentIndex == CommonOptionsPage) {
        ui->pageStack->setCurrentIndex(InfoPage);
    } else {
        ui->pageStack->setCurrentIndex(currentIndex - 1);
    }

    updateButtons();
}

void PluginCreatorWindow::nextPage()
{
    const int currentIndex = ui->pageStack->currentIndex();

    if (currentIndex == InfoPage) {
        const PluginProjectOptions options = collectOptions();
        QString errorString;
        if (!PluginProjectValidator::validate(options, &errorString)) {
            QMessageBox::warning(this, QStringLiteral("参数错误"), errorString);
            return;
        }
    }

    if (currentIndex == CommonOptionsPage) {
        refreshSummary();
    }

    if (!isViewPluginSelected() && currentIndex == InfoPage) {
        ui->pageStack->setCurrentIndex(CommonOptionsPage);
    } else if (currentIndex < ResultPage) {
        ui->pageStack->setCurrentIndex(currentIndex + 1);
    }

    updateButtons();
}

void PluginCreatorWindow::generatePlugin()
{
    const PluginProjectOptions options = collectOptions();
    PluginProjectGenerator generator;
    const PluginGenerationResult result = generator.generate(options);

    QString resultText = result.message;
    if (!result.generatedFiles.isEmpty()) {
        resultText.append(QStringLiteral("\n\n已生成文件：\n"));
        for (const QString &filePath : result.generatedFiles) {
            resultText.append(QStringLiteral("- %1\n").arg(QDir::toNativeSeparators(filePath)));
        }
    }

    if (result.success) {
        resultText.append(QStringLiteral("\n请在根目录 CMakeLists.txt 中添加：\n"));
        resultText.append(QStringLiteral("add_subdirectory(plugins/%1)\n").arg(options.className.trimmed()));
        ui->pageStack->setCurrentIndex(ResultPage);
        ui->resultTextEdit->setPlainText(resultText);
    } else {
        QMessageBox::warning(this, QStringLiteral("生成失败"), result.message);
        ui->resultTextEdit->setPlainText(resultText);
    }

    updateButtons();
}

void PluginCreatorWindow::chooseOutputDirectory()
{
    const QString selectedDirectory = QFileDialog::getExistingDirectory(
                this,
                QStringLiteral("选择输出目录"),
                ui->outputDirectoryEdit->text());

    if (!selectedDirectory.isEmpty()) {
        ui->outputDirectoryEdit->setText(QDir::toNativeSeparators(selectedDirectory));
    }
}

void PluginCreatorWindow::updatePluginTypeUi()
{
    updateButtons();
}

void PluginCreatorWindow::setupDefaults()
{
    setWindowTitle(QStringLiteral("插件创建向导"));
    resize(760, 520);

    ui->viewPluginRadio->setChecked(true);
    ui->versionEdit->setText(QStringLiteral("1.0.0"));
    ui->classNameEdit->setText(QStringLiteral("MyViewPlugin"));
    ui->pluginIdEdit->setText(QStringLiteral("com.pluginframe.my-view-plugin"));
    ui->displayNameEdit->setText(QStringLiteral("我的视图插件"));
    ui->outputDirectoryEdit->setText(QDir::toNativeSeparators(QDir::current().absoluteFilePath(QStringLiteral("plugins/MyViewPlugin"))));

    ui->areaComboBox->addItem(QStringLiteral("中心区"), static_cast<int>(CreatorWindowArea::Central));
    ui->areaComboBox->addItem(QStringLiteral("左侧区"), static_cast<int>(CreatorWindowArea::LeftDock));
    ui->areaComboBox->addItem(QStringLiteral("右侧区"), static_cast<int>(CreatorWindowArea::RightDock));
    ui->areaComboBox->addItem(QStringLiteral("底部区"), static_cast<int>(CreatorWindowArea::BottomDock));
    ui->areaComboBox->addItem(QStringLiteral("浮动窗口"), static_cast<int>(CreatorWindowArea::Floating));

    ui->registerCapabilityCheck->setChecked(true);
    ui->messageBusCheck->setChecked(true);
    ui->logServiceCheck->setChecked(true);
    ui->pluginSettingsCheck->setChecked(true);

    connect(ui->previousButton, &QPushButton::clicked, this, &PluginCreatorWindow::previousPage);
    connect(ui->nextButton, &QPushButton::clicked, this, &PluginCreatorWindow::nextPage);
    connect(ui->generateButton, &QPushButton::clicked, this, &PluginCreatorWindow::generatePlugin);
    connect(ui->closeButton, &QPushButton::clicked, this, &PluginCreatorWindow::close);
    connect(ui->chooseDirectoryButton, &QPushButton::clicked, this, &PluginCreatorWindow::chooseOutputDirectory);
    connect(ui->viewPluginRadio, &QRadioButton::toggled, this, &PluginCreatorWindow::updatePluginTypeUi);
    connect(ui->servicePluginRadio, &QRadioButton::toggled, this, &PluginCreatorWindow::updatePluginTypeUi);

    connect(ui->classNameEdit, &QLineEdit::editingFinished, this, [this]() {
        const QString className = ui->classNameEdit->text().trimmed();
        if (className.isEmpty()) {
            return;
        }

        ui->pluginIdEdit->setText(defaultPluginId(className));
        ui->displayNameEdit->setText(className);
        ui->outputDirectoryEdit->setText(QDir::toNativeSeparators(QDir::current().absoluteFilePath(QStringLiteral("plugins/%1").arg(className))));
    });

    ui->pageStack->setCurrentIndex(TypePage);
    updateButtons();
}

PluginProjectOptions PluginCreatorWindow::collectOptions() const
{
    PluginProjectOptions options;
    options.type = isViewPluginSelected() ? CreatorPluginType::View : CreatorPluginType::Service;
    options.area = static_cast<CreatorWindowArea>(ui->areaComboBox->currentData().toInt());
    options.className = ui->classNameEdit->text().trimmed();
    options.pluginId = ui->pluginIdEdit->text().trimmed();
    options.displayName = ui->displayNameEdit->text().trimmed();
    options.version = ui->versionEdit->text().trimmed();
    options.author = ui->authorEdit->text().trimmed();
    options.description = ui->descriptionEdit->toPlainText().trimmed();
    options.outputDirectory = QDir::fromNativeSeparators(ui->outputDirectoryEdit->text().trimmed());
    options.registerCapability = ui->registerCapabilityCheck->isChecked();
    options.useMessageBus = ui->messageBusCheck->isChecked();
    options.useLogService = ui->logServiceCheck->isChecked();
    options.usePluginSettings = ui->pluginSettingsCheck->isChecked();
    return options;
}

void PluginCreatorWindow::refreshSummary()
{
    const PluginProjectOptions options = collectOptions();
    QStringList files;
    files << QStringLiteral("CMakeLists.txt")
          << QStringLiteral("metadata.json")
          << QStringLiteral("%1.h").arg(options.className)
          << QStringLiteral("%1.cpp").arg(options.className);

    if (options.type == CreatorPluginType::View) {
        const QString widgetClass = options.className + QStringLiteral("Widget");
        files << QStringLiteral("%1.h").arg(widgetClass)
              << QStringLiteral("%1.cpp").arg(widgetClass)
              << QStringLiteral("%1.ui").arg(widgetClass);
    }

    QString text;
    text.append(QStringLiteral("插件类型：%1\n").arg(options.type == CreatorPluginType::View ? QStringLiteral("视图插件") : QStringLiteral("服务插件")));
    text.append(QStringLiteral("插件类名：%1\n").arg(options.className));
    text.append(QStringLiteral("插件 ID：%1\n").arg(options.pluginId));
    text.append(QStringLiteral("显示名称：%1\n").arg(options.displayName));
    text.append(QStringLiteral("版本号：%1\n").arg(options.version));
    if (options.type == CreatorPluginType::View) {
        text.append(QStringLiteral("窗口区域：%1\n").arg(areaDisplayName(options.area)));
    }
    text.append(QStringLiteral("输出目录：%1\n\n").arg(QDir::toNativeSeparators(options.outputDirectory)));
    text.append(QStringLiteral("将生成文件：\n"));
    for (const QString &fileName : files) {
        text.append(QStringLiteral("- %1\n").arg(fileName));
    }
    text.append(QStringLiteral("\n生成后请在根目录 CMakeLists.txt 中添加：\n"));
    text.append(QStringLiteral("add_subdirectory(plugins/%1)\n").arg(options.className));

    ui->summaryTextEdit->setPlainText(text);
}

void PluginCreatorWindow::updateButtons()
{
    const int currentIndex = ui->pageStack->currentIndex();
    ui->previousButton->setEnabled(currentIndex > TypePage && currentIndex < ResultPage);
    ui->nextButton->setVisible(currentIndex < SummaryPage);
    ui->generateButton->setVisible(currentIndex == SummaryPage);

    if (currentIndex == TypePage || currentIndex == ResultPage) {
        ui->previousButton->setEnabled(false);
    }
}

bool PluginCreatorWindow::isViewPluginSelected() const
{
    return ui->viewPluginRadio->isChecked();
}
