#pragma once

#include <QtWidgets/QMainWindow>
#include <memory>

namespace capvision {
namespace ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrame();

private:
    void setupUi();
    void initializeCamera();

    class MainWindowPrivate;
    std::unique_ptr<MainWindowPrivate> d_ptr;
};

} // namespace ui
} // namespace capvision