// collect_plugins.cpp
#include <fstream>      // это Ксюша
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Подключаем filesystem с запасным вариантом для старых компиляторов
#if __has_include(<filesystem>)
#  include <filesystem>
   namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#  include <experimental/filesystem>
   namespace fs = std::experimental::filesystem;
#else
#  error "Compiler does not support <filesystem>"
#endif

// Удаление расширения у имени файла
std::string remove_extension(const std::string& str) {
    size_t pos = str.find_last_of('.');
    if (pos != std::string::npos)
        return str.substr(0, pos);
    return str;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <plugin1.cpp> [plugin2.cpp ...]\n";
        return 1;
    }
    const std::string path_plugins="plugins/";
    // Собираем уникальные имена файлов
    std::vector<std::string> plugins;
    for (int i = 1; i < argc; ++i) {
        fs::path p(argv[i]);
        plugins.push_back(path_plugins+p.string());
    }

    if (plugins.empty()) {
        std::cerr << "No valid plugin files provided.\n";
        return 1;
    }

    // Генерируем plugins.cpp
    const std::string output_path = "../controller/assembler_controller/plugins.tpp";
    std::ofstream file(output_path, std::ios::trunc);
    if (!file) {
        std::cerr << "Failed to create " << output_path << "\n";
        return 1;
    }

    file << "// ============================================================\n"
         << "//  AUTOMATICALLY GENERATED FILE\n"
         << "//  DO NOT EDIT MANUALLY\n"
         << "// ============================================================\n\n"
         << "#include <string>\n"
         << "#include \"plugins_controller.hpp\"\n";

    // Включаем все переданные файлы
    for (const auto& p : plugins) {
        file << "#include \"" << p << "\"\n";
    }

    file << "template<typename T,typename Func>\n"
         << "void CollectPlug::collect(Func func, const std::string plugin_name) {\n";

    // Генерируем цепочку if-else
    for (size_t i = 0; i < plugins.size(); ++i) {
        fs::path p(plugins[i]);
        std::string name = p.stem().string();   // имя без пути и расширения
        if (i == 0) {
            file << "    if (plugin_name == \"" << name << ".cpp\" || plugin_name == \"" << name << "\") "
                 << "func(" << name << "<T>());\n";
        } else {
            file << "    else if (plugin_name == \"" << name << ".cpp\" || plugin_name == \"" << name << "\") "
                 << "func(" << name << "<T>());\n";
        }
    }
    file << "    }\n";

    file.close();
    std::cout << "Generated " << output_path << " with " << plugins.size() << " plugin(s).\n";
    return 0;
}
