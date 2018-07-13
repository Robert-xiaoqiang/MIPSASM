#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QComboBox>
#include <QSpinBox>
#include <QToolBar>
#include <QFontComboBox>
#include <QToolButton>
#include <QTextCharFormat>
#include <QTextEdit>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercustom.h>
#include "highlight.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void convertToCoe();
    void convertToBin();
    void disAssemblyCoe();
    void disAssemblyBin();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    bool okToContinue();
    bool loadFile(const QString& filename);
    bool saveFile(const QString& filename);
    void setCurrentFile(const QString& filename);

    QsciScintilla *textEdit;
    HighLight *textLexer;
    QString curFile;
    enum Hack{MaxRecentFiles = 5};

    QMenu *fileMenu;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *openBinAction;

    QMenu *editMenu;
    QAction *copyAction;
    QAction *cutAction;
    QAction *pasteAction;
    QAction *selectAllAction;
    QAction *undoAction;

    QMenu *helpMenu;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QToolBar *fileToolBar;
    QMenu *optionsMenu;
    QAction *convertToCoeAction;
    QAction *convertToBinAction;
    QAction *disAssemblyCoeAction;
    QAction *disAssemblyBinAction;

    QLabel *bottomLabel;
};

#endif // MAINWINDOW_H
