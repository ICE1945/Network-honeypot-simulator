#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

const int PORT = 5000;
const int BUFFER_SIZE = 2048;
const char* LOG_FILE = "honeypot.log";

// ===== Helper functions =====

std::string current_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return buf;
}

void log_event(const std::string& text) {
    std::ofstream log(LOG_FILE, std::ios::app);
    log << text << std::endl;
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \r\n\t");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \r\n\t");
    return s.substr(start, end - start + 1);
}

// ===== Fake filesystem =====

std::map<std::string, std::vector<std::string>> build_fs() {
    std::map<std::string, std::vector<std::string>> fs;
    fs["/"]          = {"bin", "home", "etc", "var", "tmp"};
    fs["/bin"]       = {"ls", "cat", "bash"};
    fs["/home"]      = {"user"};
    fs["/home/user"] = {"notes.txt", ".ssh"};
    fs["/home/user/.ssh"] = {"id_rsa"};
    fs["/etc"]       = {"passwd", "hosts"};
    fs["/var"]       = {"log"};
    fs["/tmp"]       = {};
    return fs;
}

bool is_directory(const std::map<std::string, std::vector<std::string>>& fs,
                  const std::string& path) {
    return fs.find(path) != fs.end();
}

std::string join_path(const std::string& cwd, const std::string& arg) {
    if (arg.empty()) return cwd;
    if (arg[0] == '/') return arg;
    if (cwd == "/") return "/" + arg;
    return cwd + "/" + arg;
}

// ===== Command handlers =====

std::string handle_pwd(const std::string& cwd) {
    return cwd + "\n";
}

std::string handle_ls(const std::map<std::string, std::vector<std::string>>& fs,
                      const std::string& cwd,
                      const std::string& arg) {
    std::string target = arg.empty() ? cwd : join_path(cwd, arg);

    auto it = fs.find(target);
    if (it == fs.end()) {
        return "ls: cannot access '" + target + "': No such file or directory\n";
    }

    const auto& items = it->second;
    std::string out;
    for (const auto& name : items) {
        out += name + "  ";
    }
    if (!out.empty()) out += "\n";
    return out;
}

std::string handle_cd(const std::map<std::string, std::vector<std::string>>& fs,
                      std::string& cwd,
                      const std::string& arg) {
    if (arg.empty()) {
        cwd = "/home/user";
        return "";
    }

    std::string target = join_path(cwd, arg);

    if (!is_directory(fs, target)) {
        return "bash: cd: " + arg + ": No such file or directory\n";
    }

    cwd = target;
    return "";
}

std::string handle_cat(const std::string& cwd, const std::string& arg) {
    if (arg.empty()) {
        return "cat: missing file operand\n";
    }

    std::string path = join_path(cwd, arg);

    if (path == "/etc/passwd") {
        return
            "root:x:0:0:root:/root:/bin/bash \n"
            "user:x:1000:1000:Fake User:/home/user:/bin/bash \n";
    }

    if (path == "/home/user/.ssh/id_rsa") {
        return
            "-----BEGIN OPENSSH PRIVATE KEY-----\n"
            "FAKEKEYFAKEKEYFAKEKEYFAKEKEY\n"
            "-----END OPENSSH PRIVATE KEY-----\n";
    }

    if (path == "/home/user/notes.txt") {
        return "TODO: secure the server\n";
    }

    return "cat: " + arg + ": No such file or directory\n";
}

std::string handle_sudo(const std::string& arg) {
    (void)arg;
    return "sudo: permission denied\n";
}

std::string handle_echo(const std::string& arg) {
    return arg + "\n";
}

std::string handle_unknown(const std::string& cmd, const std::string& arg) {
    std::string full = cmd;
    if (!arg.empty()) full += " " + arg;
    return "bash: " + full + ": command not found\n";
}

// ===== Client handler =====

