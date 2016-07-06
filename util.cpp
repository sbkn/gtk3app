#include <iostream>
#include <fstream>
#include <string>

#include "util.h"

ExampleWindow::ExampleWindow()
: m_VBox(Gtk::ORIENTATION_VERTICAL),
  m_HBox(Gtk::ORIENTATION_HORIZONTAL),
  m_VBox_Ids(Gtk::ORIENTATION_VERTICAL),
  m_VBox_Payload(Gtk::ORIENTATION_VERTICAL),
  m_HButtonBox(Gtk::ORIENTATION_HORIZONTAL),
  m_ButtonBox(Gtk::ORIENTATION_VERTICAL),
  m_CheckButton_DryRun("Dry run"),
  m_Button_Quit("_Quit", true),
  m_Button_Run("Run"),
  m_Button_LoadPayload("Load payload"),
  m_Button_SavePayload("Save payload"),
  m_Button_IdFilePicker("Load IDs"),
  m_Dispatcher(),
  m_WorkerThread(nullptr)
{
  set_title("Invoker");
  set_border_width(5);
  set_default_size(1000, 600);
  this->set_icon_from_file("icon.png");

  add(m_VBox);
  add(m_HBox);
  add(m_VBox_Ids);
  add(m_VBox_Payload);

  // ADD IDS LABEL AND TEXTVIEW
  m_Label_Ids.set_text("IDs:");
  m_ScrolledWindow_Ids.add(m_TextView_Ids);
  m_ScrolledWindow_Ids.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  m_ScrolledWindow_Ids.set_min_content_height(400);
  m_ScrolledWindow_Ids.set_min_content_width(200);

  // ADD PAYLOAD LABEL AND TEXTVIEW
  m_Label_Payload.set_text("Payload:");
  m_ScrolledWindow_Payload.add(m_TextView_Payload);
  m_ScrolledWindow_Payload.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  m_ScrolledWindow_Payload.set_min_content_height(400);
  m_ScrolledWindow_Payload.set_min_content_width(200);

  m_VBox_Ids.pack_start(m_Label_Ids);
  m_VBox_Ids.pack_start(m_ScrolledWindow_Ids);
  m_VBox_Ids.pack_start(m_Button_IdFilePicker, Gtk::PACK_EXPAND_PADDING);

  m_VBox_Payload.pack_start(m_Label_Payload);
  m_VBox_Payload.pack_start(m_ScrolledWindow_Payload);
  m_VBox_Payload.pack_start(m_Button_LoadPayload, Gtk::PACK_EXPAND_PADDING);

  m_HBox.set_spacing(5);
  m_HBox.pack_start(m_VBox_Ids);
  m_HBox.pack_start(m_VBox_Payload);
  m_VBox.pack_start(m_HBox);

  //Add buttons:
  m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_ButtonBox.pack_start(m_CheckButton_DryRun, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_SavePayload, Gtk::PACK_SHRINK);
  m_HButtonBox.pack_start(m_Button_Quit, Gtk::PACK_EXPAND_PADDING);
  m_HButtonBox.pack_start(m_Button_Run, Gtk::PACK_EXPAND_PADDING);
  m_HButtonBox.set_layout(Gtk::BUTTONBOX_EDGE);

  m_ButtonBox.pack_start(m_HButtonBox);
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_spacing(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);

  //Connect signals:
  m_Button_IdFilePicker.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_id_file_clicked) );
  m_Button_Quit.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_quit) );
  m_Button_Run.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_run) );
  m_Button_LoadPayload.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_payload_file_clicked) );
  m_Button_SavePayload.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_savePayload) );
  m_CheckButton_DryRun.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_dryrun_button_clicked) );
  m_Dispatcher.connect(
    sigc::mem_fun(*this, &ExampleWindow::on_notification_from_worker_thread)
  );

  fill_buffers();
  m_CheckButton_DryRun.set_active(true);
  on_button_savePayload();

  show_all_children();
}


void ExampleWindow::fill_buffers()
{
  m_refTextBuffer1 = Gtk::TextBuffer::create();
  m_refTextBuffer1->set_text("{\"name\": \"lambda\"}");
}

ExampleWindow::~ExampleWindow()
{
}

void ExampleWindow::on_button_quit()
{
  if (m_WorkerThread)
  {
    // Order the worker thread to stop and wait for it to stop.
    m_Worker.stop_work();
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
  }
  hide();
}

void ExampleWindow::on_button_run()
{
  std::cout << "RUNNING IT!" << std::endl;
  this->get_cur_payload();
  this->get_cur_ids();

  if (m_WorkerThread)
  {
    std::cout << "Worker thread already running." << std::endl;
  }
  else
  {
    // Start a new worker thread.
    m_WorkerThread = new std::thread(
      [this]
      {
        m_Worker.do_work(this, id_vector, payload_string);
      });
    }
  // CHECK: https://developer.gnome.org/gtkmm-tutorial/stable/sec-multithread-example.html.en
  update_start_stop_buttons();
}

void ExampleWindow::on_button_savePayload()
{
  m_TextView_Payload.set_buffer(m_refTextBuffer1);
}

void ExampleWindow::set_ids_text_view_text(std::string text)
{
  Glib::RefPtr<Gtk::TextBuffer> tmp = Gtk::TextBuffer::create();
  tmp->set_text(text);

  m_TextView_Ids.set_buffer(tmp);
}

void ExampleWindow::set_payload_text_view_text(std::string text)
{
  Glib::RefPtr<Gtk::TextBuffer> tmp = Gtk::TextBuffer::create();
  tmp->set_text(text);

  m_TextView_Payload.set_buffer(tmp);
}

