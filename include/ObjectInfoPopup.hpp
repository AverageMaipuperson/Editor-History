#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
using namespace geode::prelude;
using namespace cocos2d;
using namespace geode;

class ObjectInfoPopup : public Popup {
	static ObjectInfoPopup* create();
	bool init();
};