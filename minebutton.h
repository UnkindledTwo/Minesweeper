#ifndef MINEBUTTON_H
#define MINEBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QPixmap>

#include <QDebug>

#define SQUARE_SIZE 35
#define FONT_SIZE_DEACTIVATED 12

namespace Ui {
class MineButton;
}

class MineButton : public QPushButton
{
    Q_OBJECT


signals:
    void Pressed(MineButton *button, int row, int column, int ID, bool isMine, bool isFlagged);
public:
    bool isActive = true;

    bool isMine = false;

    bool isFlagged = false;

    int row, column, ID;

    explicit MineButton(QWidget *parent = nullptr, int ID = -1);
    ~MineButton();

    void Press();
private:

    Ui::MineButton *ui;

    void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

    QString style;
};

#endif // MINEBUTTON_H
