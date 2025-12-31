#include <thread>
#pragma once

class TCPListener {
  public:
    TCPListener();
    ~TCPListener();

    int get_port();

    int start_listener(); // Starts the listener (Creates and backgrounds the listener)
    int stop_listener();  // Stops the listener thread and closes listener fd
    void accept_loop();

  private:
    int m_listener_port;           // The Port for the Listener
    int m_listener;                // Listener Fd
    std::thread m_listener_thread; // The background thread for the listener
    bool m_running;                // Is the thread running
    
    bool is_running();             // Checks if the thread is running
    bool is_listening();       // Checks if the listener fd is present
    
    int background_listener(); // Creates a background thread for the listener to run on
    int create_listener();     // Creates the listener fd
};