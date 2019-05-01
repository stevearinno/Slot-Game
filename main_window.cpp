/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: main_window.cpp                                             #
# Description: Implements a UI for the game to be playable.         #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student name: Stefanus Wirdatmadja                                #
# Student number: 232722                                            #
#####################################################################
*/

#include "main_window.hh"
#include <QPixmap>
#include <QString>
#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <utility>
#include <iostream>
#include <QDebug>
#include <QTimer>
#include <QPalette>


// To avoid polluting the global namespace with all kinds of
// names, free functions and global variables / constants are
// usually placed inside a namespace. As we don't have a named
// namespace here, we'll use the anonymous namespace instead.
namespace {

/// \brief Get the fruit symbols and their frequencies.
///
/// \return A map of image names and corresponding images and frequencies.
///
Fruits loadFruits()
{
    // Set frequencies for each symbol.
    // * You can freely play around with the values below.
    // * Values are relative to each other, higher == more common.
    // * Names of these fruits (and berries too) have to actually match
    //   images found on the disk. The images must be in the PNG format.
    const std::map<std::string, int> frequencies{
        {"cherries", 56},
        {"strawberry", 50},
        {"orange", 46},
        {"pear", 39},
        {"apple", 35},
        {"bananas", 30},
        {"tomato", 24},
        {"grapes", 15},
        {"eggplant", 5}
    };

    // Define where the images can be found and what kind of images they are.
    // * Here we assume that the build directory is next to Slots directory.
    const std::string directory("../slots/Resources/");
    const std::string extension(".png");

    // Load the images into QPixmaps and bundle them with their frequencies.
    Fruits symbols;
    for (const std::pair<std::string, int>& frequency : frequencies) {
        const std::string filename = directory + frequency.first + extension;
        const QPixmap image(QString::fromStdString(filename));
        symbols[frequency.first] = std::make_pair(image, frequency.second);
    }
    return symbols;
}

}  // namespace

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    fruits_(loadFruits())
{
    ui_.setupUi(this);
    initUi();
}

MainWindow::~MainWindow()
{
    delete reel1;
    delete reel2;
    delete reel3;
}

void MainWindow::reelStopped(const std::string& middle_sym)
{
    middle_symbols.push_back(middle_sym);
    if (middle_symbols.size() == 3)
    {
        std::string middle_symbol = reel2->reel_symbols[1];
        int sym_weight;
        middle_symbols.clear();
        float initial_money = money_;

        // checks all the possible pay lines, in total there are 5 paylines
        // below is the checking for horizontal pay lines
        for (int row_line = 0; row_line < 3; row_line++)
        {
            if ((reel1->reel_symbols[row_line] ==
                 reel2->reel_symbols[row_line]) &&
                    (reel1->reel_symbols[row_line] ==
                     reel3->reel_symbols[row_line]))
            {
                sym_weight = winning_weight[reel1->reel_symbols[row_line]];
                calculateMoney(sym_weight);
                reelVec1[row_line]->setStyleSheet("background-color : yellow");
                reelVec2[row_line]->setStyleSheet("background-color : yellow");
                reelVec3[row_line]->setStyleSheet("background-color : yellow");
                setMidLabel(row_line, true);
            }
        }

        // below are for the checking of diagonal pay lines
        if ((middle_symbol == reel1->reel_symbols[0]) &&
                (middle_symbol == reel3->reel_symbols[2]))
        {
            sym_weight = winning_weight[reel2->reel_symbols[1]];
            calculateMoney(sym_weight);
            reelVec1[0]->setStyleSheet("background-color : yellow");
            reelVec2[1]->setStyleSheet("background-color : yellow");
            reelVec3[2]->setStyleSheet("background-color : yellow");
        }
        else if ((middle_symbol == reel3->reel_symbols[0]) &&
                (middle_symbol == reel1->reel_symbols[2]))
        {
            sym_weight = winning_weight[reel2->reel_symbols[1]];
            calculateMoney(sym_weight);
            reelVec1[2]->setStyleSheet("background-color : yellow");
            reelVec2[1]->setStyleSheet("background-color : yellow");
            reelVec3[0]->setStyleSheet("background-color : yellow");
        }
        // updates the amount of money after losing
        if (initial_money == money_)
        {
            money_ -= ui_.spin_box->value();
            ui_.money_left->setText(QString::number(money_));
            ui_.info_label->setText("You are not lucky yet. Try spinning "
                                    "again!");

            if (ui_.lock_button1->text() == "UNLOCK" ||
                    ui_.lock_button2->text() == "UNLOCK" ||
                    ui_.lock_button3->text() == "UNLOCK")
                {
                    changeLockButton(ui_.lock_button1, false, true);
                    changeLockButton(ui_.lock_button2, false, true);
                    changeLockButton(ui_.lock_button3, false, true);
                    changeEnableStatus(false);
                }
            else
            {
                changeEnableStatus(true);
            }
        }
        else
        {
            int total_winning = money_ - initial_money;
            ui_.winning_money->setText(QString::number(total_winning) +
                                       " EUR for you");
            changeEnableStatus(false);
        }

        if (money_ == 0)
        {
            ui_.spin_button->setDisabled(true);
            ui_.add_money_button->setDisabled(false);
            ui_.moneyLine->setDisabled(false);
            ui_.spin_box->setValue(0);
            ui_.withdraw_button->setDisabled("true");
            ui_.spin_box->setMaximum(int(ui_.money_left->text().toInt()));
            changeEnableStatus(false);
        }
        else
        {
            ui_.spin_button->setDisabled(false);
        }
    }    
}

