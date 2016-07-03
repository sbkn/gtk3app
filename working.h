#pragma once

#include <gtkmm.h>
#include <thread>
#include <mutex>

class ExampleWindow;

class ExampleWorker
{
public:
  ExampleWorker();

  // Thread function.
  void do_work(
    ExampleWindow* caller,
    std::vector<std::string> id_vec,
    std::string payload_string
  );

  void get_data(double* fraction_done, Glib::ustring* message) const;
  void stop_work();
  bool has_stopped() const;

private:
  // Synchronizes access to member data.
  mutable std::mutex m_Mutex;

  // Data used by both GUI thread and worker thread.
  bool m_shall_stop;
  bool m_has_stopped;
  double m_fraction_done;
  Glib::ustring m_message;

protected:
  std::string build_cmd_params(int, std::vector<std::string> *, std::string *);

};
