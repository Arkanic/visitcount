#include <iostream>
#include <string>
#include "crow.h"
#include "filenumbersync.hpp"

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

    crow::SimpleApp app;

    CROW_ROUTE(app, "/increment")([](const crow::request &req) {
        crow::json::wvalue response({
            {"success", true},
            {"views", 843776}
        });
        response["success"] = false;
        response["ip"] = request_ip(req);
        
        return response;
    });

    CROW_ROUTE(app, "/count")([views]() {
        uint64_t count = views->value();
        crow::json::wvalue response({
            {"count", count}
        });
        
        return response;
    });

    app.port(8080).multithreaded().run();
}