void MainWindow::initUi()
{


    // Initialize and display current funds etc.
    this->setWindowTitle("SLOT MACHINE GAME");
    ui_.money_left->setText("0");
    ui_.info_label->setText("Welcome to Slot Game!");

    // Set lock buttons to desired starting state.
    changeLockButton(ui_.lock_button1, false, true);
    changeLockButton(ui_.lock_button2, false, true);
    changeLockButton(ui_.lock_button3, false, true);
    changeEnableStatus(false);

    // Create one random number generator for all Reels to use.
    // * The seed value is obtained via the chrono library and
    //   represents the number of seconds since 1.1.1970.
    // * If you need to debug some pesky error, you can temporarily
    //   replace the seed with some literal. See Minesweeper or
    //   Waterdropgame versions 1 and 2.
    const auto time = std::chrono::high_resolution_clock::now();
    const auto seed = time.time_since_epoch().count();
    auto rng = std::make_shared<std::default_random_engine>(seed);
    money_ = 0;

    // Create each Reel with its own specific labels, etc.
    reelVec1 = {ui_.slot_up1, ui_.slot_mid1, ui_.slot_bot1, ui_.slot_add1};
    reelVec2 = {ui_.slot_up2, ui_.slot_mid2, ui_.slot_bot2, ui_.slot_add2};
    reelVec3 = {ui_.slot_up3, ui_.slot_mid3, ui_.slot_bot3, ui_.slot_add3};

    // aligns the symbols to be in the center of the label
    ui_.slot_up1->setAlignment(Qt::AlignCenter);
    ui_.slot_up2->setAlignment(Qt::AlignCenter);
    ui_.slot_up3->setAlignment(Qt::AlignCenter);
    ui_.slot_mid1->setAlignment(Qt::AlignCenter);
    ui_.slot_mid2->setAlignment(Qt::AlignCenter);
    ui_.slot_mid3->setAlignment(Qt::AlignCenter);
    ui_.slot_bot1->setAlignment(Qt::AlignCenter);
    ui_.slot_bot2->setAlignment(Qt::AlignCenter);
    ui_.slot_bot3->setAlignment(Qt::AlignCenter);
    ui_.slot_add1->setAlignment(Qt::AlignCenter);
    ui_.slot_add2->setAlignment(Qt::AlignCenter);
    ui_.slot_add3->setAlignment(Qt::AlignCenter);
    ui_.winning_money->setAlignment(Qt::AlignCenter);
    // sets the maximum bet based on the money that the player has
    ui_.spin_box->setMaximum(int(ui_.money_left->text().toInt()));
    ui_.spin_button->setDisabled(true);
    ui_.withdraw_button->setDisabled(true);
    // * Create the Reels yourself, nullptr is just a dummy value here.
//    Reel* reel = nullptr;
    reel1= new Reel(reelVec1, ui_.lock_button1, &fruits_, rng);
    reel2= new Reel(reelVec2, ui_.lock_button2, &fruits_, rng);
    reel3= new Reel(reelVec3, ui_.lock_button3, &fruits_, rng);

    // creates hiding label on the top of the reels
    QLabel *hiding_label = new QLabel(this);
    hiding_label->move(18,132);
    hiding_label->setStyleSheet("background-image: "
                                "url(:/MainWindow/Resources/tile.png) ");
    hiding_label->setFixedWidth(230);
    hiding_label->setFixedHeight(50);

    // creates labels between the reels
    vertical_frame1a = new QLabel(this);
    vertical_frame1b = new QLabel(this);
    vertical_frame2a = new QLabel(this);
    vertical_frame2b = new QLabel(this);
    vertical_frame3a = new QLabel(this);
    vertical_frame3b = new QLabel(this);

    // positions the labels between the reels
    vertical_frame1a->move(84,188);    
    vertical_frame1b->move(171,188);    
    vertical_frame2a->move(84,244);    
    vertical_frame2b->move(171,244);    
    vertical_frame3a->move(84,300);    
    vertical_frame3b->move(171,300);

    mid_layer_1 = {vertical_frame1a, vertical_frame1b};
    mid_layer_2 = {vertical_frame2a, vertical_frame2b};
    mid_layer_3 = {vertical_frame3a, vertical_frame3b};

    initializeMidLabel(mid_layer_1);
    initializeMidLabel(mid_layer_2);
    initializeMidLabel(mid_layer_3);

    // sets hiding labels at the bottom of the reels
    ui_.hiding_label2->setStyleSheet("background-image: "
                                     "url(:/MainWindow/Resources/tile.png) ");
    ui_.hiding_label2->setFixedHeight(45);

    connect(reel1, &Reel::stopped, this, &MainWindow::reelStopped);
    connect(reel2, &Reel::stopped, this, &MainWindow::reelStopped);
    connect(reel3, &Reel::stopped, this, &MainWindow::reelStopped);
    connect(ui_.lock_button1, &QPushButton::clicked, this,
            &MainWindow::lockButton);
    connect(ui_.lock_button2, &QPushButton::clicked, this,
            &MainWindow::lockButton);
    connect(ui_.lock_button3, &QPushButton::clicked, this,
            &MainWindow::lockButton);
    
    calculateWinningWeight();
    setInfo();
}

