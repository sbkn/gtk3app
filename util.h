#pragma once

#include <string>
#include <gtkmm.h>

struct Invoke_params {
  std::string function_name;
  bool dry_run;
  std::string payload;
  std::vector<std::string> ids;
} ;

#include "working.h"


class ExampleWindow : public Gtk::Window
{
public:
  ExampleWindow();
  virtual ~ExampleWindow();
  void notify();

private:
  void on_notification_from_worker_thread();
  void update_start_stop_buttons();

protected:

  void fill_buffers();

  void on_button_quit();
  void on_button_run();
  void on_dryrun_button_clicked();
  void on_button_id_file_clicked();
  void on_button_load_payload_file_clicked();
  void on_button_save_payload_file_clicked();
  void read_id_file(std::string);
  void read_payload_file(std::string);
  void set_ids_text_view_text(std::string);
  void clear_payload_text_view();
  void clear_ids_text_view();
  void set_payload_text_view_text(std::string);
  void get_cur_payload();
  void get_cur_ids();
  void get_cur_func_name();

  Gtk::Box m_VBox, m_HBox, m_VBox_Ids, m_VBox_Payload;

  Gtk::ScrolledWindow m_ScrolledWindow_Ids;
  Gtk::ScrolledWindow m_ScrolledWindow_Payload;
  Gtk::TextView m_TextView_Payload;
  Gtk::TextView m_TextView_Ids;
  Gtk::Label m_Label_Ids, m_Label_Payload;

  Gtk::Entry m_Entry_FuncName;

  Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1;

  Gtk::ButtonBox m_HButtonBox, m_ButtonBox;

  Gtk::CheckButton m_CheckButton_DryRun;

  Gtk::Button m_Button_Quit, m_Button_Run, m_Button_LoadPayload,
    m_Button_SavePayload, m_Button_Buffer2, m_Button_IdFilePicker;

  Glib::Dispatcher m_Dispatcher;
  ExampleWorker m_Worker;
  std::thread* m_WorkerThread;
};
