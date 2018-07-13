#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include <QIODevice>
#include <QStatusBar>
#include "mainwindow.h"
#include "CodeParse.h"
#include "CoeBinParse.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    textEdit = new QsciScintilla(this);
    textEdit->setMarginType(0,QsciScintilla::NumberMargin);//设置编号为0的页边显示行号。
    textEdit->setMarginLineNumbers(0,true);//对该页边启用行号
    textEdit->setMarginWidth(0,30);//设置页边宽

    connect(textEdit, SIGNAL(textChanged()),
            this, SLOT(documentWasModified()));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    textLexer = new HighLight;
    textLexer->setColor(QColor(Qt::green),QsciLexerCPP::CommentLine);    //设置自带的注释行为绿色
    textLexer->setColor(QColor(Qt::red),QsciLexerCPP::KeywordSet2);
    textEdit->setLexer(textLexer);
    textEdit->setFont(QFont(tr("Consolas"), 13));

    setCentralWidget(textEdit);
    setWindowIcon(QIcon(":/images/logo.png"));
    setCurrentFile("");
    resize(720, 540);
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->isModified());
}

void MainWindow::createActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon(":/images/new.png"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("&Open..."), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, QAction::triggered, this, open);

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, QAction::triggered, this, save);

    saveAsAction = new QAction(tr("Save &As..."), this);
    saveAsAction->setShortcut(tr("Alt+S"));
    connect(saveAsAction, QAction::triggered, this, saveAs);

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, QAction::triggered, this, close);

    aboutAction = new QAction(tr("&About Author"), this);
    aboutAction->setShortcut(tr("Alt+A"));
    connect(aboutAction, QAction::triggered, this, about);

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutAction->setShortcut(tr("Alt+Q"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    convertToCoeAction = new QAction(tr("Convert To &Coe"), this);
    convertToCoeAction->setShortcut(tr("Ctrl+Alt+C"));
    convertToCoeAction->setIcon(QIcon(":/images/convertToCoe.png"));
    connect(convertToCoeAction, QAction::triggered, this, convertToCoe);

    convertToBinAction = new QAction(tr("Convert To &Bin"), this);
    convertToBinAction->setIcon(QIcon(":/images/convertToBin.png"));
    convertToBinAction->setShortcut(tr("Ctrl+Alt+B"));
    connect(convertToBinAction, QAction::triggered, this, convertToBin);

    disAssemblyCoeAction = new QAction(tr("&DisAssembly Coe"), this);
    disAssemblyCoeAction->setIcon(QIcon(":/images/disAssembly.png"));
    disAssemblyCoeAction->setShortcut(tr("Ctrl+Shift+C"));
    connect(disAssemblyCoeAction, QAction::triggered, this, disAssemblyCoe);

    disAssemblyBinAction = new QAction(tr("&DisAssembly Bin"), this);
    disAssemblyBinAction->setIcon(QIcon(":/images/disAssembly2.png"));
    disAssemblyBinAction->setShortcut(tr("Ctrl+Shift+B"));
    connect(disAssemblyBinAction, QAction::triggered, this, disAssemblyBin);

    copyAction = new QAction(tr("&Copy"));
    copyAction->setIcon(QIcon(":/images/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, QAction::triggered, textEdit, QsciScintilla::copy);

    cutAction = new QAction(tr("C&ut"));
    cutAction->setIcon(QIcon(":/images/cut.png"));
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, QAction::triggered, textEdit, QsciScintilla::cut);

    pasteAction = new QAction(tr("&Paste"));
    pasteAction->setIcon(QIcon(":/images/paste.png"));
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, QAction::triggered, textEdit, QsciScintilla::paste);

    undoAction = new QAction(tr("U&ndo"));
    undoAction->setIcon(QIcon(":/images/undo.png"));
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, QAction::triggered, textEdit, QsciScintilla::undo);

    selectAllAction = new QAction(tr("S&electAll"));
    selectAllAction->setIcon(QIcon(":/images/selectAll.png"));
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, QAction::triggered, textEdit, QsciScintilla::selectAll);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(copyAction);
    editMenu->addAction(cutAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(undoAction);
    editMenu->addAction(selectAllAction);

    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(convertToCoeAction);
    optionsMenu->addAction(convertToBinAction);
    optionsMenu->addAction(disAssemblyCoeAction);
    optionsMenu->addAction(disAssemblyBinAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);
    fileToolBar->addAction(convertToCoeAction);
    fileToolBar->addAction(convertToBinAction);
    fileToolBar->addAction(disAssemblyCoeAction);
    fileToolBar->addAction(disAssemblyBinAction);
}

void MainWindow::createStatusBar()
{
    bottomLabel = new QLabel("0 Warning ! 0 Error ! Enjoy Yourself !");
    bottomLabel->setAlignment(Qt::AlignHCenter);
    bottomLabel->setMinimumSize(bottomLabel->sizeHint());
    statusBar()->addWidget(bottomLabel);
}