void ExampleWindow::clear_ids_text_view()
{
  Glib::RefPtr<Gtk::TextBuffer> tmp = m_TextView_Ids.get_buffer();
  tmp->erase(tmp->begin(), tmp->end());
}

void ExampleWindow::clear_payload_text_view()
{
  Glib::RefPtr<Gtk::TextBuffer> tmp = m_TextView_Payload.get_buffer();
  tmp->erase(tmp->begin(), tmp->end());
}


/*
 * GET THE PAYLOAD FROM TEXTVIEW
 */
void ExampleWindow::get_cur_payload()
{
  Glib::RefPtr<Gtk::TextBuffer> tmp = m_TextView_Payload.get_buffer();
  std::string tmp_payload(tmp->get_text());

  payload_string = tmp_payload;
}


/*
 * GET THE IDS FROM TEXTVIEW
 */
void ExampleWindow::get_cur_ids()
{
  id_vector.clear();
  Glib::RefPtr<Gtk::TextBuffer> tmp = m_TextView_Ids.get_buffer();
  std::string tmp_ids(tmp->get_text());

  std::istringstream iss(tmp_ids);

  std::string id;
  while (std::getline(iss, id))
  {
    id_vector.push_back(id);
  }
}


/**
  * FILEPICKER IDS
  */
void ExampleWindow::on_button_id_file_clicked()
{
  Gtk::FileChooserDialog dialog("Choose a file with IDs",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  //Add filters, so that only certain file types can be selected:

  auto filter_csv = Gtk::FileFilter::create();
  filter_csv->set_name("CSV files");
  filter_csv->add_mime_type("text/csv");
  filter_csv->add_mime_type("*.csv");
  dialog.add_filter(filter_csv);

  auto filter_text = Gtk::FileFilter::create();
  filter_text->set_name("Text files");
  filter_text->add_mime_type("text/plain");
  dialog.add_filter(filter_text);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  int result = dialog.run();

  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      //Notice that this is a std::string, not a Glib::ustring.
      std::string filename = dialog.get_filename();

      this->read_id_file(filename);
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      break;
    }
    default:
    {
      break;
    }
  }
}


/**
  * FILEPICKER PAYLOAD
  */
void ExampleWindow::on_button_payload_file_clicked()
{
  Gtk::FileChooserDialog dialog("Choose a file with a payload",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  //Add filters, so that only certain file types can be selected:

  auto filter_csv = Gtk::FileFilter::create();
  filter_csv->set_name("JSON files");
  filter_csv->add_mime_type("application/json");
  filter_csv->add_mime_type("*.json");
  dialog.add_filter(filter_csv);

  auto filter_text = Gtk::FileFilter::create();
  filter_text->set_name("Text files");
  filter_text->add_mime_type("text/plain");
  dialog.add_filter(filter_text);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  int result = dialog.run();

  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      //Notice that this is a std::string, not a Glib::ustring.
      std::string filename = dialog.get_filename();

      this->read_payload_file(filename);
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      break;
    }
    default:
    {
      break;
    }
  }
}


/**
  * ID FILE READING
  */
void ExampleWindow::read_id_file(std::string filename)
{
  std::ifstream infile(filename);
  std::string line;
  std::string tmp_string;
  Glib::RefPtr<Gtk::TextBuffer> m_textbuffer = Gtk::TextBuffer::create();

  id_vector.clear();
  this->clear_ids_text_view();

  while (std::getline(infile, line))
  {
      std::istringstream iss(line);
      std::string a;

      if (!(iss >> a)) { break; }
      id_vector.push_back(a);

      tmp_string += a + "\n";
  }
  this->set_ids_text_view_text(tmp_string);
  std::cout << "Read in " << id_vector.size() << " IDs." << std::endl;
}


/*
 * PAYLOAD FILE READING
 */
void ExampleWindow::read_payload_file(std::string filename)
{
  std::ifstream infile(filename);
  std::string line;

  payload_string.clear();
  this->clear_payload_text_view();

  while (std::getline(infile, line))
  {
      payload_string += line + "\n";
  }

  this->set_payload_text_view_text(payload_string);

  std::cout << "Size of payload: " << payload_string.size() << std::endl;
}


/*
 * SET BUTTONS ACTIVE / INACTIVE
 */
void ExampleWindow::update_start_stop_buttons()
{
  const bool thread_is_running = m_WorkerThread != nullptr;

  m_Button_Run.set_sensitive(!thread_is_running);
  m_TextView_Ids.set_editable(!thread_is_running);
  m_TextView_Payload.set_editable(!thread_is_running);
}


// notify() is called from ExampleWorker::do_work(). It is executed in the worker
// thread. It triggers a call to on_notification_from_worker_thread(), which is
// executed in the GUI thread.
void ExampleWindow::notify()
{
  m_Dispatcher.emit();
}


/*
 * FINISH WORKER THREAD HANDLING
 */
void ExampleWindow::on_notification_from_worker_thread()
{
  if (m_WorkerThread && m_Worker.has_stopped())
  {
    // Work is done.
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
    delete m_WorkerThread;
    m_WorkerThread = nullptr;
    std::cout << "stopping worker" << std::endl;
  }
  update_start_stop_buttons();
}

/*
 * HANDLE CHECKBUTTON FOR DRY RUN
 */
void ExampleWindow::on_dryrun_button_clicked()
{
  std::cout << "The Button was clicked: state="
      << (m_CheckButton_DryRun.get_active() ? "true" : "false")
      << std::endl;
}
