#include "MainWindow.h"
#include <QApplication>
#include <QFont>
#include <QLocale>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // 设置中文字体
    QFont font("Microsoft YaHei", 10);
    QApplication::setFont(font);
    

    MainWindow window;
    window.show();

    return app.exec();
} 