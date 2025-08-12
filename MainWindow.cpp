#include "MainWindow.h"
#include <QTextOption>
#include <QScrollBar>
#include <QPainter>
#include <QTransform>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // 初始化演示代码
    demoCode1 = R"(
#include "ast.h"
#include "ast.h"
int main(){
         while(a>b&&a<c){
       for(int i=0;i<10;i++){
        int a=10;
}
}
    )";

    demoCode2 = R"(
#include "ast.h"
int main(){
            while(1){
       for(int i=0;i<10;i++){
        int a=10;
}
}

     
    )";

    setupUI();
    connectActions();
}

void MainWindow::setupUI() {
    setWindowTitle("代码查重");
    setMinimumSize(1600, 800);
    setStyleSheet("background-color: #f0f0f0;");

    QWidget* mainWidget = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout;

    QVBoxLayout* controlLayout = new QVBoxLayout;
    QWidget* leftPanel = new QWidget;
    leftPanel->setFixedWidth(220);

    // 为左侧垂直布局加按钮
    showSimilarBtn = createButton("显示相似代码树", "#f39c12");
    importButton = createButton("插入代码", "#9b59b6");
    showButton = createButton("展示语法树", "#2ecc71");
    calcButton = createButton("计算查重率", "#3498db");
    preprocessButton = createButton("预处理代码", "#7f8c8d");
    clearButton = createButton("清除屏幕", "#e74c3c");
    
    controlLayout->addWidget(showSimilarBtn);
    controlLayout->addWidget(importButton);
    controlLayout->addWidget(calcButton);
    controlLayout->addWidget(showButton);
    controlLayout->addWidget(preprocessButton);
    controlLayout->addWidget(clearButton);
    controlLayout->addSpacing(20);

    resultLabel = new QLabel("");
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet(R"(
        QLabel {
            background: #ffffff;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 12px;
            font: 14px 'Microsoft YaHei';
            color: #2c3e50;
        }
    )");
    controlLayout->addWidget(resultLabel);
    controlLayout->addStretch();

    leftPanel->setLayout(controlLayout);

    // 右侧可视化区域
    view = new QGraphicsView;
    view->viewport()->installEventFilter(this);
    scene = new QGraphicsScene(this);
    view->setScene(scene);

    view->setRenderHint(QPainter::Antialiasing, true);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setStyleSheet("background: white; border: 1px solid #bdc3c7;");
    view->setSceneRect(-800, -600, 6600, 6200);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(view, 1);

    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
}

void MainWindow::connectActions() {
    connect(showSimilarBtn, &QPushButton::clicked,
        this, &MainWindow::toggleHighlighting);

    connect(importButton, &QPushButton::clicked,
        this, &MainWindow::showImportDialog);

    connect(calcButton, &QPushButton::clicked,
        this, &MainWindow::calculateRate);

    connect(showButton, &QPushButton::clicked,
        this, &MainWindow::showAST);
    
    connect(preprocessButton, &QPushButton::clicked,
        this, &MainWindow::showPreprocessedCode);
    
    connect(clearButton, &QPushButton::clicked, [this] {
        scene->clear();
        resultLabel->setText("清除成功");
    });
}

void MainWindow::showPreprocessedCode() {
    QDialog dialog(this);
    dialog.setWindowTitle("预处理后的代码");
    dialog.setMinimumSize(800, 600);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QTextEdit* code1View = createReadOnlyCodeView(preprocess(demoCode1));
    QTextEdit* code2View = createReadOnlyCodeView(preprocess(demoCode2));

    mainLayout->addWidget(new QLabel("代码1预处理结果:"));
    mainLayout->addWidget(code1View);
    mainLayout->addWidget(new QLabel("代码2预处理结果:"));
    mainLayout->addWidget(code2View);

    dialog.exec();
}

void MainWindow::toggleHighlighting() {
    highlightingEnabled = !highlightingEnabled;
    showSimilarBtn->setStyleSheet(QString("background: %1;")
        .arg(highlightingEnabled ? "#f39c12" : "#95a5a6"));
    showAST();
}

