-- This test exercises the issecurevariable function. The context is assumed
-- to start in a secure state.
assert(issecure())

SecureGlobal = {}
SecureTable = {}

forceinsecure()
InsecureGlobal = {}
InsecureTable = {}
SecureTable.InsecureField = {}

debug.forcesecure()
InsecureTable.InsecureField = {}

debug.forcesecure()
assert(issecure(), "expected state to be secure")

-- The following tests will verify that the globals have expected security,
-- and that querying shouldn't impact the security of the state.

assert(issecurevariable("SecureGlobal"), "expected 'SecureGlobal' to be secure")
assert(issecure(), "expected state to be secure")

assert(not issecurevariable("InsecureGlobal"), "expected 'InsecureGlobal' to be insecure")
assert(issecure(), "expected state to be secure")

assert(not issecurevariable(SecureTable, "InsecureField"), "expected 'SecureTable.InsecureField' to be insecure")
assert(issecure(), "expected state to be secure")

assert(not issecurevariable(InsecureTable, "InsecureField"), "expected 'InsecureTable.InsecureField' to be insecure")
assert(not issecure(), "expected state to be insecure")   -- Insecure as we read InsecureTable.
