local TestUpvalue;

local function TestFunction()
  -- Expect the following value to be assigned securely.
  TestUpvalue = "Secure";
  assert(issecure(), "expected state to be secure");
  assert(debug.issecureupvalue(TestFunction, 1), "expected 'TestUpvalue' to be secure");

  -- Assign an insecure value to the upvalue.
  forceinsecure();
  TestUpvalue = identity("Insecure");
  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecureupvalue(TestFunction, 1), "expected 'TestUpvalue' to be insecure");

  -- Expect that reading the upvalue will taint execution.
  debug.forcesecure();
  assert(issecure(), "expected state to be secure");
  local TestVariable = TestUpvalue;
  assert(not issecure(), "expected state to be insecure after reading insecure upvalue");

  -- Assign a secure value to the upvalue.
  debug.forcesecure();
  TestUpvalue = identity("Secure");
  assert(issecure(), "expected state to be secure");
  assert(debug.issecureupvalue(TestFunction, 1), "expected 'TestUpvalue' to be secure");

  -- Expect that reading the upvalue will not taint execution.
  assert(issecure(), "expected state to be secure");
  TestVariable = TestUpvalue;
  assert(issecure(), "expected state to be remain secure after reading secure upvalue");
end

TestFunction();
