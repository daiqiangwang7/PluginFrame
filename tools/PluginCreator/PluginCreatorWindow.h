#ifndef PLUGINCREATORWINDOW_H
#define PLUGINCREATORWINDOW_H

#include "PluginProjectOptions.h"

#include <QMainWindow>

namespace Ui {
class PluginCreatorWindow;
}

class PluginCreatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*
     * 创建插件创建向导窗口。
     * parent 为 Qt 对象父节点，可为空。
     */
    explicit PluginCreatorWindow(QWidget *parent = nullptr);

    /*
     * 析构插件创建向导窗口。
     * 析构时释放 Qt Designer 生成的界面对象。
     */
    ~PluginCreatorWindow() override;

private slots:
    /*
     * 切换到上一页。
     * 当前已经是第一页时保持不变。
     */
    void previousPage();

    /*
     * 切换到下一页。
     * 当前页信息校验通过后进入下一页。
     */
    void nextPage();

    /*
     * 执行插件生成。
     * 从界面收集参数并调用插件工程生成器写入文件。
     */
    void generatePlugin();

    /*
     * 选择插件输出目录。
     * 打开目录选择对话框并更新输出目录输入框。
     */
    void chooseOutputDirectory();

    /*
     * 根据插件类型刷新向导页面。
     * 视图插件显示视图选项页，服务插件跳过该页面。
     */
    void updatePluginTypeUi();

private:
    /*
     * 初始化界面默认状态。
     * 设置中文标题、默认字段值和按钮连接。
     */
    void setupDefaults();

    /*
     * 从界面收集插件创建参数。
     * 返回值用于校验和生成插件工程。
     */
    PluginProjectOptions collectOptions() const;

    /*
     * 刷新确认页内容。
     * 根据当前插件创建参数展示即将生成的文件和 CMake 提示。
     */
    void refreshSummary();

    /*
     * 刷新向导按钮状态。
     * 根据当前页决定上一步、下一步和生成按钮是否可用。
     */
    void updateButtons();

    /*
     * 刷新注册器步骤导航状态。
     * 根据当前页高亮左侧步骤文本。
     */
    void updateStepLabels();

    /*
     * 判断当前插件类型是否为视图插件。
     * 返回 true 表示当前选择视图插件，false 表示服务插件。
     */
    bool isViewPluginSelected() const;

    Ui::PluginCreatorWindow *ui = nullptr;
};

#endif // PLUGINCREATORWINDOW_H
