#pragma once

#include <gtkmm.h>

class ExampleWindow : public Gtk::Window
{
public:
  ExampleWindow();
  virtual ~ExampleWindow();
  void change_buffer_text(std::string);

protected:

  void fill_buffers();

  void on_button_quit();
  void on_button_buffer1();
  void on_button_buffer2();
  void on_button_file_clicked();
  void read_input_file(std::string);
  void change_label_text(std::string);
  void remove_label_text();

  Gtk::Box m_VBox, m_HBox;

  Gtk::ScrolledWindow m_ScrolledWindow_Label;
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TextView m_TextView;
  Gtk::Label m_TextLabel;

  Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1, m_refTextBuffer2;

  Gtk::ButtonBox m_ButtonBox;
  Gtk::Button m_Button_Quit, m_Button_Buffer1, m_Button_Buffer2,
    m_Button_File;
};
