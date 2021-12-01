
#include "application/application.hpp"

int main() {
	auto App = new TextureEditor::TextureEditor();

	App->Run();

	delete App;
}