
#include "component.h"

// BUG: does not copy
void Component::copyComponent(const Component &c)
{
	Model = c.Model;
	Name  = c.Name;
	showName = c.showName;
	Description = c.Description;
	_tx = c._tx;
	_ty = c._ty;
}
