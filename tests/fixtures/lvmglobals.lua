-- Expect the following value to be assigned securely.
TestGlobal = "Secure";
assert(issecure(), "expected state to be secure");
assert(issecurevariable("TestGlobal"), "expected 'TestGlobal' to be secure");

-- Assign an insecure value to the global.
forceinsecure();
TestGlobal = identity("Insecure");
assert(not issecure(), "expected state to be insecure");
assert(not issecurevariable("TestGlobal"), "expected 'TestGlobal' to be insecure");

-- Expect that reading the global will taint execution.
debug.forcesecure();
assert(issecure(), "expected state to be secure");
local TestVariable = TestGlobal;
assert(not issecure(), "expected state to be insecure after reading insecure global");

-- Assign a secure value to the global.
debug.forcesecure();
TestGlobal = identity("Secure");
assert(issecure(), "expected state to be secure");
assert(issecurevariable("TestGlobal"), "expected 'TestGlobal' to be secure");

-- Expect that reading the global will not taint execution.
assert(issecure(), "expected state to be secure");
TestVariable = TestGlobal;
assert(issecure(), "expected state to be remain secure after reading secure global");