void MainWindow::on_add_money_button_clicked()
{
    QString moneyAdded = ui_.moneyLine->text();
    float moneyAmount = moneyAdded.toFloat();

    // asks the player to insert the money if there is no money in the machine
    if (moneyAmount > 0)
    {
        money_ += int(moneyAmount);
        ui_.moneyLine->setDisabled(true);
        ui_.add_money_button->setDisabled(true);
        ui_.money_left->setText(QString::number(money_));
        ui_.info_label->setText("Money Added! Insert your bet!");
        ui_.spin_box->setMaximum(int(ui_.money_left->text().toInt()));
        ui_.withdraw_button->setDisabled(false);
    }
    else
    {
        ui_.info_label->setText("Please insert your money!");
    }
}


void MainWindow::lockButton()
{
    QPushButton* button = dynamic_cast<QPushButton*>(sender());
    if (isLocked(button))
        changeLockButton(button, false);
    else
        changeLockButton(button, true);
}


void MainWindow::changeLockButton(QPushButton* button, bool lockReel,
                                  bool isFirstRun)
{
    QString buttonNo = "";
    // identifies which button is locked/unlocked
    // not for the initialization/first run
    if (!isFirstRun)
    {
        if (button == ui_.lock_button1)
            buttonNo = "1";
        else if (button == ui_.lock_button2)
            buttonNo = "2";
        else
            buttonNo = "3";
    }

    QPalette pal = button->palette();
    button->setAutoFillBackground(true);
    if (lockReel)
    {
        pal.setColor(QPalette::Button, QColor(Qt::red));

        button->setText("UNLOCK");
        if(!isFirstRun)
            ui_.info_label->setText("Reel " + buttonNo + " is LOCKED");
    }
    else
    {
        pal.setColor(QPalette::Button, QColor(Qt::green));
        button->setText("LOCK");
        if(!isFirstRun)
            ui_.info_label->setText("Reel " + buttonNo + " is UNLOCKED");
    }
    button->setPalette(pal);
    button->update();
}

