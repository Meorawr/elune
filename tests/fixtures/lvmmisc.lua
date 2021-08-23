local function TestNegation()
  local _ = not true;
  assert(not debug.issecurelocal(1, 1), "expected negated value to be tainted");
end

local function TestLength()
  local _ = #"Constant";
  assert(not debug.issecurelocal(1, 1), "expected length of string to be tainted");

  _ = #{ 1, 2, 3 };
  assert(not debug.issecurelocal(1, 1), "expected length of table to be tainted");
end

local function TestConcatenation()
  local _ = "a" .. "b";
  assert(not debug.issecurelocal(1, 1), "expected concatenated string to be tainted");
end

local function TestNumericFor()
  for _ = 1, 10 do
    assert(not debug.issecurelocal(1, 4), "expected loop variable to be tainted");
  end
end

local function TestGenericFor()
  for k, v in pairs(_G) do
    assert(not debug.issecurelocal(1, 4), "expected loop key to be tainted");
    assert(not debug.issecurelocal(1, 5), "expected loop value to be tainted");
    assert(issecurevariable(_G, k), "expected key in table to be secure");
  end
end

local function TestClosure()
  local _ = function() end
  assert(not debug.issecurelocal(1, 1), "expected function value to be tainted");
  assert(not debug.issecureclosure(_), "expected closure object to be tainted");
end

local function TestVararg(...)
  local a, b, c, d = ...;

  assert(not debug.issecurelocal(1, 1), "expected 'a' to be tainted");
  assert(not debug.issecurelocal(1, 2), "expected 'b' to be tainted");
  assert(not debug.issecurelocal(1, 3), "expected 'c' to be tainted");
  assert(not debug.issecurelocal(1, 4), "expected 'd' to be tainted");
end

insecurecall_checked(TestNegation);
insecurecall_checked(TestLength);
insecurecall_checked(TestConcatenation);
insecurecall_checked(TestNumericFor);
insecurecall_checked(TestGenericFor);
insecurecall_checked(TestClosure);
insecurecall_checked(TestVararg, 1, 2, 3);
