#include "include/effect.h"

#include <string>
#include "include/fountainEffect.h"

std::shared_ptr<IEffect> EffectFactory::create(const char *name)
{
	std::string effect{ name };
	return std::make_shared<FountainEffect>();
}
