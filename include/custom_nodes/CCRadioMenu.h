#include <Geode/utils/cocos.hpp>
using namespace cocos2d;
using namespace geode::prelude;

class CCRadioMenu : public CCMenu {
public:
    static CCRadioMenu* create(CCArrayExt<CCMenuItemToggler*> togglers);
    bool init(CCArrayExt<CCMenuItemToggler*> togglers);

    void toggled(CCObject*);

    int getValue() const;
    void setValue(int);

private:
    CCArrayExt<CCMenuItemToggler*> m_pTogglers;
    int m_nValue;
    void disableAll();
};