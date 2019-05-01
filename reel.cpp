/*
#####################################################################
# TIE-02207 Programming 2: Basics, F2018                            #
# Project4: Slots                                                   #
# Program description: Implements a virtual slots game.             #
# File: reel.cpp                                                    #
# Description: Draws random fruits for labels to display.           #
# Notes: * This is an exercise program.                             #
#        * Students need to modify this file.                       #
# Student name: Stefanus Wirdatmadja                                #
# Student number: 232722                                            #
#####################################################################
*/

#include "reel.hh"


Reel::Reel(const std::vector<QLabel*>& labels,
           const QPushButton* lock_button,
           const Fruits* fruits,
           std::shared_ptr<std::default_random_engine> rng):
    labels_(labels), lock_button_(lock_button), fruits_(fruits), rng_(rng)
{
    // Connect all signals & slots you need here.
    auto fruitIterator = fruits_->begin();
    for(;fruitIterator!=fruits_->end(); fruitIterator++)
    {
        fruitVector.push_back(fruitIterator->first);
        weights.push_back((fruitIterator->second).second);
    }

    setPictures();
    reel_timer = new QTimer(this);
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(movingPicture()));
    connect(reel_timer, SIGNAL(timeout()),this, SLOT(stopSpinning()));
}

void Reel::setPictures()
{
    if (lock_button_->text() == "LOCK")
    {
        // Setup the weights (in this case linearly weighted)
        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        for (unsigned int reelElement = 0; reelElement < labels_.size();
             reelElement++)
        {
            int random_value = dist(*rng_);
            labels_[reelElement]->
                    setPixmap(fruits_->at(fruitVector[random_value]).
                              first.scaled(50,50,Qt::KeepAspectRatio));
            saveSymbol(fruitVector[random_value]);
        }
    }
}

void Reel::spin()
{
    timer_value = rand()% 2000 + 1000;
    reel_timer->setSingleShot(true);
    reel_timer->start(timer_value);
    timer->start(2);
}

Reel::~Reel()
{
    delete reel_timer;
    delete timer;
}

void Reel::saveSymbol(std::string symbol)
{
    // for initializing/clearing the vector in each spin
    if (reel_symbols.size() == 4)
        reel_symbols.clear();
    reel_symbols.push_back(symbol);
}

void Reel::movingPicture()
{
    if (lock_button_->text() == "LOCK")
    {
        for (int index = 0; index < 4; index++)
        {
            qreal xx = labels_[index]->x();
            qreal yy = labels_[index]->y();
            yy += 1;
            labels_[index]->move(xx,yy);

            // checks if the the symbol is already on the final position
            if ((yy == 184) && (index == 3))
            {

                labels_[2]->setPixmap(fruits_->at(reel_symbols[1]).
                                      first.scaled(50,50,Qt::KeepAspectRatio));
                reel_symbols[2] = reel_symbols[1];
                labels_[1]->setPixmap(fruits_->at(reel_symbols[0]).
                                      first.scaled(50,50,Qt::KeepAspectRatio));
                reel_symbols[1] = reel_symbols[0];
                labels_[0]->setPixmap(fruits_->at(reel_symbols[3]).
                                      first.scaled(50,50,Qt::KeepAspectRatio));
                reel_symbols[0] = reel_symbols[3];

                // generates random symbol for the upcoming picture
                std::discrete_distribution<int> dist(weights.begin(),
                                                     weights.end());
                int random_value = dist(*rng_);
                labels_[3]->setPixmap(fruits_->at(fruitVector[random_value]).
                                      first.scaled(50,50,Qt::KeepAspectRatio));

                reel_symbols[3] = fruitVector[random_value];
            }
        }
    }
}

void Reel::stopSpinning()
{
    timer->stop();
    movingPicture();    // makes sure that the symbols stop at correct position
    emit stopped(reel_symbols[1]);
}
