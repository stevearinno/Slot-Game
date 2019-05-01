/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: reel.h                                                      #
# Description: Draws random fruits for labels to display.           #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student name: Stefanus Wirdatmadja                                #
# Student number: 232722                                            #
#####################################################################
*/

#ifndef SLOTS_REEL_H
#define SLOTS_REEL_H

#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QPushButton>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <QTimer>

// Fruits is an alias for a map which contains a fruit's name and
// corresponding image and likelyhood of appearance.
using Fruits = std::map<std::string, std::pair<QPixmap, int>>;


/// \class Reel
/// \brief Implements reels which are used to determine if the player has won.
///
/// Reel spins for a random duration and draws random fruits to display on its
/// labels. Reel signals MainWindow with the name of the middlemost fruit, but
/// an ordered (top-to-bottom) list of all fruits can also be queried.
///
class Reel: public QObject {
    Q_OBJECT

public:

    /// \brief Construct a new Reel.
    ///
    /// \param[in] labels Labels which should be used to display the symbols.
    /// \param[in] lock_button Button whose state determines if Reel is locked.
    /// \param[in] fruits Images and frequencies of each fruit.
    /// \param[in] rng Random number generator used to determine the symbols.
    ///
    Reel(const std::vector<QLabel*>& labels,
         const QPushButton* lock_button,
         const Fruits* fruits,
         std::shared_ptr<std::default_random_engine> rng);
    void setPictures();                     // sets the symbols of each reel
    std::vector<std::string> reel_symbols;  // saves the symbols of each reel
    void spin();                            // spins the reel

    ~Reel();

signals:

    /// \brief Is emitted when the Reel stops.
    ///
    /// \param[in] Name of the middle symbol on the Reel.
    ///

    // sends the middle symbol of the middle reel when it stops
    void stopped(const std::string& middle_sym);

private slots:
    // animates the moving reel
    void movingPicture();
    // when the generated reel time is finished, the reel will stop
    void stopSpinning();

private:
    // saves symbols in a vector for each reel
    void saveSymbol(std::string symbol);

    std::vector<std::string> fruitVector;
    std::vector<int> weights;
    const std::vector<QLabel*> labels_;
    const QPushButton* lock_button_;
    const Fruits* fruits_;
    std::shared_ptr<std::default_random_engine> rng_;
    QTimer* reel_timer;         // timer for spinning reel
    QTimer* timer;              // timer for one period of reel movement
    int timer_value;            // the random time for reel to spin

};  // class Reel


#endif  // SLOTS_REEL_H
