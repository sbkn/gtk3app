#include "util.h"
#include "working.h"
#include <sstream>
#include <chrono>

ExampleWorker::ExampleWorker() :
  m_Mutex(),
  m_shall_stop(false),
  m_has_stopped(false),
  m_fraction_done(0.0),
  m_message()
{
}

// Accesses to these data are synchronized by a mutex.
// Some microseconds can be saved by getting all data at once, instead of having
// separate get_fraction_done() and get_message() methods.
void ExampleWorker::get_data(double* fraction_done, Glib::ustring* message) const
{
  std::lock_guard<std::mutex> lock(m_Mutex);

  if (fraction_done)
    *fraction_done = m_fraction_done;

  if (message)
    *message = m_message;
}

void ExampleWorker::stop_work()
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  m_shall_stop = true;
}

bool ExampleWorker::has_stopped() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_stopped;
}

void ExampleWorker::do_work(ExampleWindow* caller)
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = false;
    m_fraction_done = 0.0;
    m_message = "";
  } // The mutex is unlocked here by lock's destructor.

  // Simulate a long calculation.
  for (int i = 0; i < 3; ++i) // do until break
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    {
      std::lock_guard<std::mutex> lock(m_Mutex);

      m_fraction_done += 0.01;

      if (i % 4 == 3)
      {
        std::ostringstream ostr;
        ostr << (m_fraction_done * 100.0) << "% done\n";
        m_message += ostr.str();
      }

      if (m_fraction_done >= 1.0)
      {
        m_message += "Finished";
        break;
      }
      if (m_shall_stop)
      {
        m_message += "Stopped";
        break;
      }
    }

    caller->notify();
  }

  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_shall_stop = false;
    m_has_stopped = true;
  }

  caller->notify();
}
