#include "main.hpp"

#include "HMUI/ViewController.hpp"
#include "beatsaber-hook/shared/utils/typedefs-string.hpp"
#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "UnityEngine/Resources.hpp"
#include "lang_ctrl.hpp"
#include "scotland2/shared/modloader.h"
#include "modconfig.hpp"
#include "sslocalization/shared/SSL10n.hpp"
#include "bsml/shared/BSML.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "HMUI/FlowCoordinator.hpp"
static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};
// Stores the ID and version of our mod, and is sent to
// the modloader upon startup

GlobalNamespace::MenuTransitionsHelper * menuTransitionHelper = nullptr;

#define TEXT_FOLLOW_GAME "FollowGame"

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    // Create our UI elements only when shown for the first time.
    
    static std::vector<std::string_view> languages = {
      TEXT_FOLLOW_GAME,
      #define L(x) #x,
      FOR_EACH_LANGUAGE(L)
      #undef L
    };

    if(firstActivation) {
        // Create a container that has a scroll bar
        UnityEngine::GameObject* container = BSML::Lite::CreateScrollableSettingsContainer(self->get_transform());
        
        int old_config = getConfig().SelectedLanguage.GetValue();
        if(old_config < 0 || old_config >= languages.size())
          old_config = 0;

        BSML::Lite::CreateDropdown(container->get_transform(), SSL10n::Get("SSLCTRL_SETTHING_LABEL_LANGUAGE"), languages[old_config],  languages, [](StringW str){
          std::string v = str;
          if(v == TEXT_FOLLOW_GAME){
            getConfig().SelectedLanguage.SetValue(SSLCTRL::L_FollowGame);
            return;
          }

          #define L(x) if(v == #x) { getConfig().SelectedLanguage.SetValue(SSLCTRL::L_##x); return; }
          FOR_EACH_LANGUAGE(L)
          #undef  L
        });

        BSML::Lite::CreateText(container->get_transform(), SSL10n::Get("SSLCTRL_SETTHING_TEXT_FOLLOW_HINT"), {0,0},{0,5});
        BSML::Lite::CreateText(container->get_transform(), SSL10n::Get("SSLCTRL_SETTHING_TEXT_KEPT"), {0,0},{0,5});

        BSML::Lite::CreateUIButton(container->get_transform(), SSL10n::Get("SSLCTRL_SETTHING_APPLY_NOW"), [](){
          SSLCTRL::SyncSelectedLanguage();
          UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuTransitionsHelper*>()->First()->RestartGame(nullptr);
        });
    }
}


// Called at the early stages of game loading
MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
  *info = modInfo.to_c();

  getConfig().Init(modInfo);

  // File logging
  Paper::Logger::RegisterFileContextId(PaperLogger.tag);

  PaperLogger.info("Completed setup!");

  SSLCTRL::SyncSelectedLanguage();
}

// Called later on in the game loading - a good time to install function hooks
MOD_EXTERN_FUNC void late_load() noexcept {
  il2cpp_functions::Init();

  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_LABEL_LANGUAGE", "Language");
  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_LABEL_LANGUAGE", "语言设置/Language", SSL10n::L_Simplified_Chinese);

  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_MENU_TITLE", "Mod Language");
  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_MENU_TITLE", "Mod语言", SSL10n::L_Simplified_Chinese);

  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_APPLY_NOW", "Apply Now");
  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_APPLY_NOW", "立即应用", SSL10n::L_Simplified_Chinese);

  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_TEXT_KEPT", "Some text will not changed until you restart game.");
  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_TEXT_KEPT", "部分内容重启游戏生效", SSL10n::L_Simplified_Chinese);
  
  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_TEXT_FOLLOW_HINT", "FollowGame need restart game.");
  SSL10n::Database::AddKeyValue("SSLCTRL_SETTHING_TEXT_FOLLOW_HINT", "FollowGame需重启游戏生效", SSL10n::L_Simplified_Chinese);

  BSML::Init();
  BSML::Register::RegisterSettingsMenu(SSL10n::Get("SSLCTRL_SETTHING_MENU_TITLE"),DidActivate, false);


  PaperLogger.info("Installing hooks...");

  PaperLogger.info("Installed all hooks!");
}