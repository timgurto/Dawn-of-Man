// (C) 2010 Tim Gurto

#ifndef DAWN_H
#define DAWN_H

class ScreenElement;
class Screen;

void buildScreens(Screen &mainMenu,
                  Screen &credits);

//for adding elements to the credits screen
void addCreditTitle(Screen &creditsScreen, int &yPos, std::string text);
void addCredit(Screen &creditsScreen, int &yPos, std::string text);
void addCreditGap(int &yPos);

//plays through each level in the campaign
void playCampaign(Screen &game);

#endif