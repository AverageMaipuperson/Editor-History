#include "CCRadioMenu.h"

CCRadioMenu* CCRadioMenu::create(CCArrayExt<CCMenuItemToggler*> togglers)
{
    auto ret = new CCRadioMenu();

    if (ret && ret->init(togglers))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool CCRadioMenu::init(CCArrayExt<CCMenuItemToggler*> togglers)
{
    if(!CCMenu::init()) return false;

    m_nValue = 0;

    for (size_t i = 0; i < togglers.size(); ++i)
    {
        auto t = togglers[i];

        this->addChild(t);
        t->setTag(i + 1);
        t->setTarget(this, menu_selector(CCRadioMenu::toggled));

        m_pTogglers.push_back(t);
    }

    return true;
}

void CCRadioMenu::disableAll()
{
    for (const auto& t : m_pTogglers)
    {
        t->toggle(false);
    }

    m_nValue = 0;
}

void CCRadioMenu::toggled(CCObject* sender)
{
    auto toggle = static_cast<CCMenuItemToggler*>(sender);

    for (const auto& t : m_pTogglers)
    {
        if (t->getTag() != toggle->getTag()) 
        {
            t->toggle(false);
        }
    }

    m_nValue = (!toggle->isToggled()) ? toggle->getTag() : 0;
}

int CCRadioMenu::getValue() const
{
    return m_nValue;
}

void CCRadioMenu::setValue(int val)
{
    auto obj = this->getChildByTag(val);

    if(obj != nullptr)
    {
        m_nValue = val;
        static_cast<CCMenuItemToggler*>(obj)->toggle(true);
    }
}