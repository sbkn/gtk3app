#include <iostream>
#include <fstream>
#include <string>

#include "util.h"

ExampleWindow::ExampleWindow()
: m_VBox(Gtk::ORIENTATION_VERTICAL),
  m_HBox(Gtk::ORIENTATION_HORIZONTAL),
  m_HButtonBox(Gtk::ORIENTATION_HORIZONTAL),
  m_ButtonBox(Gtk::ORIENTATION_VERTICAL),
  m_Button_Quit("_Quit", true),
  m_Button_Run("Run"),
  m_Button_Buffer1("Use buffer 1"),
  m_Button_File("Choose a File"),
  m_Dispatcher(),
  m_WorkerThread(nullptr)
{
  set_title("Invoker");
  set_border_width(5);
  set_default_size(1000, 600);
  this->set_icon_from_file("icon.png");

  add(m_VBox);
  add(m_HBox);

  // ADD TEXTVIEW
  m_ScrolledWindow_Label.add(m_TextLabel);
  m_ScrolledWindow_Label.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // ADD LABEL
  m_ScrolledWindow.add(m_TextView);
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_HBox.pack_start(m_ScrolledWindow_Label);
  m_HBox.pack_start(m_ScrolledWindow);
  m_VBox.pack_start(m_HBox);

  //Add buttons:
  m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_ButtonBox.pack_start(m_Button_File, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_Buffer1, Gtk::PACK_SHRINK);
  m_HButtonBox.pack_start(m_Button_Quit, Gtk::PACK_EXPAND_PADDING);
  m_HButtonBox.pack_start(m_Button_Run, Gtk::PACK_EXPAND_PADDING);
  m_HButtonBox.set_layout(Gtk::BUTTONBOX_EDGE);

  m_ButtonBox.pack_start(m_HButtonBox);
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_spacing(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);

  //Connect signals:
  m_Button_File.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_file_clicked) );
  m_Button_Quit.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_quit) );
  m_Button_Run.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_run) );
  m_Button_Buffer1.signal_clicked().connect(sigc::mem_fun(*this,
              &ExampleWindow::on_button_buffer1) );
  // Connect the handler to the dispatcher.
  m_Dispatcher.connect(sigc::mem_fun(*this, &ExampleWindow::on_notification_from_worker_thread));

  fill_buffers();
  on_button_buffer1();

  show_all_children();
}

void ExampleWindow::fill_buffers()
{
  m_refTextBuffer1 = Gtk::TextBuffer::create();
  m_refTextBuffer1->set_text("This is the text from TextBuffer #1.");
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

  if (m_WorkerThread)
  {
    std::cout << "Can't start a worker thread while another one is running." << std::endl;
  }
  else
  {
    // Start a new worker thread.
    m_WorkerThread = new std::thread(
      [this]
      {
        m_Worker.do_work(this, id_vector);
      });
    }
  // UPDATE START BUTTON HERE, MAKE IT CANCEL OR SOMETHING ..
  // CHECK: https://developer.gnome.org/gtkmm-tutorial/stable/sec-multithread-example.html.en
}

void ExampleWindow::on_button_buffer1()
{
  m_TextView.set_buffer(m_refTextBuffer1);
}

void ExampleWindow::change_buffer_text(std::string text)
{

  Glib::RefPtr<Gtk::TextBuffer> tmp = Gtk::TextBuffer::create();
  tmp->set_text(text);

  m_TextView.set_buffer(tmp);
}

void ExampleWindow::change_label_text(std::string text)
{
  m_TextLabel.set_text(text);
}

void ExampleWindow::remove_label_text()
{
  std::string tmp = " ";
  m_TextLabel.set_text(tmp);
}

/**
  * FILEPICKER
  */
void ExampleWindow::on_button_file_clicked()
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

      this->read_input_file(filename);
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
  * FILE READING AND STUFF
  */
void ExampleWindow::read_input_file(std::string filename)
{
  std::ifstream infile(filename);
  std::string line;

  this->remove_label_text();

  while (std::getline(infile, line))
  {
      std::istringstream iss(line);
      std::string a;

      if (!(iss >> a)) { break; }
      id_vector.push_back(a);
      std::string tmp = m_TextLabel.get_text();
      this->change_label_text(tmp + "\n" + a);
  }
  std::cout << "Read in " << id_vector.size() << " IDs." << std::endl;
}

void ExampleWindow::update_start_stop_buttons()
{
  const bool thread_is_running = m_WorkerThread != nullptr;

  m_Button_Run.set_sensitive(thread_is_running);
}

// notify() is called from ExampleWorker::do_work(). It is executed in the worker
// thread. It triggers a call to on_notification_from_worker_thread(), which is
// executed in the GUI thread.
void ExampleWindow::notify()
{
  m_Dispatcher.emit();
}

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
    update_start_stop_buttons();
  }
  //update_widgets();
}
