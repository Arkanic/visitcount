#include <iostream>
#include <string>
#include "crow.h"
#include "filenumbersync.hpp"
#include "nonce.hpp"

#define BEHIND_PROXY false
#define DEVHOST true

std::string request_ip(const crow::request &req) {
    if(BEHIND_PROXY) {
        return req.get_header_value("X-Forwarded-For");
    } else {
        return req.remote_ip_address;
    }
}

int main(int argc, char **argv) {
    if(argc < 3) {
        std::cout << "Not enough args!\nUsage: " << std::quoted(argv[0]) << " <file> <port>\n";
        std::exit(1);
    }

    std::string syncpath = std::format("{}", argv[1]);
    auto views = std::make_shared<FileNumberSync>(syncpath);
    auto nonce = std::make_shared<Nonce>(4); // last 3 hex 0

    crow::SimpleApp app;

    if(DEVHOST) {
        app.static_file("/", "web/test.html");
        app.static_file("/hash.js", "web/hash.js");
        app.static_file("/visitcount.js", "web/visitcount.js");
    }

    CROW_ROUTE(app, "/increment").methods("POST"_method)([views, nonce](const crow::request &req) {
        crow::json::wvalue response({
            {"success", false},
            {"message", "bad request"}
        });

        auto reqjson = crow::json::load(req.body);
        if(!reqjson || !reqjson.has("nonce")) return crow::response(400, response);
        std::string rawNonce = reqjson["nonce"].s();
        if(rawNonce.size() > NONCE_INPUT_MAXSIZE) return crow::response(400, response);

        bool nonceCorrect = nonce->validateResponse(rawNonce);
        if(!nonceCorrect) {
            response["message"] = "invalid nonce";
            return crow::response(400, response);
        }

        // they did it!
        views->increment();

        response["success"] = true;
        response["message"] = "";
        
        return crow::response(200, response);
    });

    CROW_ROUTE(app, "/count")([views, nonce]() {
        uint64_t count = views->value();
        crow::json::wvalue response({
            {"count", count},
            {"challenge", nonce->issueChallenge()},
            {"difficulty", nonce->getDifficulty()}
        });
        
        return response;
    });

    std::string portstr = std::format("{}", argv[2]);
    app.port(std::stoi(portstr)).multithreaded().run();

    static auto s_views = views;
    std::atexit([]() {
        std::cout << "Exiting...\n";
        s_views->forceSave();
    });
}