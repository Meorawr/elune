-- This script should be loaded securely.
assert(issecure());

RectangleMixin = {};

function RectangleMixin:OnLoad(left, right, top, bottom)
	self:SetSides(left or 0.0, right or 0.0, top or 0.0, bottom or 0.0);
end

function RectangleMixin:SetSides(left, right, top, bottom)
	self.left = left;
	self.right = right;
	self.top = top;
	self.bottom = bottom;
end

function RectangleMixin:Reset()
	self.left = 0.0;
	self.right = 0.0;
	self.top = 0.0;
	self.bottom = 0.0;
end
