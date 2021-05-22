#include "minesweeper.h"
#include "ui_minesweeper.h"

Minesweeper::Minesweeper(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Minesweeper)
{
    ui->setupUi(this);

    QApplication::setApplicationName("Minesweeper");

    ui->mineArea->setEnabled(false);

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &Minesweeper::TimerTick);

    ui->sWidth->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->sHeight->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->sMines->setButtonSymbols(QAbstractSpinBox::NoButtons);

    LoadSettings();
}

Minesweeper::~Minesweeper()
{
    delete ui;
}

void Minesweeper::LoadSettings(){
    QSettings settings("Minesweeper");

    theme = settings.value("theme").toInt();
    ui->sWidth->setValue(settings.value("width").toInt());
    ui->sHeight->setValue(settings.value("height").toInt());
    ui->sMines->setValue(settings.value("minecount").toInt());

    LoadTheme();

    qDebug() << "Loaded settings: " + settings.fileName();
}

void Minesweeper::closeEvent(QCloseEvent *e){
    QSettings settings("Minesweeper");

    settings.setValue("theme", theme);
    settings.setValue("width", ui->sWidth->value());
    settings.setValue("height", ui->sHeight->value());
    settings.setValue("minecount", ui->sMines->value());

    qDebug() << "Saved settings: " + settings.fileName();
}

void Minesweeper::LoadTheme(){
    switch (theme) {
    case 0:
        on_actionLight_Mode_triggered();
        break;
    case 1:
        on_actionDark_Mode_triggered();
        break;
    }
}

void Minesweeper::clear(){

    for(int i = 0; i < buttonList.count(); i++){
        buttonList[i]->deleteLater();
    }

    ui->lBombs->clear();
    ui->lFlags->clear();

    seconds = 0;
    refreshDisplay();
    timer->stop();

    buttonList.clear();

    lastID = 0;

    firstClick = true;
}

void Minesweeper::RefreshBombsAndFlags(){
    int bombcnt = 0;
    int flagcnt = 0;
    for(int i = 0; i < buttonList.count(); i++){
        MineButton *button = buttonList.at(i);

        if(button->isFlagged) {flagcnt++;}
        if(button->isMine)    {bombcnt++;}

    }
    ui->lBombs->setText(QString::number(bombcnt));
    ui->lFlags->setText(QString::number(flagcnt));
}

void Minesweeper::on_bClear_clicked()
{
    clear();
}

void Minesweeper::on_bStart_clicked()
{
    clear();

    timer->start();

    lost = false;

    height = ui->sHeight->value();
    width = ui->sWidth->value();
    mineCount = ui->sMines->value();

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            MineButton *m = new MineButton(0, lastID);
            m->row = i;
            m->column = j;
            connect(m, &MineButton::Pressed, this, &Minesweeper::minePressed);

            ui->mineArea->addWidget(m, i, j);
            buttonList.append(m);

            m->setFixedSize(SQUARE_SIZE, SQUARE_SIZE);
            m->setMinimumSize(SQUARE_SIZE, SQUARE_SIZE);
            m->setMaximumSize(SQUARE_SIZE, SQUARE_SIZE);

            lastID++;
        }

    }

    this->setMinimumHeight(height * SQUARE_SIZE + 120 );
    this->setMinimumWidth(width * SQUARE_SIZE + ui->groupBox->width() + 60);

    RefreshBombsAndFlags();
}


