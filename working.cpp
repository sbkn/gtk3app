#include "util.h"
#include "working.h"
#include <sstream>
#include <chrono>
#include <iostream>
#include <string>

// FOR PROCESS SPAWNING
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>



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

void ExampleWorker::do_work(
  ExampleWindow* caller,
  std::vector<std::string> id_vec,
  std::string payload_string
)
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = false;
    m_fraction_done = 0.0;
    m_message = "";
  } // The mutex is unlocked here by lock's destructor.

  // Simulate a long calculation. REMOVE IT
  for (uint i = 0; i < id_vec.size(); ++i) // do until break
  {
    std::string cmd = this->build_cmd_params(i, &id_vec, &payload_string);

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



      // CALL THE CLI HERE
      std::cout << cmd.c_str() << std::endl;
      char buffer[128];
      std::string result = "";
      std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
      if (!pipe) throw std::runtime_error("popen() failed!");
      while (!feof(pipe.get())) {
          if (fgets(buffer, 128, pipe.get()) != NULL)
              result += buffer;
      }
      std::cout << result << std::endl;

      //TODO: EVALUATE THE StatusCode OF result HERE



    caller->notify();
  }

  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_shall_stop = false;
    m_has_stopped = true;
  }

  caller->notify();
}

std::string ExampleWorker::build_cmd_params(
  int index_in_array,
  std::vector<std::string> *id_vec,
  std::string *payload_string
)
{
  std::string payload = (*payload_string);

  this->finalize_payload(&payload);
  this->insert_id_into_payload(&payload, (*id_vec)[index_in_array]);

  //TODO: USE DryRun as invocation-type to simulate a run
  std::string cmd = std::string("aws lambda invoke") +
  std::string(" --invocation-type RequestResponse") +
  std::string(" --function-name vwfs-dmks-euw1-lambda-pias:test") +
  std::string(" --region eu-west-1") +
	std::string(" --log-type Tail") +
	std::string(" --payload \"") + std::string(payload) + "\"" +
	std::string(" outputfile_") + std::string((*id_vec)[index_in_array]) +
  std::string(".txt");

  return cmd;
}

void ExampleWorker::finalize_payload(std::string *payload)
{
  payload->erase(
    std::remove(payload->begin(), payload->end(), '\n'), payload->end()
  );
  payload->erase(
    std::remove(payload->begin(), payload->end(), '\t'), payload->end()
  );
  payload->erase(
    std::remove(payload->begin(), payload->end(), ' '), payload->end()
  );

  size_t index = 0;
  while (true) {
     index = payload->find("\"", index);
     if (index == std::string::npos) break;

     payload->replace(index, 1, "\\\"");

     index += 3;
   }
}


/*  REPLACE TEMPLATE STRINGS WITH IDS: */
void ExampleWorker::insert_id_into_payload(
  std::string *payload,
  std::string id
)
{
  size_t index = 0;
  while (true) {
     index = payload->find("{{ID}}", index);
     if (index == std::string::npos) break;

     payload->replace(index, 6, id);

     index += id.length();
   }
}
