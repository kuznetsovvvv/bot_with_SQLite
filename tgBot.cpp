#include <stdio.h>
#include <tgbot/tgbot.h>
#define SQLITECPP_COMPILE_DLL
#include<SQLiteCpp/SQLiteCpp.h>
SQLite::Database db("example.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

int main() {
   // db.exec("CREATE TABLE person (id INTEGER PRIMARY KEY, name TEXT, age INT)"); //создание таблицы заново, но нужно создать только 1 раз

  /*  SQLite::Statement query(db, "INSERT INTO users (idTg) VALUES (?)");
    query.bind(1, 123);
    query.exec();*/

   /* SQLite::Statement query_select(db, "SELECT * FROM person");
    while (query_select.executeStep()) {
        std::cout << "ID: " << query_select.getColumn(0) << ", Name: " << query_select.getColumn(1) << ", Age: " << query_select.getColumn(2) << std::endl;

    }*/

   // return 0;

    TgBot::Bot bot("7293840848:AAGKON39UvM-kfQBVJGGhOB6_3HBDedsnmM");
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi, i'm your personal bot. I was writen in c++ language! Write /functions to know about my oppotrunities");
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
    bot.getEvents().onCommand("id", [&bot](TgBot::Message::Ptr message) {
        std::string id = std::to_string(message->chat->id);
        bot.getApi().sendMessage(message->chat->id, "Your Tg id is: " + id);
        });
    bot.getEvents().onCommand("functions", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Functions:\n/id -I can show your Tg id\n");
        });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        std::cout << message->chat->id << ':';

        printf(" User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        if (StringTools::startsWith(message->text, "/id")) {
            return;
        }
        if (StringTools::startsWith(message->text, "/functions")) {
            return;
        }

        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
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