void MainWindow::showImportDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("选择相应位置插入代码");
    dialog.setMinimumSize(600, 600);
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QTextOption codeOption;
    codeOption.setAlignment(Qt::AlignLeft);
    codeOption.setFlags(QTextOption::Flag::IncludeTrailingSpaces);
    codeOption.setWrapMode(QTextOption::NoWrap);

    QLabel* label1 = new QLabel("代码1:");
    QTextEdit* code1Edit = new QTextEdit();
    code1Edit->setPlainText(QString::fromStdString(demoCode1));
    code1Edit->setWordWrapMode(QTextOption::NoWrap);
    code1Edit->setLineWrapMode(QTextEdit::NoWrap);
    code1Edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    code1Edit->setTabStopDistance(40);
    code1Edit->document()->setDefaultTextOption(codeOption);

    QLabel* label2 = new QLabel("代码2:");
    QTextEdit* code2Edit = new QTextEdit();
    code2Edit->setPlainText(QString::fromStdString(demoCode2));
    code2Edit->setAcceptRichText(false);
    code2Edit->setWordWrapMode(QTextOption::NoWrap);
    code2Edit->setLineWrapMode(QTextEdit::NoWrap);
    code2Edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    code2Edit->setTabStopDistance(40);
    code2Edit->document()->setDefaultTextOption(codeOption);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    mainLayout->addWidget(label1);
    mainLayout->addWidget(code1Edit);
    mainLayout->addWidget(label2);
    mainLayout->addWidget(code2Edit);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        demoCode1 = code1Edit->toPlainText().toStdString();
        demoCode2 = code2Edit->toPlainText().toStdString();
        resultLabel->setText("插入成功！\n");
    }
}

void MainWindow::calculateRate() {
    try {
        auto tokens1 = tokenize(preprocess(demoCode1));
        auto tokens2 = tokenize(preprocess(demoCode2));
        ASTNode* root1 = parseCode(tokens1);
        ASTNode* root2 = parseCode(tokens2);

        double rate = calculateDuplicationRate(root1, root2);
        resultLabel->setText(QString("查重率: %1%").arg(rate * 100, 0, 'f', 2));

        delete root1;
        delete root2;
    }
    catch (const std::exception& e) {
        showError("no", e.what());
    }
}

void MainWindow::showAST() {
    scene->clear();
    try {
        auto tokens1 = tokenize(preprocess(demoCode1));
        ASTNode* astRoot1 = parseCode(tokens1);

        auto tokens2 = tokenize(preprocess(demoCode2));
        ASTNode* astRoot2 = parseCode(tokens2);

        ASTVisualizer visualizer;
        if (highlightingEnabled) {
            visualizer.compareAndHighlight(astRoot1, astRoot2);
        }
        else {
            visualizer.clearColors();
        }

        view->resetTransform();
        view->setRenderHint(QPainter::Antialiasing, true);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setInteractive(true);

        visualizer.visualize(astRoot1, scene, 200, 50);
        visualizer.visualize(astRoot2, scene, 200, 850);

        QRectF totalRect = scene->itemsBoundingRect();
        totalRect.adjust(-200, -200, 300, 300);
        qreal scaleFactor = qMin(
            view->viewport()->width() / totalRect.width() * 3.5,
            view->viewport()->height() / totalRect.height() * 3.5
        );
        view->scale(scaleFactor, scaleFactor);

        view->viewport()->update();

        delete astRoot1;
        delete astRoot2;
    }
    catch (const std::exception& e) {
        showError("可视化错误", e.what());
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == view->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            const double zoomStep = 0.1;
            double zoom = 1.0 + (wheelEvent->angleDelta().y() > 0 ? zoomStep : -zoomStep);

            zoomFactor *= zoom;
            zoomFactor = qBound(0.2, zoomFactor, 5.0);

            QTransform transform;
            transform.scale(zoomFactor, zoomFactor);
            view->setTransform(transform);

            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

QPushButton* MainWindow::createButton(const QString& text, const QString& color) {
    QPushButton* btn = new QPushButton(text);
    btn->setStyleSheet(QString(R"(
        QPushButton {
            background: %1;
            color: white;
            border: none;
            padding: 12px;
            border-radius: 6px;
            font: bold 14px 'Microsoft YaHei';
        }
        QPushButton:hover {
            background: %2;
        }
    )").arg(color).arg(QColor(color).darker(120).name()));
    return btn;
}

void MainWindow::showError(const QString& title, const QString& msg) {
    QMessageBox::critical(this, title,
        QString("<b>错误详情：</b><br>%1").arg(msg));
} 