#include "util.h"
#include <gtkmm/application.h>

int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  ExampleWindow helloworld;

  const char* text = "My text.";

  helloworld.change_buffer_text(text);

  //Shows the window and returns when it is closed.
  return app->run(helloworld);
}
