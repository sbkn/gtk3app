#pragma once

#include <gtkmm.h>

class FilePicker : public Gtk::Window
{
public:
  FilePicker();
  virtual ~FilePicker();

protected:
  //Signal handlers:
  void on_button_file_clicked();

  //Child widgets:
  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_Button_File;
};