bool MainWindow::isLocked(QPushButton *button)
{
    if (button->text() == "LOCK")
        return false;
    else
        return true;
}

void MainWindow::calculateWinningWeight()
{
    std::map<std::string, int> winning_bet;

    // gets the fruits and their frequencies
    auto fruitIterator = fruits_.begin();
    int max_weight= 0;
    for(; fruitIterator != fruits_.end(); fruitIterator++)
    {
        winning_bet[fruitIterator->first] = (fruitIterator->second).second;
        if (max_weight < (fruitIterator->second).second)
            max_weight = (fruitIterator->second).second;
    }

    // calculates the weight of each symbols
    auto winIterator = winning_bet.begin();
    std::string fruit_symbol;
    int fruit_weight;
    for(; winIterator != winning_bet.end(); winIterator++)
    {
        fruit_symbol = winIterator->first;
        fruit_weight = winIterator->second;
        winning_weight[fruit_symbol] = (11-(10*fruit_weight/max_weight));
    }
}

void MainWindow::setInfo()
{
    QPixmap apple = fruits_.at("apple").first.scaled(25,25,Qt::KeepAspectRatio);
    QString apple_weight = QString::number(winning_weight.at("apple"))+"x";
    ui_.apple1->setPixmap(apple);
    ui_.apple2->setPixmap(apple);
    ui_.apple3->setPixmap(apple);
    ui_.apple_weight->setText(apple_weight);

    QPixmap bananas = fruits_.at("bananas").first.scaled(25,25,Qt::KeepAspectRatio);
    QString bananas_weight = QString::number(winning_weight.at("bananas"))+"x";
    ui_.bananas1->setPixmap(bananas);
    ui_.bananas2->setPixmap(bananas);
    ui_.bananas3->setPixmap(bananas);
    ui_.bananas_weight->setText(bananas_weight);

    QPixmap cherries = fruits_.at("cherries").first.scaled(25,25,Qt::KeepAspectRatio);
    QString cherries_weight = QString::number(winning_weight.at("cherries"))+"x";
    ui_.cherries1->setPixmap(cherries);
    ui_.cherries2->setPixmap(cherries);
    ui_.cherries3->setPixmap(cherries);
    ui_.cherries_weight->setText(cherries_weight);

    QPixmap eggplant = fruits_.at("eggplant").first.scaled(25,25,Qt::KeepAspectRatio);
    QString eggplant_weight = QString::number(winning_weight.at("eggplant"))+"x";
    ui_.eggplant1->setPixmap(eggplant);
    ui_.eggplant2->setPixmap(eggplant);
    ui_.eggplant3->setPixmap(eggplant);
    ui_.eggplant_weight->setText(eggplant_weight);

    QPixmap grapes = fruits_.at("grapes").first.scaled(25,25,Qt::KeepAspectRatio);
    QString grapes_weight = QString::number(winning_weight.at("grapes"))+"x";
    ui_.grapes1->setPixmap(grapes);
    ui_.grapes2->setPixmap(grapes);
    ui_.grapes3->setPixmap(grapes);
    ui_.grapes_weight->setText(grapes_weight);

    QPixmap orange = fruits_.at("orange").first.scaled(25,25,Qt::KeepAspectRatio);
    QString orange_weight = QString::number(winning_weight.at("orange"))+"x";
    ui_.orange1->setPixmap(orange);
    ui_.orange2->setPixmap(orange);
    ui_.orange3->setPixmap(orange);
    ui_.orange_weight->setText(orange_weight);

    QPixmap pear = fruits_.at("pear").first.scaled(25,25,Qt::KeepAspectRatio);
    QString pear_weight = QString::number(winning_weight.at("pear"))+"x";
    ui_.pear1->setPixmap(pear);
    ui_.pear2->setPixmap(pear);
    ui_.pear3->setPixmap(pear);
    ui_.pear_weight->setText(pear_weight);

    QPixmap strawberry = fruits_.at("strawberry").first.scaled(25,25,Qt::KeepAspectRatio);
    QString strawberry_weight = QString::number(winning_weight.at("strawberry"))+"x";
    ui_.strawberry1->setPixmap(strawberry);
    ui_.strawberry2->setPixmap(strawberry);
    ui_.strawberry3->setPixmap(strawberry);
    ui_.strawberry_weight->setText(strawberry_weight);

    QPixmap tomato = fruits_.at("tomato").first.scaled(25,25,Qt::KeepAspectRatio);
    QString tomato_weight = QString::number(winning_weight.at("tomato"))+"x";
    ui_.tomato1->setPixmap(tomato);
    ui_.tomato2->setPixmap(tomato);
    ui_.tomato3->setPixmap(tomato);
    ui_.tomato_weight->setText(tomato_weight);

    ui_.apple1->setAlignment(Qt::AlignCenter);
    ui_.apple2->setAlignment(Qt::AlignCenter);
    ui_.apple3->setAlignment(Qt::AlignCenter);
    ui_.bananas1->setAlignment(Qt::AlignCenter);
    ui_.bananas2->setAlignment(Qt::AlignCenter);
    ui_.bananas3->setAlignment(Qt::AlignCenter);
    ui_.cherries1->setAlignment(Qt::AlignCenter);
    ui_.cherries2->setAlignment(Qt::AlignCenter);
    ui_.cherries3->setAlignment(Qt::AlignCenter);
    ui_.eggplant1->setAlignment(Qt::AlignCenter);
    ui_.eggplant2->setAlignment(Qt::AlignCenter);
    ui_.eggplant3->setAlignment(Qt::AlignCenter);
    ui_.grapes1->setAlignment(Qt::AlignCenter);
    ui_.grapes2->setAlignment(Qt::AlignCenter);
    ui_.grapes3->setAlignment(Qt::AlignCenter);
    ui_.orange1->setAlignment(Qt::AlignCenter);
    ui_.orange2->setAlignment(Qt::AlignCenter);
    ui_.orange3->setAlignment(Qt::AlignCenter);
    ui_.pear1->setAlignment(Qt::AlignCenter);
    ui_.pear2->setAlignment(Qt::AlignCenter);
    ui_.pear3->setAlignment(Qt::AlignCenter);
    ui_.strawberry1->setAlignment(Qt::AlignCenter);
    ui_.strawberry2->setAlignment(Qt::AlignCenter);
    ui_.strawberry3->setAlignment(Qt::AlignCenter);
    ui_.tomato1->setAlignment(Qt::AlignCenter);
    ui_.tomato2->setAlignment(Qt::AlignCenter);
    ui_.tomato3->setAlignment(Qt::AlignCenter);
}

