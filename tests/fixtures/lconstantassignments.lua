
--[[
  This test is a bit of an anomaly in the whole taint handling process
  and largely dictates why things have been implemented the way that they
  are.

  The core behavior we're attempting to replicate here is that given a
  securely loaded script which defines a function that assigns a constant
  value to a fixed table key, if the call to that function occurs
  insecurely then taint is _not_ applied to the field.

  This can be seen ingame with the following insecurely executed script:

    local rect = CreateFromMixins(RectangleMixin)
    rect:OnLoad()
    assert(rect.left == 0, "expected 'rect.left' to be zero")
    assert(not issecurevariable(rect, "left"), "expected 'rect.left' to be insecure")
    rect:Reset()
    assert(rect.left == 0, "expected 'rect.left' to be zero")
    assert(issecurevariable(rect, "left"), "expected 'rect.left' to be secure")

  The Reset method in question does the following assignments:

    self.left = 0.0;
    self.right = 0.0;
    self.top = 0.0;
    self.bottom = 0.0;

  These end up securely overwriting any existing fields of the same name
  in `self` always, even if called from an insecure context.

  The behavior seen here implies that either taint isn't "strict"; if we
  implement taint through modifications to the setobj macro(s) to taint
  values based on the taint present in the state, these would end up
  insecurely assigned. Similarly this also means that taint shouldn't
  be propagated in the luaV_settable function, since the above snippet
  only generates SETTABLE opcodes.
]]--

forceinsecure();

local rect = CreateFromMixins(RectangleMixin);
rect:OnLoad();
assert(not issecurevariable(rect, "left"), "expected 'rect.left' to be insecure");

rect:Reset();
assert(issecurevariable(rect, "left"), "expected 'rect.left' to be secure");