bool MainWindow::okToContinue()
{
    if (isWindowModified()) {
        int ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::newFile()
{
    if (okToContinue()) {
        textEdit->clear();
        setCurrentFile("");
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = tr("Untitled");
    if (!curFile.isEmpty()) {
        shownName = curFile;
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("QMA"))); // modified & unsaved *!!!
                                                        // unmodified !!!
}

void MainWindow::open()
{
    QString dummy;
    dummy +=  QCoreApplication::applicationDirPath();
    dummy += "..";
    if (okToContinue()) {
        QString path = QFileDialog::getOpenFileName(
                    this,   //父窗口
                    tr("Open File"),    //对话框名字
                    dummy,    //默认目录
                    tr("Asm File(*.asm);;Asm File(*.ASM);;Coe File(*.coe)")
                    );
        if (!path.isEmpty())
        {
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QMessageBox ::information(NULL, NULL, "open file error");
                return; //打开失败
            }
            QTextStream in(&file);
            textEdit->setText(in.readAll());
            file.close();
            setCurrentFile(path);
        }
    }
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(NULL, NULL, "Write file error");
        return false; //打开失败
    }
    QTextStream out(&file);
    out << textEdit->text();
    file.close();

    setCurrentFile(fileName);       // window modified is FAKED!!!
    return true;
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString dummy;
    dummy +=  QCoreApplication::applicationDirPath();
    dummy += "..";
    QString fileName = QFileDialog::getSaveFileName(
                this,   //父窗口
                tr("Save File"),    //对话框名字
                dummy,    //默认目录
                tr("Asm File(*.asm);;Asm File(*.ASM);;Coe File(*.coe)")
                );
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QMA"),
            tr("<h2>QMA 1.1  QIndomitable Mips Assembler</h2>"
               "<p>Copyright &copy; 2018.5 QIndomitable."
               "<p>A naive implementation by Qt5"
               "<p>Connact me for BUGS: -> "
               "3160101819@zju.edu.cn"));
}

void MainWindow::convertToCoe()
{
    QString dummy;
    dummy +=  QCoreApplication::applicationDirPath();
    dummy += "..";
    if(curFile.isEmpty() && !isWindowModified()) {
        QMessageBox::information(NULL, NULL, "Please open/new a ASM file firstly");
        bottomLabel->setText("Please open/new a ASM file firstly");
    } else if(okToContinue()){
        QString fileName = QFileDialog::getSaveFileName(
                    this,   //父窗口
                    tr("Output to"),    //对话框名字
                    dummy,    //默认目录
                    tr("Coe File(*.coe)")
                    );
        if(fileName.isEmpty()) {
            QMessageBox::information(NULL, NULL, "Please name *.coe");
            bottomLabel->setText("Please name *.coe");
        } else {
            CodeParse parser(curFile.toStdString());
            parser.format();
            parser.convertToCoe(fileName.toStdString());
            bottomLabel->setText(curFile + " -> " + fileName + " Successfully !");
            return;
        }
    }
    return;
}

void MainWindow::convertToBin()
{
    QString dummy;
    dummy +=  QCoreApplication::applicationDirPath();
    dummy += "..";
    if(curFile.isEmpty()) {
        QMessageBox::information(NULL, NULL, "Please open/new a ASM file firstly");
        bottomLabel->setText("Please open/new a ASM file firstly");
    } else if(okToContinue()){
       QString fileName = QFileDialog::getSaveFileName(
                   this,   //父窗口
                   tr("Output to"),    //对话框名字
                   dummy,    //默认目录
                   tr("Bin File(*.bin)")
                   );
       if(fileName.isEmpty()) {
           QMessageBox::information(NULL, NULL, "Please name *.bin");
           bottomLabel->setText("Please name *.bin");
       } else {
           CodeParse parser(curFile.toStdString());
           parser.format();
           parser.convertToBin(fileName.toStdString());
           bottomLabel->setText(curFile + " -> " + fileName + " Successfully !");
           return;
       }
    }
    return;
}

void MainWindow::disAssemblyCoe()
{
    QString dummy;
    dummy +=  QCoreApplication::applicationDirPath();
    dummy += "..";
    QString source = QFileDialog::getOpenFileName(
                this,   //父窗口
                tr("Choose a Coe File"),       //对话框名字
                dummy,    //默认目录
                tr("Coe File(*.coe)")
                );
    QString target = QFileDialog::getSaveFileName(
                this,   //父窗口
                tr("Output to"),    //对话框名字
                dummy,    //默认目录
                tr("Asm File(*.asm);;Asm File(*.ASM)")
                );
    if(source.isEmpty()) {
        QMessageBox::information(NULL, NULL, "Please open *.coe");
        bottomLabel->setText("Please open *.coe");
    }
    if(target.isEmpty()){
        QMessageBox::information(NULL, NULL, "Please name *.asm or *.ASM");
        bottomLabel->setText("Please name *.asm or *.ASM");
    }
    if(!source.isEmpty() && !target.isEmpty()) {
        CoeBinParse cbParser(source.toStdString());
        cbParser.format(COE);
        cbParser.convertToAsm(target.toStdString());
        bottomLabel->setText(source + " -> " + target + " Successfully !");
        return;
    }
     return;
}

void MainWindow::disAssemblyBin()
{
    QString dummy;
    dummy +=  QCoreApplication::applicationDirPath();
    dummy += "..";
    QString source = QFileDialog::getOpenFileName(
                this,   //父窗口
                tr("Choose a Coe File"),       //对话框名字
                dummy,    //默认目录
                tr("Bin File(*.bin)")
                );
    QString target = QFileDialog::getSaveFileName(
                this,   //父窗口
                tr("Output to"),    //对话框名字
                dummy,    //默认目录
                tr("Asm File(*.asm);;Asm File(*.ASM)")
                );
    if(source.isEmpty()){
        QMessageBox::information(NULL, NULL, "Please open *.bin");
        bottomLabel->setText("Please open *.bin");
    }
    if(target.isEmpty()){
        QMessageBox::information(NULL, NULL, "Please name *.asm or *.ASM");
        bottomLabel->setText("Please name *.asm or *.ASM");
    }
    if(!source.isEmpty() && !target.isEmpty()) {
        CoeBinParse cbParser(source.toStdString());
        cbParser.format(BIN);
        cbParser.convertToAsm(target.toStdString());
        bottomLabel->setText(source + " -> " + target + " Successfully !");
        return;
    }
    return;
}
