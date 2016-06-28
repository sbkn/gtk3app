#include <gtkmm/application.h>
#include "util.h"

int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  ExampleWindow helloworld;

  return app->run(helloworld);
}
