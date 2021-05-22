#include "minebutton.h"
#include "ui_minebutton.h"

MineButton::MineButton(QWidget *parent, int ID) :
    QPushButton(parent),
    ui(new Ui::MineButton)
{
    ui->setupUi(this);

    this->setCheckable(true);
    this->ID = ID;

    QFile style(":/minebutton.qss");
    style.open(QIODevice::ReadOnly);

    this->setFont(this->font());

    QFont f = this->font();
    f.setPointSize(FONT_SIZE_DEACTIVATED);
    this->setFont(f);

    this->setStyleSheet(this->styleSheet() + style.readAll());

    //setText(QString::number(ID));

}

MineButton::~MineButton()
{
    delete ui;
}

void MineButton::Press(){
    emit Pressed(this, row, column, ID, isMine, isFlagged);
}

void MineButton::mouseReleaseEvent(QMouseEvent *e){
    if(isActive){
        if(e->button() == Qt::LeftButton && !isFlagged){
            emit Pressed(this, row, column, ID, isMine, isFlagged);
            isActive = false;

            this->setChecked(true);
        }
        else if(e->button() == Qt::RightButton){

            this->isFlagged = !this->isFlagged;
            if(isFlagged){
                //this->setText("Flagged");
                QPixmap img(":/Resources/flag_icon.png");
                this->setIcon(img.scaled(this->size()));
                emit Pressed(this, row, column, ID, isMine, isFlagged);
            }
            else {
                this->setText("");
                this->setIcon(QPixmap());
            }
        }
    }
    else{
        emit Pressed(this, row, column, ID, isMine, isFlagged);
    }
}

void MineButton::mousePressEvent(QMouseEvent *e){

}
