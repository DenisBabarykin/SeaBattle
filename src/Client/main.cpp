#include <QApplication>
#include <QTextCodec>

#include "seabattle.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    QCoreApplication::setOrganizationName("DB");
    QCoreApplication::setOrganizationDomain("http://vk.com/babarykin.denis");
    QCoreApplication::setApplicationName("Sea Battle Game");


//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    SeaBattle w;
    w.show();

    return a.exec();
}
