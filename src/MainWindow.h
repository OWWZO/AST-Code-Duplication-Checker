#pragma once

#include "ASTVisualizer.h"
#include "CodeProcessor.h"
#include "Utils.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFont>
#include <QTextEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QWheelEvent>
#include <QEvent>
#include <QTransform>
#include <QApplication>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void showPreprocessedCode();
    void toggleHighlighting();
    void showImportDialog();
    void calculateRate();
    void showAST();

private:
    void setupUI();
    void connectActions();
    QPushButton* createButton(const QString& text, const QString& color);
    void showError(const QString& title, const QString& msg);
    bool eventFilter(QObject* obj, QEvent* event);

    // UI组件
    double zoomFactor = 1.0;
    bool highlightingEnabled = false;
    QPushButton* showSimilarBtn;
    QGraphicsView* view;
    QGraphicsScene* scene;
    QLabel* resultLabel;
    QPushButton* preprocessButton;
    QPushButton* calcButton;
    QPushButton* showButton;
    QPushButton* clearButton;
    QPushButton* importButton;

    // 演示代码
    std::string demoCode1;
    std::string demoCode2;
}; 