void handle_client(SOCKET client, const std::string& ip) {
    log_event("[" + current_time() + "] Connection from " + ip);

    char buffer[BUFFER_SIZE] = {0};

    // --- Fake login ---
    std::string askUser = "login: ";
    send(client, askUser.c_str(), askUser.size(), 0);

    int bytes = recv(client, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) { closesocket(client); return; }
    std::string username = trim(std::string(buffer, bytes));
    log_event("[" + current_time() + "] Username attempt: " + username);

    std::string askPass = "Password: ";
    send(client, askPass.c_str(), askPass.size(), 0);

    memset(buffer, 0, BUFFER_SIZE);
    bytes = recv(client, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) { closesocket(client); return; }
    std::string password = trim(std::string(buffer, bytes));
    log_event("[" + current_time() + "] Password attempt: " + password);

    std::string fail = "Login incorrect\nlogin: ";
    send(client, fail.c_str(), fail.size(), 0);

    memset(buffer, 0, BUFFER_SIZE);
    bytes = recv(client, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) { closesocket(client); return; }
    std::string user2 = trim(std::string(buffer, bytes));
    log_event("[" + current_time() + "] Username retry: " + user2);

    std::string banner =
        "Welcome to FakeService Shell v3.5\n"
        "Type commands. Type 'exit' to disconnect.\n\n$ ";
    send(client, banner.c_str(), banner.size(), 0);

    // --- Shell stage ---
    auto fs = build_fs();
    std::string cwd = "/home/user";

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes = recv(client, buffer, BUFFER_SIZE - 1, 0);

        if (bytes <= 0) {
            log_event("[" + current_time() + "] Client disconnected: " + ip);
            break;
        }

        std::string raw(buffer, bytes);
        std::string line = trim(raw);

        if (line.empty()) {
            std::string prompt = "$ ";
            send(client, prompt.c_str(), prompt.size(), 0);
            continue;
        }

        // Split into command + arguments
        std::string cmd, arg;
        size_t spacePos = line.find(' ');
        if (spacePos == std::string::npos) {
            cmd = line;
            arg = "";
        } else {
            cmd = line.substr(0, spacePos);
            arg = trim(line.substr(spacePos + 1));
        }

        log_event("[" + current_time() + "] CMD from " + ip + ": " + line);

        std::string out;

        if (cmd == "exit") {
            out = "logout\n";
            send(client, out.c_str(), out.size(), 0);
            break;
        } else if (cmd == "pwd") {
            out = handle_pwd(cwd);
        } else if (cmd == "ls") {
            out = handle_ls(fs, cwd, arg);
        } else if (cmd == "cd") {
            out = handle_cd(fs, cwd, arg);
        } else if (cmd == "cat") {
            out = handle_cat(cwd, arg);
        } else if (cmd == "sudo") {
            out = handle_sudo(arg);
        } else if (cmd == "echo") {
            out = handle_echo(arg);
        } else {
            out = handle_unknown(cmd, arg);
        }

        out += "$ ";
        send(client, out.c_str(), out.size(), 0);
    }

    closesocket(client);
}

// ===== Main =====

int main() {
    std::cout << "=== Interactive Windows Honeypot (Fake Shell) ===\n";
    std::cout << "Listening on port " << PORT << "..." << std::endl;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "[!] WSAStartup failed\n";
        return 1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        std::cerr << "[!] Failed to create socket\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[!] Bind failed\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }

    if (listen(server, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[!] Listen failed\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }

    std::cout << "[*] Honeypot active. Waiting for connections...\n";

    while (true) {
        sockaddr_in client_addr{};
        int len = sizeof(client_addr);
        SOCKET client = accept(server, (sockaddr*)&client_addr, &len);
        if (client == INVALID_SOCKET) {
            continue;
        }

        std::string ip = inet_ntoa(client_addr.sin_addr);
        handle_client(client, ip);
    }

    closesocket(server);
    WSACleanup();
    return 0;
}
