#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QClipboard>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
//    , ui(new Ui::MainWindow)
{
//    ui->setupUi(this);
//    initDstAddr();



}

void MainWindow::initDstAddr()
{
    QWidget *m_pWidget = new QWidget();
    this->setCentralWidget(m_pWidget);

    m_pDstAddrLayout = new QHBoxLayout(this);
    m_pDstAddrLayout->setSpacing(3);
    m_pDstAddrLayout->setContentsMargins(0,0,0,0);


    m_pLineIP_1 = new QLineEdit(this);
    m_pLineIP_2 = new QLineEdit(this);
    m_pLineIP_3 = new QLineEdit(this);
    m_pLineIP_4 = new QLineEdit(this);

    //  IP四段输入框
    initIPEdit(m_pLineIP_1);
    initIPEdit(m_pLineIP_2);
    initIPEdit(m_pLineIP_3);
    initIPEdit(m_pLineIP_4);

    m_pDot1 = new QLabel(".",this);
    m_pDot2 = new QLabel(".",this);
    m_pDot3 = new QLabel(".",this);

    m_pDstAddrLayout->addWidget(m_pLineIP_1);
    m_pDstAddrLayout->addWidget(m_pDot1);
    m_pDstAddrLayout->addWidget(m_pLineIP_2);
    m_pDstAddrLayout->addWidget(m_pDot2);
    m_pDstAddrLayout->addWidget(m_pLineIP_2);
    m_pDstAddrLayout->addWidget(m_pDot2);

    m_pWidget->setLayout(m_pDstAddrLayout);
}

//  输入合法判断
void MainWindow::initIPEdit(QLineEdit* edit)
{
    edit->setFrame(false);
    edit->setAlignment(Qt::AlignCenter);
    edit->installEventFilter(this);

    QRegExpValidator* validator = new QRegExpValidator(QRegExp("^(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$"), this);
    edit->setValidator(validator);

//    connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(editTextChanged(const QString&)));
    connect(edit,
            &QLineEdit::textChanged,
            this,
            &MainWindow::sl_ipEdit);
}


//  指针指向下一个LineEdit
QLineEdit* MainWindow::m_pNextEdit(QLineEdit *m_pCurEdit)
{
    if (m_pCurEdit == m_pLineIP_1)
            return m_pLineIP_2;
        else if (m_pCurEdit == m_pLineIP_2)
            return m_pLineIP_3;
        else if (m_pCurEdit == m_pLineIP_3)
            return m_pLineIP_4;
        else
        return NULL;
}

bool MainWindow::isEdit(QObject *object)
{
    return(object == m_pLineIP_1 ||
           object == m_pLineIP_2 ||
           object == m_pLineIP_3 ||
           object == m_pLineIP_4 );
}

QString MainWindow::get_ip()
{
    QString sec1 = m_pLineIP_1->text().isEmpty()?"0":m_pLineIP_1->text();
    QString sec2 = m_pLineIP_2->text().isEmpty()?"0":m_pLineIP_2->text();
    QString sec3 = m_pLineIP_3->text().isEmpty()?"0":m_pLineIP_3->text();
    QString sec4 = m_pLineIP_4->text().isEmpty()?"0":m_pLineIP_4->text();
    return QString("%1.%2.%3.%4").arg(sec1).arg(sec2).arg(sec3).arg(sec4);
}

void MainWindow::setText(const QString &text)
{
    QRegExp reg("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
        if (!reg.exactMatch(text))
            return;

        QStringList ips = text.split(".");
        m_pLineIP_1->setText(ips.at(0));
        m_pLineIP_2->setText(ips.at(1));
        m_pLineIP_3->setText(ips.at(2));
        m_pLineIP_4->setText(ips.at(3));

}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QStyleOptionFrame option;
    option.initFrom(this);
    option.lineWidth = 1;
    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &option, &painter, this);
    QWidget::paintEvent(event);
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (isEdit(object))
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Period)
            {
                QLineEdit* next = m_pNextEdit(qobject_cast<QLineEdit*>(object));
                if (next)
                {
                    next->setFocus();
                    next->selectAll();
                }
            }
            else if (keyEvent->matches(QKeySequence::Paste))
            {
                QString clip = QApplication::clipboard()->text(QClipboard::Clipboard);
                if (clip.split(".").size() == 4)
                {
                    setText(clip);
                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(object, event);
}

void MainWindow::sl_ipEdit(const QString& text)
{
    QLineEdit* curEdit = qobject_cast<QLineEdit*>(sender());
        if (text.size() == 3)
        {
            QLineEdit* next = m_pNextEdit(curEdit);
            if (next)
            {
                next->setFocus();
                next->selectAll();
            }
        }
}








MainWindow::~MainWindow()
{
//    delete ui;
}

