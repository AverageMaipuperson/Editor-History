#include <Geode/utils/cocos.hpp>
using namespace cocos2d;

class CCRadioMenu : public CCMenu {
    public:
    CCArrayExt<CCMenuItemToggler*> m_togglers;
    CCRadioMenu* create(CCArrayExt<CCMenuItemToggler*> togglers);
    CCRadioMenu* create(std::vector<CCMenuItemToggler*> togglers);
    bool init(CCArrayExt<CCMenuItemToggler*> togglers);
};