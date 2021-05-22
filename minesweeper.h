#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <QMainWindow>
#include <minebutton.h>
#include <QDebug>
#include <QRandomGenerator>
#include <QList>
#include <cmath>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QFile>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class Minesweeper; }
QT_END_NAMESPACE

class Minesweeper : public QMainWindow
{
    Q_OBJECT

public:
    Minesweeper(QWidget *parent = nullptr);
    ~Minesweeper();


private slots:
    void on_bStart_clicked();

    void customGameStart(int w, int h, int m);

    void clear();

    void minePressed(MineButton *button, int row, int column, int ID, bool isMine, bool isFlagged);

    void on_bClear_clicked();

    void on_sWidth_valueChanged(int arg1);

    void on_sHeight_valueChanged(int arg1);

    void EndGame();

    void CheckWin();

    void on_actionShow_Controls_triggered();

    void refreshDisplay();

    void TimerTick();

    void RefreshBombsAndFlags();

    void on_actionDark_Mode_triggered();

    void on_actionLight_Mode_triggered();

    int BombCnt();

    int FlagCnt();

    void LoadTheme();

    void LoadSettings();

    void closeEvent(QCloseEvent *e);

    void on_action8x8_10_mines_triggered();

    void on_action30x16_99_mines_triggered();

    void on_action16x16_40_mines_triggered();

    void on_actionAdjust_Size_triggered();

private:
    bool lost = false;

    Ui::Minesweeper *ui;

    int height = 0, width = 0, mineCount;

    int lastID = 0;

    QList<MineButton *> buttonList;

    QList<QPair<int, int>> mineList;

    int seconds = 0;

    QTimer *timer;

    bool firstClick = true;

    int theme = 0;

};
#endif // MINESWEEPER_H
