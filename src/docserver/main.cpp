//do not remove the include below
#include <winsock2.h>

#include "../bond.h"
#include <cstdint>
#include <httplib.h>
#include "htmx.h"
#include "../engine.h"


auto main_page() -> t_string {
    return R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>docs</title>
    <script src="/htmx"></script>
</head>
<body>
    <div hx-get="/hello" hx-trigger="click" hx-target="#hello"></div>
    <div id="hello">help-me</div>
</body>
</html>
    )";
}


namespace bond{
    auto doc_for_module(const GcPtr<Module>& mod) -> t_string {
       return "";
    }
}

int main(int32_t argc, char **argv) {
#ifdef _WIN32
    auto lib_path = std::filesystem::path(bond::get_exe_path()).parent_path().string() + "/../libraries/";

    if (!std::filesystem::exists(lib_path)) {
        lib_path = std::filesystem::path(bond::get_exe_path()).parent_path().string() + "/";
    }

    fmt::print("lib_path: {}\n", lib_path);
#else
    auto lib_path = "/usr/local/libraries/bond/";
#endif
    auto args =
            std::vector<std::string, gc_allocator<std::string>>(argv, argv + argc);

    auto engine = bond::create_engine(lib_path, args);
    engine->set_checker(false);


    httplib::Server svr;

    svr.Get("/htmx", [](const httplib::Request & /*req*/, httplib::Response &res) {
        res.set_content(htmx_source, "text/javascript");
    });

    svr.Get("/", [&](const httplib::Request & /*req*/, httplib::Response &res) {
        res.set_content(main_page().c_str(), "text/html");
    });

    svr.listen("localhost", 8080);

    bond::Runtime::ins()->exit();
    return 0;
}