-- Expect the following value to be assigned securely.
local TestLocal = "Secure";
assert(issecure(), "expected state to be secure");
assert(debug.issecurelocal(1, 1), "expected 'TestLocal' to be secure");

-- Assign an insecure value to the local.
forceinsecure();
TestLocal = identity("Insecure");
assert(not issecure(), "expected state to be insecure");
assert(not debug.issecurelocal(1, 1), "expected 'TestLocal' to be insecure");

-- Expect that reading the local will taint execution.
debug.forcesecure();
assert(issecure(), "expected state to be secure");
local TestVariable = TestLocal;
assert(not issecure(), "expected state to be insecure after reading insecure local");

-- Assign a secure value to the local.
debug.forcesecure();
TestLocal = identity("Secure");
assert(issecure(), "expected state to be secure");
assert(debug.issecurelocal(1, 1), "expected 'TestLocal' to be secure");

-- Expect that reading the local will not taint execution.
assert(issecure(), "expected state to be secure");
TestVariable = TestLocal;
assert(issecure(), "expected state to be remain secure after reading secure local");
