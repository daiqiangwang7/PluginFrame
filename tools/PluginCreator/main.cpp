#include "PluginCreatorWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PluginCreatorWindow window;
    window.show();

    return app.exec();
}