void Minesweeper::minePressed(MineButton *button, int row, int column, int ID, bool isMine, bool isFlagged){
    if(firstClick){
        for(int i = 0; i <= mineCount; i++){
            int rowR = 0, columnR = 0;

            rowR = QRandomGenerator::global()->bounded(0, height);
            columnR = QRandomGenerator::global()->bounded(0, width);

            MineButton *button1 = (MineButton *)ui->mineArea->itemAtPosition(rowR, columnR)->widget();

            if(button1->isMine)
                mineCount++;
            else
                button1->isMine = true;

            if(abs(button1->column - column) == 1 && abs(button1->row - row) == 1){
                button1->isMine = false;
            }
            if(button1->column == column && abs(button1->row - row) == 1){
                button1->isMine = false;
            }
            if(abs(button1->column - column) == 1 && button1->row == row){
                button1->isMine = false;
            }
        }
        qDebug() << "First click";
        firstClick = false;
        button->isMine = false;
        button->Press();
    }

    if(isFlagged){CheckWin(); RefreshBombsAndFlags(); return;}
    qDebug() << "Mine pressed with id: " + QString::number(ID);

    int bombCount = 0;
    bool noBombNear = false;

    /*Check for boms in the 8 direction*/
    for(int i = 0; i < buttonList.count(); i++){
        if(abs(buttonList.at(i)->column - column) == 1 && abs(buttonList.at(i)->row - row) == 1){
            if(buttonList.at(i)->isMine)
                bombCount++;
        }
        if(buttonList.at(i)->column == column && abs(buttonList.at(i)->row - row) == 1){
            if(buttonList.at(i)->isMine)
                bombCount++;
        }
        if(abs(buttonList.at(i)->column - column) == 1 && buttonList.at(i)->row == row){
            if(buttonList.at(i)->isMine)
                bombCount++;
        }
        //noBombNear = bombCount == 0;
    }
    /*Check for boms in the 8 direction*/

    /*Check if pressed button is a mine*/
    if(isMine){
        QPixmap bomb(":/Resources/bomb.png");
        button->setIcon(bomb.scaled(button->size()));
        EndGame();
    }
    else{
        button->setText(QString::number(bombCount));
        QString color;
        switch (bombCount){
        case 1:
            color = "blue";
            break;
        case 2:
            color = "green";
            break;
        case 3:
            color = "red";
            break;
        case 4:
            color = "purple";
            break;
        case 5:
            color = "maroon";
            break;
        case 6:
            color = "turqoise";
            break;
        default:
            color = "black";
            break;
        }
        button->setStyleSheet(button->styleSheet()+ "QPushButton{ color: " + color + ";}");

    }
    /*Check if pressed button is a mine*/

    /*Press near mines*/
    noBombNear = bombCount == 0;
    if(noBombNear){
        for(int i = 0; i < buttonList.count(); i++){
            MineButton *button1 = buttonList.at(i);

            if(button1->column == column && button1->row == row){
                button1->setText("");
            }

            if((abs(button1->column - column) == 1 && abs(button1->row - row) == 1)){
                if(button1->isActive){
                    button1->isActive = false;
                    button1->setChecked(true);
                    button1->Press();
                }

            }
            if((button1->column == column && abs(button1->row - row) == 1)){
                if(button1->isActive){
                    button1->isActive = false;
                    button1->setChecked(true);
                    button1->Press();
                }

            }
            if((abs(button1->column - column) == 1 && button1->row == row)){
                if(button1->isActive){
                    button1->isActive = false;
                    button1->setChecked(true);
                    button1->Press();
                }

            }
        }}
    /*Press near mines*/

    if(!button->isActive){
        int nearMines = 0;
        int nearFlags = 0;

        for(int i = 0; i < buttonList.count(); i++){
            if(abs(buttonList.at(i)->column - column) == 1 && abs(buttonList.at(i)->row - row) == 1){
                if(buttonList.at(i)->isMine)
                    nearMines++;
                if(buttonList.at(i)->isFlagged){
                    nearFlags++;
                }
            }
            if(buttonList.at(i)->column == column && abs(buttonList.at(i)->row - row) == 1){
                if(buttonList.at(i)->isMine)
                    nearMines++;
                if(buttonList.at(i)->isFlagged){
                    nearFlags++;
                }
            }
            if(abs(buttonList.at(i)->column - column) == 1 && buttonList.at(i)->row == row){
                if(buttonList.at(i)->isMine)
                    nearMines++;
                if(buttonList.at(i)->isFlagged){
                    nearFlags++;
                }
            }
        }
        if(nearFlags == nearMines){
            for(int i = 0; i < buttonList.count(); i++){
                if(abs(buttonList.at(i)->column - column) == 1 && abs(buttonList.at(i)->row - row) == 1){
                    if(!buttonList.at(i)->isFlagged && buttonList.at(i)->isActive){
                        MineButton *button1 = buttonList.at(i);
                        button1->isActive = false;
                        button1->setChecked(true);
                        button1->Press();
                    }
                }
                if(buttonList.at(i)->column == column && abs(buttonList.at(i)->row - row) == 1){
                    if(!buttonList.at(i)->isFlagged && buttonList.at(i)->isActive){
                        MineButton *button1 = buttonList.at(i);
                        button1->isActive = false;
                        button1->setChecked(true);
                        button1->Press();
                    }
                }
                if(abs(buttonList.at(i)->column - column) == 1 && buttonList.at(i)->row == row){
                    if(!buttonList.at(i)->isFlagged && buttonList.at(i)->isActive){
                        MineButton *button1 = buttonList.at(i);
                        button1->isActive = false;
                        button1->setChecked(true);
                        button1->Press();
                    }
                }

            }
        }
    }
    if(!lost){
        CheckWin();
    }

    RefreshBombsAndFlags();
}

