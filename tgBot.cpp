#include <stdio.h>
#include <tgbot/tgbot.h>
#include<random>
#define SQLITECPP_COMPILE_DLL
#include<SQLiteCpp/SQLiteCpp.h>
SQLite::Database db("example.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);


struct GameSession {
    int guessedNumber;
    bool gameStarted;
};


int main() {
    std::map<int64_t, GameSession> gameSessions;
    TgBot::Bot bot("7293840848:AAGKON39UvM-kfQBVJGGhOB6_3HBDedsnmM");
    //
   TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
        button1->text = "Author";
        button1->callbackData = "Author";

        TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
        button2->text = "Rules";
        button2->callbackData = "Rules";


        TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
        button3->text = "Functions";
        button3->callbackData = "Functions";

        TgBot::InlineKeyboardButton::Ptr button4(new TgBot::InlineKeyboardButton);
        button4->text = "Contact";
        button4->url = "https://t.me/Last_Kinggg";

        TgBot::InlineKeyboardButton::Ptr button10(new TgBot::InlineKeyboardButton);
        button10->text = "Help";
        button10->callbackData = "Help";
       
        keyboard->inlineKeyboard.push_back({ button1,button2 });
        keyboard->inlineKeyboard.push_back({ button3 });
        keyboard->inlineKeyboard.push_back({ button10 });
        keyboard->inlineKeyboard.push_back({ button4 });


        //
        TgBot::InlineKeyboardMarkup::Ptr keyboardtwo(new TgBot::InlineKeyboardMarkup);

        TgBot::InlineKeyboardButton::Ptr button5(new TgBot::InlineKeyboardButton);
        button5->text = "id";
        button5->callbackData = "id";

        TgBot::InlineKeyboardButton::Ptr button6(new TgBot::InlineKeyboardButton);
        button6->text = "game";
        button6->callbackData = "game";

        keyboardtwo->inlineKeyboard.push_back({ button5,button6 });




        //
        TgBot::InlineKeyboardMarkup::Ptr keyboardthree(new TgBot::InlineKeyboardMarkup);

        TgBot::InlineKeyboardButton::Ptr button9(new TgBot::InlineKeyboardButton);
        button9->text = "exit";
        button9->callbackData = "exit";

        keyboardthree->inlineKeyboard.push_back({ button9 });



  
  


    bot.getEvents().onCommand("start", [&bot, &keyboard](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi, i'm your personal bot. I was writen in c++ language! Tap Functions to know about my oppotrunities.", false,0,keyboard);
        try {
            SQLite::Statement query(db, "SELECT COUNT(*) FROM users WHERE idTg = :userId");
            query.bind(":userId", message->chat->id);

            if (query.executeStep()) {
                int count = query.getColumn(0).getInt();
                if (count > 0) {
                    std::cout << "ID " << message->chat->id << " found." << std::endl;
                }
                else {
                    std::cout << "ID " << message->chat->id << " not found." << std::endl;

                    SQLite::Statement query(db, "INSERT INTO users (idTg) VALUES (?)");
                    query.bind(1, message->chat->id);
                    query.exec();
                }


            }

            
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
       

   });

    bot.getEvents().onCommand("id", [&bot, &keyboard](TgBot::Message::Ptr message) {
        std::string id = std::to_string(message->chat->id);
        bot.getApi().sendMessage(message->chat->id, "Your Tg id is: " + id, false, 0, keyboard);
        });
    bot.getEvents().onCommand("functions", [&bot, &keyboard, &keyboardtwo](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Functions:\n/id -I can show your Tg id\n/game -guessed number\n", false, 0, keyboardtwo);
        });
   

    bot.getEvents().onCommand("game", [&bot, &gameSessions, &keyboardthree](TgBot::Message::Ptr message) {
        if (!gameSessions[message->chat->id].gameStarted) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(1, 50);
            gameSessions[message->chat->id].guessedNumber = distrib(gen);
            gameSessions[message->chat->id].gameStarted = true;

            bot.getApi().sendMessage(message->chat->id, "Hi! I guessed number from 1 to 50.Try to guess it! Use /exit, to finish the game.", false, 0, keyboardthree);
        }
        else {
            bot.getApi().sendMessage(message->chat->id, "Game has already started! Guess the number.", false, 0, keyboardthree);
        }
        });



   bot.getEvents().onCommand("exit", [&bot, &gameSessions, &keyboard](TgBot::Message::Ptr message) {
        if (gameSessions[message->chat->id].gameStarted) {
            gameSessions[message->chat->id].gameStarted = false;
            bot.getApi().sendMessage(message->chat->id, "You exit the game.", false, 0, keyboard);
        }
        else {
            bot.getApi().sendMessage(message->chat->id, "Game hasn't started.", false, 0, keyboard);
        }
        });

    bot.getEvents().onAnyMessage([&bot, &gameSessions, &keyboard ](TgBot::Message::Ptr message) {
        std::cout << message->chat->id << ':';
        printf(" User wrote %s\n", message->text.c_str());

        if (StringTools::startsWith(message->text, "/start") ||
            StringTools::startsWith(message->text, "/id") ||
            StringTools::startsWith(message->text, "/functions") ||
            StringTools::startsWith(message->text, "/game") ||
            StringTools::startsWith(message->text, "/exit")) {
            return; 
        }


        if (gameSessions[message->chat->id].gameStarted) {
            try {
                int userGuess = std::stoi(message->text);
                if (userGuess == gameSessions[message->chat->id].guessedNumber) {
                    gameSessions[message->chat->id].gameStarted = false; // Завершаем игру
                    bot.getApi().sendMessage(message->chat->id, "Right! You guessed the number!", false, 0, keyboard);
                }
                else if (userGuess < gameSessions[message->chat->id].guessedNumber) {
                    bot.getApi().sendMessage(message->chat->id, "Wrong! My number is greater.");
                }
                else {
                    bot.getApi().sendMessage(message->chat->id, "Wrong! My number is less.");
                }
            }
            catch (std::exception& e) { // Обработка ошибки преобразования в int
                bot.getApi().sendMessage(message->chat->id, "Enter the number!");
            }
        }
        else {
           // bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        }
        });


    bot.getEvents().onCallbackQuery([&bot, &keyboard,&keyboardtwo, &gameSessions, &keyboardthree](TgBot::CallbackQuery::Ptr query) {
        std::string message = "Received callback query: " + query->data;
        if (query->data == "Author") {
            bot.getApi().sendMessage(query->from->id, u8"Гений, миллиардер, филантроп.", false, 0, keyboard);
            std::cout << query->from->id << ':';
            printf(" User tap Author\n");
        }
       else if(query->data == "Rules"){
            bot.getApi().sendMessage(query->from->id, u8"Не спамить. Не на команды бот не отвечает.", false, 0, keyboard);
            std::cout << query->from->id << ':';
            printf(" User tap Rules\n");
        }
       else if (query->data == "Help") {
            bot.getApi().sendMessage(query->from->id, u8"Помимо кнопок можно пользоваться командами:\n/start /functions /id /game /exit");
            std::cout << query->from->id << ':';
            printf(" User tap Help\n");
        }
       else if (query->data == "Functions") {
            bot.getApi().sendMessage(query->from->id, u8"Functions:", false, 0, keyboardtwo);
            std::cout << query->from->id << ':';
            printf(" User tap Functions\n");
        }
        else if (query->data == "id") {
            std::string id = std::to_string(query->from->id);
            bot.getApi().sendMessage(query->from->id, "Your Tg id is: " + id, false, 0, keyboard);
            std::cout << query->from->id << ':';
            printf(" User tap id\n");
        }
        else if (query->data == "game") {
            
            if (!gameSessions[query->from->id].gameStarted) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(1, 50);
                gameSessions[query->from->id].guessedNumber = distrib(gen);
                gameSessions[query->from->id].gameStarted = true;

                bot.getApi().sendMessage(query->from->id, "Hi! I guessed number from 1 to 50.Try to guess it! Use exit, to finish the game.",false,0, keyboardthree);
            }
            else {
                bot.getApi().sendMessage(query->from->id, "Game has already started! Guess the number.");
            }
            std::cout << query->from->id << ':';
            printf(" User tap game\n");
        }
        else if (query->data == "exit") {
            
            if (gameSessions[query->from->id].gameStarted) {
                gameSessions[query->from->id].gameStarted = false;
                bot.getApi().sendMessage(query->from->id, "You exit the game.", false, 0, keyboard);
            }
            else {
                bot.getApi().sendMessage(query->from->id, "Game hasn't started.");
            }
            std::cout << query->from->id << ':';
            printf(" User tap exit\n");
          //  bot.getApi().sendMessage(query->from->id, u8"game exit", false, 0, keyboardthree);
        }



        });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }






    return 0;
}
