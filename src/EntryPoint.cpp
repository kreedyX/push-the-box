#pragma once
#include <iostream>
#include "Core/Application.h"

int main() {
	Application app(1280, 720, "Push The Box");
	app.run(250);
	return 0;
}