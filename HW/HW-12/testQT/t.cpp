#include "ui_untitled.h"
using namespace Ui;

int main(int argc,char *argv[])
{
    QApplication test(argc, argv);
    QDialog qdlg;
    Dialog dlg(&qdlg);
    qdlg.show();
    return test.exec();
}