/*
* Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/
#include "setupPage.h"
#include "ui_setupPage.h"
#include "QSlider"
#include <QPixmap>
#include "setAlarmRepeatDialog.h"
#include <QDebug>
#include <QBitmap>
#include <QPainterPath>
#include "customStyle.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

setuppage::setuppage( double position_x, double position_y, QWidget *parent  ) :
    QWidget(parent),
    pos_x(position_x),
    pos_y(position_y),
    ui(new Ui::setuppage)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    this->setProperty("blurRegion", QRegion(QRect(1, 1, 1, 1)));
    Qt::WindowFlags m_flags = windowFlags();
    this->setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);

    ui->setupUi(this);
    this->setWindowOpacity(0.5);
    ui->pushButton->hide();

    dialog_werk_day = new  set_alarm_repeat_Dialog(ui->widget, 7);
    Time_format = new  set_alarm_repeat_Dialog(ui->widget, 3);
    Pop_up_window = new  set_alarm_repeat_Dialog(ui->widget, 2);
    Reminder_off = new  set_alarm_repeat_Dialog(ui->widget, 5);
    Default_ringtone = new  set_alarm_repeat_Dialog(ui->widget, 4);

    connect(dialog_werk_day->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(werk_day_listClickslot()));
    connect(Time_format->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(Time_format_listClickslot()));
    connect(Pop_up_window->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(Pop_up_window_listClickslot()));
    connect(Reminder_off->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(Reminder_off_listClickslot()));
    connect(Default_ringtone->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(Default_ringtone_listClickslot()));

    repeat_sel = new Btn_new(10, tr("  work"), this);
    Time_sel = new Btn_new(0, tr("  Time"), this);
    Pop_sel = new Btn_new(0, tr("  Pop-up"), this);
    duration_sel = new Btn_new(0, tr("  duration"), this);
    ringtone_sel = new Btn_new(0, tr("  ringtone"), this);
    repeat_sel->move(45, 97);
    Time_sel->move(45, 147);
    Pop_sel ->move(45, 197);
    duration_sel->move(45, 247);
    ringtone_sel->move(45, 297);

    connect(repeat_sel, SIGNAL(clicked()), this, SLOT(werk_day_set()) );
    connect(Time_sel, SIGNAL(clicked()), this, SLOT(Time_format_set()) );
    connect(Pop_sel, SIGNAL(clicked()), this, SLOT(Pop_up_window_set()) );
    connect(duration_sel, SIGNAL(clicked()), this, SLOT(Reminder_off_set()) );
    connect(ringtone_sel, SIGNAL(clicked()), this, SLOT(Default_ringtone_set()) );

    model_setup = new QSqlTableModel(this);
    model_setup->setTable("setup");
    model_setup->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_setup->select();
    model_setup->submitAll();

    repeat_on_Pixmap = QPixmap(":/object-select-symbolic.png");
    repeat_off_Pixmap = QPixmap("");

    QString werk_day;
    QString Time = model_setup->index(0, 15).data().toString();
    QString Pop_up = model_setup->index(0, 16).data().toString();
    QString Reminder = model_setup->index(0, 17).data().toString();
    QString Default = model_setup->index(0, 5).data().toString();

    int werk = 0;
    for (int i=0; i<7; i++) {
        if (model_setup->index(0, i+7).data().toInt()) {
            if(i == 0){
                werk_day = werk_day + tr("Mon");
            }else if(i == 1){
                werk_day = werk_day + tr("Tue");
            }else if(i == 2){
                werk_day = werk_day + tr("Wed");
            }else if(i == 3){
                werk_day = werk_day + tr("Thu");
            }else if(i == 4){
                werk_day = werk_day + tr("Fri");
            }else if(i == 5){
                werk_day = werk_day + tr("Sat");
            }else if(i == 6){
                werk_day = werk_day + tr("Sun");
            }
        }else{
            werk = 1;
        }
    }
    if(Time == "Following system" || "跟随系统"){
        Time = tr("Following system");
    }else if(Time == "24 hour system" || "24小时制(23:59:59)"){
        Time = tr("24 hour system");
    }else if(Time == "12 hour system" || "12小时制(下午11:59:59)"){
        Time = tr("12 hour system");
    }
    if(Pop_up == "Notification" || "通知栏弹窗"){
        Pop_up = tr("Notification");
    }else if(Pop_up == "Full screen" || "全屏弹窗"){
        Pop_up = tr("Full screen");
    }
    if(Reminder == "Alert in 2 minutes" || "2分钟后提醒"){
        Reminder = tr("Alert in 2 minutes");
    }else if(Reminder == "Alert in 5 minutes" || "5分钟后提醒"){
        Reminder = tr("Alert in 5 minutes");
    }else if(Reminder == "Alert in 10 minutes" || "10分钟后提醒"){
        Reminder = tr("Alert in 10 minutes");
    }else if(Reminder == "Alert in 30 minutes" || "30分钟后提醒"){
        Reminder = tr("Alert in 30 minutes");
    }else if(Reminder == "Alert in 60 minutes" || "60分钟后提醒"){
        Reminder = tr("Alert in 60 minutes");
    }
    if(Default == "glass" || "玻璃"){
        Default = tr("glass");
    }else if(Default == "bark" || "犬吠"){
        Default = tr("bark");
    }else if(Default == "sonar" || "声呐"){
        Default = tr("sonar");
    }else if(Default == "drip" || "雨滴"){
        Default = tr("drip");
    }

    if(werk){
        repeat_sel->textLabel->setText(werk_day);
    }else {
        repeat_sel->textLabel->setText(werk_day);
    }
    Time_sel->textLabel->setText(Time);
    Pop_sel->textLabel->setText(Pop_up);
    duration_sel->textLabel->setText(Reminder);
    ringtone_sel->textLabel->setText(Default);

    dialog_werk_day->hide();
    Time_format->hide() ;
    Pop_up_window->hide() ;
    Reminder_off->hide() ;
    Default_ringtone->hide() ;

    QPalette palette1 = ui->pushButton_4->palette();
    QColor ColorPlaceholderText3(255,255,255,0);
    QBrush brush;
    brush.setColor(ColorPlaceholderText3);
    palette1.setBrush(QPalette::Button, brush);
    ui->pushButton_4->setPalette(palette1);
    ui->pushButton_5->setPalette(palette1);
    ui->pushButton_6->setPalette(palette1);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);

    ui->pushButton_4->installEventFilter(this);
    ui->pushButton_5->installEventFilter(this);
    ui->pushButton_6->installEventFilter(this);
}

setuppage::~setuppage()
{
    delete  model_setup;
    delete  dialog_werk_day ;
    delete  Time_format ;
    delete  Pop_up_window ;
    delete  Reminder_off ;
    delete  Default_ringtone ;
    delete  ui;
}

void setuppage::Mute_starting()
{

}
//默认工作日日期设置回调
// Default workday date setting callback
void setuppage::werk_day_set()
{
    //手动构造一个鼠标移走的事件
    //Manually construct an event to move the mouse away
    repeat_sel->setAttribute(Qt::WA_UnderMouse, false);
    QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(40, 40), QPoint(0, 0));
    QCoreApplication::sendEvent(repeat_sel, &hoverEvent);

    dialog_werk_day->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    dialog_werk_day->setAttribute(Qt::WA_TranslucentBackground);
    QPointF position = parentWidget()->pos();
    dialog_werk_day->move(position.x()+83,position.y()+162);
    dialog_werk_day->resize(280,225);
    dialog_werk_day->listWidget->setFixedSize(280,225);
    dialog_werk_day->widget[0]->alarmLabel0->setText(tr("Mon"));
    dialog_werk_day->widget[1]->alarmLabel0->setText(tr("Tue"));
    dialog_werk_day->widget[2]->alarmLabel0->setText(tr("Wed"));
    dialog_werk_day->widget[3]->alarmLabel0->setText(tr("Thu"));
    dialog_werk_day->widget[4]->alarmLabel0->setText(tr("Fri"));
    dialog_werk_day->widget[5]->alarmLabel0->setText(tr("Sat"));
    dialog_werk_day->widget[6]->alarmLabel0->setText(tr("Sun"));

    for (int i=0; i<7; i++) {
        if (model_setup->index(0, i+7).data().toInt()) {
            dialog_werk_day->widget[i]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        } else {
            dialog_werk_day->widget[i]->alarmLabel1->setPixmap(repeat_off_Pixmap);
        }
    }
    dialog_werk_day->show();
}
//工作日选择单击回调
// Weekday select click callback
void setuppage::werk_day_listClickslot()
{
    model_setup->select();
    QString repeat_str;
    int num=dialog_werk_day->listWidget->currentRow();
    QString day[7] ;
    day[0] = tr("Mon");
    day[1] = tr("Tue");
    day[2] = tr("Wed");
    day[3] = tr("Thu");
    day[4] = tr("Fri");
    day[5] = tr("Sat");
    day[6] = tr("Sun");

    switch (num)
    {
    case 0:
        if (model_setup->index(0, 7).data().toInt() ){
            if(model_setup->index(0, 8).data().toInt()==0&&
                    model_setup->index(0, 9).data().toInt()==0&&
                    model_setup->index(0, 10).data().toInt()==0&&
                    model_setup->index(0, 11).data().toInt()==0&&
                    model_setup->index(0, 12).data().toInt()==0&&
                    model_setup->index(0, 13).data().toInt()==0){
            }else{
                model_setup->setData(model_setup->index(0, 7), 0);
                dialog_werk_day->widget[0]->alarmLabel1->setPixmap(repeat_off_Pixmap);
            }
        }else{
            model_setup->setData(model_setup->index(0, 7), 1);
            dialog_werk_day->widget[0]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        }

        break;

    case 1:
        if (model_setup->index(0, 8).data().toInt() ){
            if(model_setup->index(0, 7).data().toInt()==0&&
                    model_setup->index(0, 9).data().toInt()==0&&
                    model_setup->index(0, 10).data().toInt()==0&&
                    model_setup->index(0, 11).data().toInt()==0&&
                    model_setup->index(0, 12).data().toInt()==0&&
                    model_setup->index(0, 13).data().toInt()==0 ){
            }else{
                model_setup->setData(model_setup->index(0, 8), 0);
                dialog_werk_day->widget[1]->alarmLabel1->setPixmap(repeat_off_Pixmap);
            }
        }else{
            model_setup->setData(model_setup->index(0, 8), 1);
            dialog_werk_day->widget[1]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        }

        break;

    case 2:
        if (model_setup->index(0, 9).data().toInt() ){
            if(model_setup->index(0, 8).data().toInt()==0&&
                    model_setup->index(0, 7).data().toInt()==0&&
                    model_setup->index(0, 10).data().toInt()==0&&
                    model_setup->index(0, 11).data().toInt()==0&&
                    model_setup->index(0, 12).data().toInt()==0&&
                    model_setup->index(0, 13).data().toInt()==0 ){
            }else{
                model_setup->setData(model_setup->index(0, 9), 0);
                dialog_werk_day->widget[2]->alarmLabel1->setPixmap(repeat_off_Pixmap);
            }
        }else{
            model_setup->setData(model_setup->index(0, 9), 1);
            dialog_werk_day->widget[2]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        }

        break;

    case 3:
        if (model_setup->index(0, 10).data().toInt() ){
            if(model_setup->index(0, 8).data().toInt()==0&&
                    model_setup->index(0, 9).data().toInt()==0&&
                    model_setup->index(0, 7).data().toInt()==0&&
                    model_setup->index(0, 11).data().toInt()==0&&
                    model_setup->index(0, 12).data().toInt()==0&&
                    model_setup->index(0, 13).data().toInt()==0 ){
            }else{
                model_setup->setData(model_setup->index(0, 10), 0);
                dialog_werk_day->widget[3]->alarmLabel1->setPixmap(repeat_off_Pixmap);
            }
        }else{
            model_setup->setData(model_setup->index(0, 10), 1);
            dialog_werk_day->widget[3]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        }
        break;

    case 4:
        if (model_setup->index(0, 11).data().toInt()) {
            if(model_setup->index(0, 8).data().toInt()==0&&
                    model_setup->index(0, 9).data().toInt()==0&&
                    model_setup->index(0, 10).data().toInt()==0&&
                    model_setup->index(0, 7).data().toInt()==0&&
                    model_setup->index(0, 12).data().toInt()==0&&
                    model_setup->index(0, 13).data().toInt()==0 ){
            }else{
                model_setup->setData(model_setup->index(0, 11), 0);
                dialog_werk_day->widget[4]->alarmLabel1->setPixmap(repeat_off_Pixmap);
            }
        } else {
            model_setup->setData(model_setup->index(0, 11), 1);
            dialog_werk_day->widget[4]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        }
        break;

    case 5:
        if (model_setup->index(0, 12).data().toInt() ) {
            if(model_setup->index(0, 8).data().toInt()==0&&
                    model_setup->index(0, 9).data().toInt()==0&&
                    model_setup->index(0, 10).data().toInt()==0&&
                    model_setup->index(0, 11).data().toInt()==0&&
                    model_setup->index(0, 7).data().toInt()==0&&
                    model_setup->index(0, 13).data().toInt()==0 ){
            }else{
                model_setup->setData(model_setup->index(0, 12), 0);
                dialog_werk_day->widget[5]->alarmLabel1->setPixmap(repeat_off_Pixmap);
            }
        } else {
            model_setup->setData(model_setup->index(0, 12), 1);
            dialog_werk_day->widget[5]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        }
        break;

    case 6:
        if (model_setup->index(0, 13).data().toInt()) {
            if(model_setup->index(0, 8).data().toInt()==0&&
                    model_setup->index(0, 9).data().toInt()==0&&
                    model_setup->index(0, 10).data().toInt()==0&&
                    model_setup->index(0, 11).data().toInt()==0&&
                    model_setup->index(0, 12).data().toInt()==0&&
                    model_setup->index(0, 7).data().toInt()==0 ){
            }else{
                model_setup->setData(model_setup->index(0, 13), 0);
                dialog_werk_day->widget[6]->alarmLabel1->setPixmap(repeat_off_Pixmap);
            }
        } else {
            model_setup->setData(model_setup->index(0, 13), 1);
            dialog_werk_day->widget[6]->alarmLabel1->setPixmap(repeat_on_Pixmap);
        }
        break;

    default:
        break;
    }

    for (int i=0; i<7; i++) {
        if (model_setup->index(0, i+7).data().toInt()) {
            repeat_str=repeat_str+day[i];
        }
    }
    if (model_setup->index(0, 7).data().toInt()&&model_setup->index(0, 8).data().toInt()\
            &&model_setup->index(0, 9).data().toInt()&&model_setup->index(0, 10).data().toInt() \
            &&model_setup->index(0, 11).data().toInt()&&model_setup->index(0, 12).data().toInt() \
            &&model_setup->index(0, 13).data().toInt()) {
        repeat_str = tr("Every day");
    }
    repeat_sel->textLabel->setText(repeat_str);
    model_setup->setData(model_setup->index(0, 14), repeat_str);

    repeat_str="";
    model_setup->submitAll();
}

//时间格式设置回调
// Time format callback
void setuppage::Time_format_set()
{
    Time_sel->setAttribute(Qt::WA_UnderMouse, false);
    QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(40, 40), QPoint(0, 0));
    QCoreApplication::sendEvent(Time_sel, &hoverEvent);

    Time_format->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    Time_format->setAttribute(Qt::WA_TranslucentBackground);
    QPointF position = parentWidget()->pos();
    Time_format->move(position.x()+83,position.y()+212);
    Time_format->resize(280,100);
    Time_format->listWidget->setFixedSize(280,100);
    Time_format->widget[0]->alarmLabel0->setText(tr("Following system"));
    Time_format->widget[1]->alarmLabel0->setText(tr("24 hour system"));
    Time_format->widget[2]->alarmLabel0->setText(tr("12 hour system"));

    Time_format->show();

}

//时间格式选择单击回调
// Time format selection click callback
void setuppage::Time_format_listClickslot()
{
    model_setup->select();

    int num=Time_format->listWidget->currentRow();

    switch (num) {
    case 0:
        model_setup->setData(model_setup->index(0, 2), 0);
        model_setup->setData(model_setup->index(0, 15), tr("Following system"));
        break;

    case 1:
        model_setup->setData(model_setup->index(0, 2), 1);
        model_setup->setData(model_setup->index(0, 15), tr("24 hour system"));
        break;

    case 2:
        model_setup->setData(model_setup->index(0, 2), 2);
        model_setup->setData(model_setup->index(0, 15), tr("12 hour system"));
        break;
    default:
        break;
    }

    Time_sel->textLabel->setText(model_setup->index(0, 15).data().toString());
    Time_format->hide();
    model_setup->submitAll();
}
//弹窗方式设置回调
// Pop up mode setting callback
void setuppage::Pop_up_window_set()
{
    Pop_sel->setAttribute(Qt::WA_UnderMouse, false);
    QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(40, 40), QPoint(0, 0));
    QCoreApplication::sendEvent(Pop_sel, &hoverEvent);

    Pop_up_window->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    Pop_up_window->setAttribute(Qt::WA_TranslucentBackground);;
    QPointF position = parentWidget()->pos();
    Pop_up_window->move(position.x()+83,position.y()+262);
    Pop_up_window->resize(280,75);
    Pop_up_window->listWidget->setFixedSize(280,75);
    Pop_up_window->widget[0]->alarmLabel0->setText(tr("Notification"));
    Pop_up_window->widget[1]->alarmLabel0->setText(tr("Full screen"));
    Pop_up_window->show();
}

//弹窗方式选择单击回调
// Pop up mode select click callback
void setuppage::Pop_up_window_listClickslot()
{
    model_setup->select();
    int num=Pop_up_window->listWidget->currentRow();
    switch (num) {
    case 0:
        model_setup->setData(model_setup->index(0, 3), 0);
        break;

    case 1:
        model_setup->setData(model_setup->index(0, 3), 1);
        break;
    default:
        break;
    }
    if (!model_setup->index(0, 3).data().toInt()) {
        model_setup->setData(model_setup->index(0, 16), tr("Notification"));
    } else {
        model_setup->setData(model_setup->index(0, 16), tr("Full screen"));
    }
    Pop_sel->textLabel->setText(model_setup->index(0, 16).data().toString());
    Pop_up_window->hide();
    model_setup->submitAll();
}

//提醒关闭回调
// Reminder close callback
void setuppage::Reminder_off_set()
{
    duration_sel->setAttribute(Qt::WA_UnderMouse, false);
    QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(40, 40), QPoint(0, 0));
    QCoreApplication::sendEvent(duration_sel, &hoverEvent);

    Reminder_off->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    Reminder_off->setAttribute(Qt::WA_TranslucentBackground);
    QPointF position = parentWidget()->pos();
    Reminder_off->move(position.x()+83,position.y()+312);
    Reminder_off->resize(280,170);
    Reminder_off->listWidget->setFixedSize(280,170);
    Reminder_off->widget[0]->alarmLabel0->setText(tr("Alert in 2 minutes"));
    Reminder_off->widget[1]->alarmLabel0->setText(tr("Alert in 5 minutes"));
    Reminder_off->widget[2]->alarmLabel0->setText(tr("Alert in 10 minutes"));
    Reminder_off->widget[3]->alarmLabel0->setText(tr("Alert in 30 minutes"));
    Reminder_off->widget[4]->alarmLabel0->setText(tr("Alert in 60 minutes"));
    Reminder_off->show();
}

//提醒关闭选择单击回调
// Reminder off select click callback
void setuppage::Reminder_off_listClickslot()
{
    model_setup->select();
    int num=Reminder_off->listWidget->currentRow();
    model_setup->setData(model_setup->index(0, 4), num);

    switch (num) {
    case 0:
        model_setup->setData(model_setup->index(0, 17), tr("Alert in 2 minutes"));
        break;
    case 1:
        model_setup->setData(model_setup->index(0, 17), tr("Alert in 5 minutes"));
        break;
    case 2:
        model_setup->setData(model_setup->index(0, 17), tr("Alert in 10 minutes"));
        break;
    case 3:
        model_setup->setData(model_setup->index(0, 17), tr("Alert in 30 minutes"));
        break;
    case 4:
        model_setup->setData(model_setup->index(0, 17), tr("Alert in 60 minutes"));
        break;
    default:
        break;
    }
    duration_sel->textLabel->setText(model_setup->index(0, 17).data().toString());
    Reminder_off->hide();
    model_setup->submitAll();
}
//默认铃声设置回调
// Default ringtone setting callback
void setuppage::Default_ringtone_set()
{
    ringtone_sel->setAttribute(Qt::WA_UnderMouse, false);
    QHoverEvent hoverEvent(QEvent::HoverLeave, QPoint(40, 40), QPoint(0, 0));
    QCoreApplication::sendEvent(ringtone_sel, &hoverEvent);

    Default_ringtone->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    Default_ringtone->setAttribute(Qt::WA_TranslucentBackground);
    QPointF position = parentWidget()->pos();
    Default_ringtone->move(position.x()+83,position.y()+362);
    Default_ringtone->resize(280,129);
    Default_ringtone->listWidget->setFixedSize(280,129);
    Default_ringtone->widget[0]->alarmLabel0->setText(tr("glass"));
    Default_ringtone->widget[1]->alarmLabel0->setText(tr("bark"));
    Default_ringtone->widget[2]->alarmLabel0->setText(tr("sonar"));
    Default_ringtone->widget[3]->alarmLabel0->setText(tr("drip"));
    Default_ringtone->show();

}

//默认铃声选择单击回调
// Default ring tone selection click callback
void setuppage::Default_ringtone_listClickslot()
{
    model_setup->select();

    int num=Default_ringtone->listWidget->currentRow();

    switch (num)
    {
    case 0:
        model_setup->setData(model_setup->index(0, 5), tr("glass"));
        break;
    case 1:
        model_setup->setData(model_setup->index(0, 5), tr("bark"));
        break;
    case 2:
        model_setup->setData(model_setup->index(0, 5), tr("sonar"));
        break;
    case 3:
        model_setup->setData(model_setup->index(0, 5), tr("drip"));
        break;
    default:
        break;
    }
    ringtone_sel->textLabel->setText(model_setup->index(0, 5).data().toString());
    Default_ringtone->hide();
    model_setup->submitAll();
}


void setuppage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;

    QStyleOption opt;
    opt.init(this);
    painter.setBrush(opt.palette.color(QPalette::Base));

    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 0);
    rect.setHeight(rect.height() - 0);
    painter.drawRoundedRect(rect, 7, 7);
    {
        QPainterPath painterPath;
        painterPath.addRoundedRect(rect, 7, 7);
        painter.drawPath(painterPath);
    }

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(6, 6, -6, -6), 6, 6);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::gray);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(QColor(78,78,78));
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());
    p.setOpacity(0.9);
    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();

    QWidget::paintEvent(event);
}

bool setuppage::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->pushButton_4 && event->type() == QEvent::Paint)
    {
        showPaint(); //响应函数
    }
    if(watched == ui->pushButton_5 && event->type() == QEvent::Paint)
    {
        showPaint1();
    }
    if(watched == ui->pushButton_6 && event->type() == QEvent::Paint)
    {
        showPaint2();
    }

    return QWidget::eventFilter(watched,event);
}

//实现响应函数
void setuppage::showPaint()
{
    QPalette palette = ui->pushButton_4->palette();
    QColor ColorPlaceholderText(248,163,76,255);
    QBrush brush3;
    brush3.setColor(ColorPlaceholderText);
    QStyleOption opt;
    opt.init(this);
    if(QColor(255,255,255) == opt.palette.color(QPalette::Base) || QColor(248,248,248) == opt.palette.color(QPalette::Base))
    {
        palette.setBrush(QPalette::ButtonText, QBrush(QColor(0, 0, 0)));
    }else{
        palette.setBrush(QPalette::ButtonText, QBrush(QColor(255, 255, 255)));
    }

    ui->pushButton_4->setPalette(palette);
}

//实现响应函数
void setuppage::showPaint1()
{
    QPalette palette = ui->pushButton_5->palette();
    QColor ColorPlaceholderText(248,163,76,255);
    QBrush brush3;
    brush3.setColor(ColorPlaceholderText);
    QStyleOption opt;
    opt.init(this);
    if(QColor(255,255,255) == opt.palette.color(QPalette::Base) || QColor(248,248,248) == opt.palette.color(QPalette::Base))
    {
        palette.setBrush(QPalette::ButtonText, QBrush(QColor(0, 0, 0)));
    }else{
        palette.setBrush(QPalette::ButtonText, QBrush(QColor(255, 255, 255)));
    }

    ui->pushButton_5->setPalette(palette);
}

void setuppage::showPaint2()
{
    QPalette palette = ui->pushButton_6->palette();
    QColor ColorPlaceholderText(248,163,76,255);
    QBrush brush3;
    brush3.setColor(ColorPlaceholderText);
    QStyleOption opt;
    opt.init(this);

    if(QColor(255,255,255) == opt.palette.color(QPalette::Base) || QColor(248,248,248) == opt.palette.color(QPalette::Base))
    {
        palette.setBrush(QPalette::ButtonText, QBrush(QColor(0, 0, 0)));
    }else{
        palette.setBrush(QPalette::ButtonText, QBrush(QColor(255, 255, 255)));
    }

    ui->pushButton_6->setPalette(palette);
}
