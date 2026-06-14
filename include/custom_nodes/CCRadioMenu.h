#include <Geode/utils/cocos.hpp>
using namespace cocos2d;
using namespace geode::prelude;

/* CCRadioMenu works like a CCMenu but only allows an array of CCMenuItemToggler to be passed down (will change soon)

Any toggler that you add will be included for the radio logic

getValue returns the selected toggler
*/
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