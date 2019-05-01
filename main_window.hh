/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: main_window.h                                               #
# Description: Implements a UI for the game to be playable.         #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student name: Stefanus Wirdatmadja                                #
# Student number: 232722                                            #
#####################################################################
*/

#ifndef SLOTS_MAINWINDOW_H
#define SLOTS_MAINWINDOW_H

#include "reel.hh"
#include "ui_main_window.h"
#include <QMainWindow>
#include <string>
#include <QTimer>


/// \class MainWindow
/// \brief Implements the main window which is used to interact with the game.
///
/// MainWindow owns all the widgets used to display the game to the player
/// and to play the game. MainWindow contains the whole game, no other windows
/// are used.
///
class MainWindow: public QMainWindow {
    Q_OBJECT

public:

    /// \brief Construct a new MainWindow.
    ///
    /// \param[in] parent Points to this MainWindow's parent widget.
    ///
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow();



private slots:

    /// \brief Check if the round is over and prepare for the next round.
    ///
    /// Should get called when a Reel has decided on which fruits to display.
    ///
    /// \param[in] middle_sym Name of the fruit symbol on the middle lane.
    ///
    void reelStopped(const std::string& middle_sym);
    // adds the money that the player inputs
    void on_add_money_button_clicked();
    // locks the corresponding reel
    void lockButton();
    // unlocks all the reels
    void on_release_button_clicked();
    // spins the reels
    void on_spin_button_clicked();
    // checks the player's bet, it should be more than 0
    void on_spin_box_valueChanged(int arg1);
    // withdraws all the money from the machine
    void on_withdraw_button_clicked();

private:

    /// \brief Initialize the Reels, buttons, etc. so the game can be played.
    ///
    void initUi();
    // locks and unlocks reel. The inputs are [in 1] the button which indicates
    // the corresponding reel, [in 2] if the player wants to lock the rell,
    // and [in 3] if the info label should be updated or not.
    void changeLockButton(QPushButton* button, bool lockReel,
                          bool isFirstRun=false);
    // checks if the reel is locked
    bool isLocked(QPushButton* button);
    // calculates the weight of each symbol for winning purpose
    void calculateWinningWeight();
    // shows the scoring/weight info
    void setInfo();
    // calculates the money after wining
    void calculateMoney(int sym_weight);
    // changes the status of all lock buttons enabled/disabled
    void changeEnableStatus(bool to_enable);
    // saves all the middle symbols of all reels
    void backgroundClear(std::vector<QLabel*> one_reel);
    std::vector<std::string> middle_symbols;
    // initilizing the area between the reel (cosmetics purpose)
    void initializeMidLabel(std::vector<QLabel*> mid_label_vec);
    // turns the area between the reel to yellow if needed (if win)
    void setMidLabel(int row, bool turn_to_yellow);

    const Fruits fruits_;       ///< The game symbols and their rarities.
    Ui::MainWindowClass ui_;    ///< (Black) Qt magic handling the UI.
    float money_;                       // amount of money that the player has
    QTimer* timer;                      // the timer for looping welcome text
    // initializes the vector of symbols of each reel
    std::vector<QLabel*> reelVec1;
    std::vector<QLabel*> reelVec2;
    std::vector<QLabel*> reelVec3;
    // initializes the reels
    Reel* reel1;
    Reel* reel2;
    Reel* reel3;
    // save symbols and their weights
    std::map<std::string, int> winning_weight;
    // initializes the labels for the area between the reels
    QLabel *vertical_frame1a;
    QLabel *vertical_frame1b;
    QLabel *vertical_frame2a;
    QLabel *vertical_frame2b;
    QLabel *vertical_frame3a;
    QLabel *vertical_frame3b;
    std::vector<QLabel*> mid_layer_1;
    std::vector<QLabel*> mid_layer_2;
    std::vector<QLabel*> mid_layer_3;

};  // class MainWindow


#endif  // SLOTS_MAINWINDOW_H
