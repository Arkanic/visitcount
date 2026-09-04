#include <iostream>
#include <string>
#include "crow.h"
#include "filenumbersync.hpp"
#include "nonce.hpp"

#define BEHIND_PROXY false

std::string request_ip(const crow::request &req) {
    if(BEHIND_PROXY) {
        return req.get_header_value("X-Forwarded-For");
    } else {
        return req.remote_ip_address;
    }
}

int main() {
    auto views = std::make_shared<FileNumberSync>("./views.txt");
    auto nonce = std::make_shared<Nonce>(4); // 65536

    crow::SimpleApp app;

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

    app.port(8080).multithreaded().run();
}