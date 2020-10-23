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
	: hWnd(hWnd), page(0u), tabs{ "Triggerbot","Aimbot","Snaplines","Barrelesp","ESP","Misc" }
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
					auto curropt = cm.triggerbot.options();
					if (ImGui::ConstCheckbox(cm.triggerbot.name(), &curropt->active)) {
						cm.triggerbot.toggle();
					}
					ImGui::Checkbox("Friendly Fire", &curropt->friendlyfire);
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
					ImGui::Checkbox("Show FOV", &curropt->showcircle);
					if (curropt->showcircle)
						ImGui::ColorEdit3("FOV Circle Color", curropt->circlecolor);
					ImGui::Checkbox("Friendly Fire", &curropt->friendlyfire);
					ImGui::Checkbox("On Hotkey", &curropt->onkey);
					if (curropt->onkey){
						static const char* items[] = { "Shift", "LControl", "Alt" };
						static int current = (curropt->hotkey== SDLK_LSHIFT) ? 0  // ?maaaybee find a better way  
							: (curropt->hotkey == SDLK_LCTRL) ? 1 : (curropt->hotkey == SDLK_LALT) ? 2 : 0; 
						ImGui::Combo("Hotkey", &current, items, IM_ARRAYSIZE(items));
						switch (current) {
						case 0:
							curropt->hotkey = SDLK_LSHIFT;
							break;
						case 1:
							curropt->hotkey = SDLK_LCTRL;
							break;
						case 2:
							curropt->hotkey = SDLK_LALT;
							break;
						}
					}
					ImGui::Checkbox("Autofire", &curropt->autoshoot);
					ImGui::Checkbox("Range", &curropt->range);
					if (curropt->range) {
						ImGui::DragFloat("", &curropt->rangevalue, 0.5f, 0.f, 150.f, "%.2f", ImGuiSliderFlags_::ImGuiSliderFlags_ClampOnInput);
					}
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
				ImGui::PushID(&cm.snaplines);
				{
					auto curropt = cm.snaplines.options();
					if (ImGui::ConstCheckbox(cm.snaplines.name(), &curropt->active)) {
						cm.snaplines.toggle();
					}
					ImGui::Checkbox("Teammates", &curropt->teammates);
					ImGui::SliderFloat("Line Width", &curropt->linewidth,
						cm.snaplines.minlinewidth, cm.snaplines.maxlinewidth);
					ImGui::ColorEdit3("Teammate Color", curropt->allyColor);
					ImGui::ColorEdit3("Enemy Color", curropt->enemyColor);
				}
				ImGui::PopID();
				break;
			case 3:
				ImGui::PushID(&cm.barrelesp);
				{
					auto curropt = cm.barrelesp.options();
					if (ImGui::ConstCheckbox(cm.barrelesp.name(), &curropt->active)) {
						cm.barrelesp.toggle();
					}
					ImGui::Checkbox("Teammates", &curropt->teammates);
					ImGui::SliderFloat("Line Length", &curropt->linelength,
						cm.barrelesp.minlinelength, cm.barrelesp.maxlinelength);
					ImGui::SliderFloat("Line Width", &curropt->linewidth,
						cm.barrelesp.minlinewidth, cm.barrelesp.maxlinewidth);
					ImGui::ColorEdit3("Teammate Color", curropt->allyColor);
					ImGui::ColorEdit3("Enemy Color", curropt->enemyColor);

				}
				ImGui::PopID();
				break;
			case 4:
				ImGui::PushID(&cm.esp);
				{
					auto curropt = cm.esp.options();
					if (ImGui::ConstCheckbox(cm.esp.name(), &curropt->active)) {
						cm.esp.toggle();
					}
					ImGui::Checkbox("Teammates", &curropt->teammates);
					ImGui::Checkbox("Healthbar", &curropt->healthbar);
					ImGui::ColorEdit3("Teammate Color", curropt->allyColor);
					ImGui::ColorEdit3("Enemy Color", curropt->enemyColor);
				}
				ImGui::PopID();
			case 5:
				ImGui::PushID(&globals::ActiveProfile->misc);
				{
					auto curropt = &globals::ActiveProfile->misc;
					ImGui::Checkbox("Super-Secret Features", &curropt->supersecret);
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