void MainWindow::calculateMoney(int sym_weight)
{
    int bet = ui_.spin_box->value();

    int winning_money = sym_weight * bet;
    money_ += winning_money;
    ui_.money_left->setText(QString::number(money_));

    // updates the amount of money after winning
    ui_.release_button->click();
    ui_.info_label->setText("YOU.. LUCKY BASTARD! Spin more!");
    changeEnableStatus(false);
    ui_.spin_box->setMaximum(int(ui_.money_left->text().toInt()));
}

void MainWindow::changeEnableStatus(bool to_enable)
{
    if (to_enable)
    {
        ui_.lock_button1->setDisabled(false);
        ui_.lock_button2->setDisabled(false);
        ui_.lock_button3->setDisabled(false);
    }
    else
    {
        ui_.lock_button1->setDisabled(true);
        ui_.lock_button2->setDisabled(true);
        ui_.lock_button3->setDisabled(true);
    }
}

void MainWindow::backgroundClear(std::vector<QLabel *> one_reel)
{
    for (int row = 0; row < 3; row++)
    {
        one_reel[row]->setStyleSheet("");
    }
}

void MainWindow::initializeMidLabel(std::vector<QLabel *> mid_label_vec)
{
    for (unsigned int vec_elm = 0; vec_elm < mid_label_vec.size(); vec_elm++)
    {
        mid_label_vec[vec_elm]->setFixedWidth(13);
        mid_label_vec[vec_elm]->setFixedHeight(50);
    }
}

