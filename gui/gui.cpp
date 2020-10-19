#include "gui.hpp"
#include "../cheats/cheatmanager.hpp"
#include "../memory.hpp"
#include "../hookmanager.hpp"

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"
#include <gl/gl.h>
#include <SDL.h>
#include <SDL_mouse.h>


drawing::gui::gui(HWND hWnd)
	: hWnd(hWnd), page(0u), tabs{ "Triggerbot","Aimbot","Tracelines","Barrelesp","Misc" }
{
	assert(globals::ConfigManager);
	assert(globals::ActiveProfile);
	assert(globals::CheatManager);
	assert(globals::HookManager);

	this->profile = &globals::ActiveProfile;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplOpenGL2_Init();
	this->initStyle();
}

drawing::gui::~gui()
{
	ImGui::PopStyleColor(4);
	if (this->options()->active) this->hide();
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void drawing::gui::render() noexcept
{
	if (this->options()->active) {
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			cheats::cheatManager& cm = *globals::CheatManager; // shortcut

			ImGui::Begin("ACM", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::BeginGroup();

			for (size_t i = 0; i < this->tabcount; ++i) {
				ImGui::SameLine();
				if (ImGui::Button(this->tabs[i], ImVec2(300 / this->tabcount - 9, 25))) {
					this->page = i;
				}
			}
			ImGui::EndGroup();

			switch (this->page) {
			case 0:
				ImGui::PushID(&cm.triggerbot);//uuid for scope labels
				{
					if (ImGui::ConstCheckbox(cm.triggerbot.name(), &cm.triggerbot.options()->active)) {
						cm.triggerbot.toggle();
					}
					ImGui::Checkbox("Friendly Fire", &cm.triggerbot.options()->friendlyfire);
				} 
				ImGui::PopID();
				break;
			case 1:
				ImGui::PushID(&cm.aimbot);//uuid for scope labels
				{
					auto curropt = cm.aimbot.options();
					if (ImGui::ConstCheckbox(cm.aimbot.name(), &curropt->active)) {
						cm.aimbot.toggle();
					}
					ImGui::Checkbox("Show Circle", &curropt->showcircle);
					if (curropt->showcircle)
						ImGui::ColorEdit3("Circle Color", curropt->circlecolor);
					ImGui::Checkbox("Friendly Fire", &curropt->friendlyfire);
					ImGui::Checkbox("Autofire", &curropt->autoshoot);
					ImGui::Checkbox("Lock Target", &curropt->targetLock);
					ImGui::Checkbox("Smoothing", &curropt->smoothing);
					if (curropt->smoothing) {
						ImGui::SliderFloat("Smooth level", &curropt->smoothvalue, cm.aimbot.minsmooth,
							cm.aimbot.maxsmooth);
					}
					ImGui::SliderFloat("FOV", &curropt->fov, cm.aimbot.minfov, cm.aimbot.maxfov);
				}
				ImGui::PopID();
				break;
			case 2:
				ImGui::PushID(&cm.tracelines);
				{
					if (ImGui::ConstCheckbox(cm.tracelines.name(), &cm.tracelines.options()->active)) {
						cm.tracelines.toggle();
					}
					ImGui::Checkbox("Teammates", &cm.tracelines.options()->teammates);
					ImGui::SliderFloat("Line Width", &cm.tracelines.options()->linewidth,
						cm.tracelines.minlinewidth, cm.tracelines.maxlinewidth);
					ImGui::ColorEdit3("Teammate Color", cm.tracelines.options()->allyColor);
					ImGui::ColorEdit3("Enemy Color", cm.tracelines.options()->enemyColor);
				}
				ImGui::PopID();
				break;
			case 3:
				ImGui::PushID(&cm.barrelesp);
				{
					if (ImGui::ConstCheckbox(cm.barrelesp.name(), &cm.barrelesp.options()->active)) {
						cm.barrelesp.toggle();
					}
					ImGui::Checkbox("Teammates", &cm.barrelesp.options()->teammates);
					ImGui::SliderFloat("Line Length", &cm.barrelesp.options()->linelength,
						cm.barrelesp.minlinelength, cm.barrelesp.maxlinelength);
					ImGui::SliderFloat("Line Width", &cm.barrelesp.options()->linewidth,
						cm.barrelesp.minlinewidth, cm.barrelesp.maxlinewidth);
					ImGui::ColorEdit3("Teammate Color", cm.barrelesp.options()->allyColor);
					ImGui::ColorEdit3("Enemy Color", cm.barrelesp.options()->enemyColor);

				}
				ImGui::PopID();
				break;
			case 4:
				ImGui::PushID(&globals::ActiveProfile->misc);
				{
					ImGui::Checkbox("Super-Secret Features", &globals::ActiveProfile->misc.supersecret);
				}
				ImGui::PopID();
				break;
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}
}

void drawing::gui::show() const noexcept
{
	
	//globals::MemoryManager->SDL_WM_GrabInput(SDL_GRAB_OFF);
	globals::MemoryManager->SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	globals::MemoryManager->SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	globals::MemoryManager->SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	globals::MemoryManager->SDL_ShowCursor(SDL_ENABLE);
	this->options()->active = true;
}

void drawing::gui::hide() const noexcept
{
	globals::MemoryManager->SDL_ShowCursor(SDL_DISABLE);
	//globals::MemoryManager->SDL_WM_GrabInput(SDL_GRAB_ON);
	globals::MemoryManager->SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
	globals::MemoryManager->SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
	globals::MemoryManager->SDL_EventState(SDL_MOUSEBUTTONUP, SDL_ENABLE);
	this->options()->active = false;
}

void drawing::gui::toggle() const noexcept
{
	if (this->options()->active) {
		return this->hide();
	}
	this->show();
}

void drawing::gui::initStyle() const noexcept
{
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(0, 0, 0, 150));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(0, 0, 0, 100));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::ImColor(255, 255, 255));
}


configManager::config::menu_conf* drawing::gui::options() const noexcept
{
	return &(*this->profile)->general;
}