void Minesweeper::EndGame(){
    if(lost)return;
    lost = true;
    for(int i = 0; i < buttonList.count(); i++){
        MineButton *button = buttonList.at(i);
        button->setEnabled(false);
        if(button->isActive){
            button->isActive = false;
            button->setChecked(true);
            button->Press();
        }
    }
    timer->stop();

    QMessageBox::information(this, "You lose", "Better luck next time");

}

void Minesweeper::CheckWin(){
    qDebug() << "-------------";
    qDebug() << "Checking win";

    int bombcnt = 0;
    int flaggedminecnt = 0;
    int activeButtoncnt = 0;

    for(int i = 0; i < buttonList.count(); i++){
        MineButton *button = buttonList.at(i);
        if(button->isMine){
            bombcnt++;
        }
        if(button->isFlagged && button->isMine){
            flaggedminecnt++;
        }
        if(button->isActive){
            activeButtoncnt++;
        }
    }
    if(bombcnt == flaggedminecnt && flaggedminecnt == FlagCnt()){
        timer->stop();

        QMessageBox::information(this, "You win!", "You are the winner!");
        lost = true;

        for(int i = 0; i < buttonList.count(); i++){
            buttonList.at(i)->setEnabled(false);
        }
    }
    qDebug() << "Flagged mine count: " + QString::number(flaggedminecnt);
    qDebug() << "Active button count: " + QString::number(activeButtoncnt -1);
    qDebug() << "Bomb count: " + QString::number(bombcnt);
    qDebug() << "Checking win finished";
    qDebug() << "-------------";
}

void Minesweeper::on_sWidth_valueChanged(int arg1)
{
    ui->sMines->setMaximum(arg1 * ui->sHeight->value());
    ui->sMines->setMinimum((arg1 * ui->sHeight->value()) / 15);
}


void Minesweeper::on_sHeight_valueChanged(int arg1)
{
    ui->sMines->setMaximum(arg1 * ui->sWidth->value());
    ui->sMines->setMinimum((arg1 * ui->sWidth->value()) / 15);
}


void Minesweeper::on_actionShow_Controls_triggered()
{
    if(ui->groupBox->isHidden()){
        ui->groupBox->show();
    }
    else ui->groupBox->hide();
}

void Minesweeper::refreshDisplay(){
    ui->timeDisplay->display(seconds);
}

void Minesweeper::TimerTick(){
    seconds++;
    refreshDisplay();
}

void Minesweeper::on_actionDark_Mode_triggered()
{
    QFile darkFile(":/dark.qss");
    darkFile.open(QIODevice::ReadOnly);

    this->setStyleSheet(darkFile.readAll());
    darkFile.close();
    theme = 1;
}

void Minesweeper::on_actionLight_Mode_triggered()
{
    QFile lightFile(":/light.qss");
    lightFile.open(QIODevice::ReadOnly);

    this->setStyleSheet(lightFile.readAll());
    lightFile.close();
    theme = 0;
}

int Minesweeper::FlagCnt(){
    int val = 0;
    for(int i = 0; i < buttonList.count(); i++){
        MineButton *button = buttonList.at(i);
        if(button->isFlagged){
            val++;
        }
    }
    return val;
}

int Minesweeper::BombCnt(){
    int val = 0;
    for(int i = 0; i < buttonList.count(); i++){
        MineButton *button = buttonList.at(i);
        if(button->isMine){
            val++;
        }
    }
    return val;
}

void Minesweeper::customGameStart(int w, int h, int m){
    ui->sWidth->setValue(w);
    ui->sHeight->setValue(h);
    ui->sMines->setValue(m);
    on_bStart_clicked();
}

void Minesweeper::on_action8x8_10_mines_triggered()
{
    customGameStart(8,8,10);
}

void Minesweeper::on_action30x16_99_mines_triggered()
{
    customGameStart(30,16,99);
}


void Minesweeper::on_action16x16_40_mines_triggered()
{
    customGameStart(16,16,40);
}


void Minesweeper::on_actionAdjust_Size_triggered()
{
    this->resize(this->minimumSize());
}
