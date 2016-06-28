#include <gtkmm/application.h>
#include "util.h"

int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  ExampleWindow helloworld;

  std::string text = "My text.";

  helloworld.change_buffer_text(text);

  return app->run(helloworld);
}
