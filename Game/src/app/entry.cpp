
#include "application/application.hpp"

int main() {
	auto App = new DesecratedDungeons::DesecratedDungeons();

	App->Run();

	delete App;
}