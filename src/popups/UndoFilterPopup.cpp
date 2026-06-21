#include "UndoFilterPopup.hpp"

CCMenuItemToggler* UndoFilterPopup::toggler(char const* spr1, const char* spr2) {
    auto off = CCSprite::create(spr1);
    auto on = CCSprite::create(spr2);

    return CCMenuItemToggler::create(off, on, this, NULL);
}

bool UndoFilterPopup::init(float width, float height, std::string const& text, UndoObjectPopup* parent)
{   
    if (!geode::Popup::init(width, height, "GJ_square01.png")) return false;

    m_parent = parent;

    auto arr = CCArrayExt<CCMenuItemToggler*>(CCArray::create());

    arr.push_back(toggler("cd_01.png"_spr, "cd_02.png"_spr));
    arr.push_back(toggler("t_01.png"_spr, "t_02.png"_spr));
    arr.push_back(toggler("sd_01.png"_spr, "sd_02.png"_spr));

    m_menu = CCRadioMenu::create(arr);
    m_menu->alignItemsHorizontallyWithPadding(5.f);
    m_menu->setPosition(ccp(m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2));
    m_mainLayer->addChild(m_menu);

    this->setTitle(text.c_str());
    return true;
}

void UndoFilterPopup::onClose(cocos2d::CCObject* sender)
{
    UndoObjectPopup::m_filter = m_menu->getValue();
    geode::Popup::onClose(sender);
    m_parent->reload();
}