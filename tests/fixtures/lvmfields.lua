local TestTable = {};

-- Expect the following value to be assigned securely.
TestTable.field = "Secure";
assert(issecure(), "expected state to be secure");
assert(issecurevariable(TestTable, "field"), "expected 'TestTable.field' to be secure");

-- Assign an insecure value to the field.
forceinsecure();
TestTable.field = identity("Insecure");
assert(not issecure(), "expected state to be insecure");
assert(not issecurevariable(TestTable, "field"), "expected 'TestTable.field' to be insecure");

-- Expect that reading the field will taint execution.
debug.forcesecure();
assert(issecure(), "expected state to be secure");
local TestVariable = TestTable.field;
assert(not issecure(), "expected state to be insecure after reading insecure field");

-- Assign a secure value to the field.
debug.forcesecure();
TestTable.field = identity("Secure");
assert(issecure(), "expected state to be secure");
assert(issecurevariable(TestTable, "field"), "expected 'TestTable.field' to be secure");

-- Expect that reading the field will not taint execution.
assert(issecure(), "expected state to be secure");
TestVariable = TestTable.field;
assert(issecure(), "expected state to be remain secure after reading secure field");
