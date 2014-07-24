#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // leitura das DLLs de imagens
    //a.addLibraryPath(QApplication::applicationDirPath()
    //                                + QDir::separator()
     //                               + "plugins");

    // pasta de bibliotecas
    // qDebug() << a.libraryPaths();
    // formatos de imagem suportados
    // qDebug() << QImageReader::supportedImageFormats();

    // um hack para salvar as configuracoes
    a.setApplicationName("hIP");
    a.setOrganizationName("hCode");
    a.setOrganizationDomain("hCode.com");

    MainWindow w;

    w.show();

    return a.exec();
}
