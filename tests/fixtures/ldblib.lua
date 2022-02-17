------------------------------------------------------------------------------
-- API COMPATIBILITY
------------------------------------------------------------------------------

-- Expect debug.getinfo() to work as before.
assert(type(debug.getinfo(0)) == "table", "expected debug.getinfo to work")

------------------------------------------------------------------------------
-- THREAD TAINT CONTROL
------------------------------------------------------------------------------

-- Expect to start off secure.
assert(issecure(), "expected thread to be secure");

-- Transition to insecure state.
forceinsecure();
assert(not issecure(), "expected thread to be insecure");

-- Transition back to secure state.
debug.forcesecure();
assert(issecure(), "expected thread to be secure");

-- Use debug.setthreadtaint to make thread insecure.
debug.setthreadtaint("Test");
assert(not issecure(), "expected thread to be insecure");

-- Expect debug.issecurethread to give more details.
do
  local secure, source = debug.issecurethread();
  assert(secure == false, "expected 'issecurethread' to return 'secure == false'");
  assert(source == "Test", "expected 'issecurethread' to return 'source == \"Test\"'");
end

-- The debug.setthreadtaint API should also allow clearing taint on the thread.
debug.setthreadtaint(nil);
assert(issecure(), "expected thread to be secure");

-- ...And during which, debug.issecurethread should return (false, nil).
do
  local secure, source = debug.issecurethread();
  assert(secure == true, "expected 'issecurethread' to return 'secure == true'");
  assert(source == nil, "expected 'issecurethread' to return 'source == nil'");
end

-- The thread-based APIs should correctly work with coroutines. We'll create
-- two threads with different secure statuses, toggle them, and then resume.

local thread1 = securecall_checked(
  function()
    return coroutine.create(
      function()
        assert(not issecure(), "expected 'thread1' to resume insecurely");
      end
    );
  end
);

local thread2 = insecurecall_checked(
  function()
    return coroutine.create(
      function()
        assert(issecure(), "expected 'thread2' to resume securely");
      end
    );
  end
);

assert(debug.issecurethread(thread1), "expected 'thread1' to be secure");
assert(not debug.issecurethread(thread2), "expected 'thread2' to be insecure");

debug.setthreadtaint(thread1, "Bad");
debug.setthreadtaint(thread2, nil);

assert(not debug.issecurethread(thread1), "expected 'thread1' to be insecure");
assert(debug.issecurethread(thread2), "expected 'thread2' to be secure");

coroutine.resume(thread1);
coroutine.resume(thread2);

-- Resuming those threads shouldn't have impacted our own security in any way.
assert(issecure(), "expected thread to remain secure");

------------------------------------------------------------------------------
-- CLOSURE TAINT CONTROL
------------------------------------------------------------------------------

local SecureClosure;
local InsecureClosure;

SecureClosure = insecurecall_checked(
  function()
    return function()
      assert(issecure(), "expected 'SecureClosure' to be invoked securely");
    end;
  end
);

InsecureClosure = securecall_checked(
  function()
    return function()
      assert(not issecure(), "expected 'InsecureClosure' to be invoked insecurely");
    end;
  end
);

-- Expect SecureClosure to be insecure, and InsecureClosure to be secure due
-- to the taint on the thread at the point the closure object was created.

assert(not debug.issecureclosure(SecureClosure), "expected 'SecureClosure' to be insecure");
assert(debug.issecureclosure(InsecureClosure), "expected 'InsecureClosure' to be secure");

-- Now change them and verify.

debug.setclosuretaint(SecureClosure, nil);
debug.setclosuretaint(InsecureClosure, "Test");

assert(debug.issecureclosure(SecureClosure), "expected 'SecureClosure' to be secure");
assert(not debug.issecureclosure(InsecureClosure), "expected 'InsecureClosure' to be insecure");

-- Executing them should test their inner assertions and prove that the taint
-- propagated from the closure for the insecure case.

securecall_checked(SecureClosure);
securecall_checked(InsecureClosure);

------------------------------------------------------------------------------
-- FIELD TAINT CONTROL
------------------------------------------------------------------------------

local TestTable = {};

securecall_checked(function() TestTable[true] = identity("Insecure"); end);
insecurecall_checked(function() TestTable[false] = identity("Secure"); end);

-- Test current security state on the fields; noting that they have non-string
-- or numeric keys which is what issecurevariable would normally only support.

assert(debug.issecurefield(TestTable, true), "expected 'true' field on 'TestTable' to be secure");
assert(not debug.issecurefield(TestTable, false), "expected 'false' field on 'TestTable' to be insecure");

-- Swap security on the fields.

debug.setfieldtaint(TestTable, true, "Test");
debug.setfieldtaint(TestTable, false, nil);

assert(not debug.issecurefield(TestTable, true), "expected 'true' field on 'TestTable' to be insecure");
assert(debug.issecurefield(TestTable, false), "expected 'false' field on 'TestTable' to be secure");

-- Reading those fields should taint as expected.

securecall_checked(
  function()
    nop(TestTable[true]);
    assert(not issecure(), "expected read of 'true' field to taint");
  end
);

securecall_checked(
  function()
    nop(TestTable[false]);
    assert(issecure(), "expected read of 'false' field to not taint");
  end
);

------------------------------------------------------------------------------
-- UPVALUE TAINT CONTROL
------------------------------------------------------------------------------

local TestUpvalue = 100;

local function TestUpvalueFunction()
  local _ = TestUpvalue;
  assert(issecure(), "expected thread to be initially secure");
  assert(debug.issecureupvalue(TestUpvalueFunction, 1), "expected 'TestUpvalue' to be initially secure");
  debug.setupvaluetaint(TestUpvalueFunction, 1, "Bad");
  assert(not debug.issecureupvalue(TestUpvalueFunction, 1), "expected 'TestUpvalue' to be become insecure");
  debug.setupvaluetaint(TestUpvalueFunction, 1, nil);
  assert(debug.issecureupvalue(TestUpvalueFunction, 1), "expected 'TestUpvalue' to be become secure");
  assert(issecure(), "expected thread to remain secure");
end

TestUpvalueFunction();

------------------------------------------------------------------------------
-- LOCAL TAINT CONTROL
------------------------------------------------------------------------------

local function TestLocalFunction()
  local _ = 100;
  assert(issecure(), "expected thread to be initially secure");
  assert(debug.issecurelocal(1, 1), "expected 'TestLocal' to be initially secure");
  debug.setlocaltaint(1, 1, "Bad");
  assert(not debug.issecurelocal(1, 1), "expected 'TestLocal' to be become insecure");
  debug.setlocaltaint(1, 1, nil);
  assert(debug.issecurelocal(1, 1), "expected 'TestLocal' to be become secure");
  assert(issecure(), "expected thread to remain secure");
end

TestLocalFunction();