void MainWindow::setMidLabel(int row, bool turn_to_yellow)
{
    std::vector<QLabel *> mid_label_vec;
    if (row == 0)
    {
        mid_label_vec = mid_layer_1;
    }
    else if (row == 1)
    {
        mid_label_vec = mid_layer_2;
    }
    else
    {
        mid_label_vec = mid_layer_3;
    }

    QString back_color;
    if (turn_to_yellow)
    {
        back_color = "background-color:yellow";
    }
    else
    {
        back_color = "background-image: url(:/MainWindow/Resources/tile.png)";
    }

    for (unsigned int vec_elm = 0; vec_elm < mid_label_vec.size(); vec_elm++)
    {
        mid_label_vec[vec_elm]->setStyleSheet(back_color);
    }
}

void MainWindow::on_release_button_clicked()
{
    changeLockButton(ui_.lock_button1, false);
    changeLockButton(ui_.lock_button2, false);
    changeLockButton(ui_.lock_button3, false);
    ui_.info_label->setText("All reels are UNLOCKED!");
}

void MainWindow::on_spin_button_clicked()
{
    backgroundClear(reelVec1);
    backgroundClear(reelVec2);
    backgroundClear(reelVec3);
    setMidLabel(0, false);
    setMidLabel(1, false);
    setMidLabel(2, false);
    if (ui_.lock_button1->text() == "LOCK" ||
            ui_.lock_button2->text() == "LOCK" ||
            ui_.lock_button3->text() == "LOCK")
    {
        ui_.spin_button->setDisabled(true);
        ui_.winning_money->setText("");
        changeEnableStatus(true);
        ui_.info_label->setText("SPINNING...");
        reel1->spin();
        reel2->spin();
        reel3->spin();
        changeEnableStatus(false);
    }
    else
    {
        ui_.info_label->setText("Don't lock all the reels");
    }
}

void MainWindow::on_spin_box_valueChanged(int arg1)
{
    if (arg1 == 0)
    {
        ui_.spin_button->setDisabled(true);
        if (ui_.add_money_button->isEnabled() == false)
            ui_.info_label->setText("Insert your bet!");
        else
            ui_.info_label->setText("Insert money to continue playing!");
    }
    else
    {
        ui_.spin_button->setDisabled(false);
        ui_.info_label->setText("Start spinning!");
    }
}

void MainWindow::on_withdraw_button_clicked()
{
    ui_.spin_button->setDisabled(true);
    ui_.add_money_button->setDisabled(false);
    ui_.moneyLine->setDisabled(false);
    ui_.spin_box->setValue(0);
    ui_.info_label->setText("DISPENSE " + ui_.money_left->text() +
                            " EUR. PLEASE TAKE YOUR MONEY!");
    ui_.moneyLine->setText("");
    ui_.money_left->setText("0");
    ui_.withdraw_button->setDisabled(true);
    money_ = 0;
}
